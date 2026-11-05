#include "UrlRequest_Base.h"

#include <arcana/threading/task_schedulers.h>
#include <android/asset_manager.h>
#include <AndroidExtensions/Globals.h>
#include <AndroidExtensions/JavaWrappers.h>
#include <gsl/util>
#include <algorithm>

using namespace android::global;
using namespace android::net;
using namespace java::lang;
using namespace java::io;
using namespace java::net;

namespace UrlLib
{
    namespace
    {
        void ThrowIfJavaFaulted(JNIEnv* env)
        {
            if (env->ExceptionCheck())
            {
                const auto exception = env->ExceptionOccurred();
                env->ExceptionClear();
                const auto releaseException = gsl::finally([&] { env->DeleteLocalRef(exception); });
                throw Throwable{exception};
            }
        }

        void WriteRequestBody(OutputStream& stream, const std::string& body)
        {
            auto* env = GetEnvForCurrentThread();
            const auto streamClass = stream.GetClass();
            const auto writeMethod = env->GetMethodID(streamClass, "write", "([BII)V");
            ThrowIfJavaFaulted(env);
            const auto closeMethod = env->GetMethodID(streamClass, "close", "()V");
            ThrowIfJavaFaulted(env);

            if (!body.empty())
            {
                // Write bytes directly: NewStringUTF is NUL-terminated and uses modified UTF-8.
                const auto capacity = std::min(body.size(), size_t{16384});
                const auto buffer = env->NewByteArray(static_cast<jsize>(capacity));
                ThrowIfJavaFaulted(env);
                const auto releaseBuffer = gsl::finally([&] { env->DeleteLocalRef(buffer); });
                for (size_t offset{}; offset < body.size();)
                {
                    const auto count = static_cast<jsize>(std::min(body.size() - offset, capacity));
                    env->SetByteArrayRegion(buffer, 0, count, reinterpret_cast<const jbyte*>(body.data() + offset));
                    ThrowIfJavaFaulted(env);
                    env->CallVoidMethod(stream, writeMethod, buffer, 0, count);
                    ThrowIfJavaFaulted(env);
                    offset += static_cast<size_t>(count);
                }
            }
            env->CallVoidMethod(stream, closeMethod);
            ThrowIfJavaFaulted(env);
        }

        template<typename T> void LoadAsset(AAssetManager* assetManager, const char* url, T& data)
        {
            AAsset* asset = AAssetManager_open(assetManager, url, AASSET_MODE_UNKNOWN);
            if (asset == nullptr)
            {
                throw std::runtime_error("Failed to open asset");
            }

            data.resize(AAsset_getLength64(asset));
            AAsset_read(asset, data.data(), data.size());
            AAsset_close(asset);
        }
    }

    class UrlRequest::Impl : public ImplBase
    {
    public:
        void Open(UrlMethod method, const std::string& url)
        {
            ResetForOpen();
            m_responseBuffer.clear();

            m_method = method;
            Uri uri{Uri::Parse(url.data())};
            // If the URL string doesn't contain a scheme, the URI object's scheme will be null. We throw in this case
            // The path is never null, even if it's empty it will be an empty string
            if ((jstring)uri.getScheme() == nullptr)
            {
                throw std::runtime_error("Cannot parse a URI without a scheme");
            }
            if ((std::string)uri.getScheme() == "app")
            {
                m_schemeIsApp = true;
                m_appPathOrUrl = uri.getPath();
            }
            else
            {
                // Platform API can handle both http:// and file:// schemes
                m_schemeIsApp = false;
                m_appPathOrUrl = std::move(url);
            }
        }

        arcana::task<void, std::exception_ptr> SendAsync()
        {
            return arcana::make_task(arcana::threadpool_scheduler, m_cancellationSource, [this]()
            {
                try
                {
                    if (m_schemeIsApp)
                    {
                        std::string path{m_appPathOrUrl.substr(1)};
                        AAssetManager* assetsManager{GetAppContext().getAssets()};

                        switch (m_responseType)
                        {
                            case UrlResponseType::String:
                            {
                                LoadAsset(assetsManager, path.data(), m_responseString);
                                break;
                            }
                            case UrlResponseType::Buffer:
                            {
                                LoadAsset(assetsManager, path.data(), m_responseBuffer);
                                break;
                            }
                            default:
                            {
                                throw std::runtime_error{"Invalid response type"};
                            }
                        }

                        m_statusCode = UrlStatusCode::Ok;
                    }
                    else
                    {
                        URL url{m_appPathOrUrl.data()};

                        URLConnection connection{url.OpenConnection()};

                        // set request headers
                        for (auto request : m_requestHeaders)
                        {
                            const std::string& key = request.first;
                            const std::string& value = request.second;
                            connection.SetRequestProperty(key, value);
                        }
                        m_requestHeaders.clear();

                        // if this a POST request
                        if (m_method == UrlMethod::Post)
                        {
                            ((HttpURLConnection)connection).SetRequestMethod("POST");
                            connection.SetDoOutput(true);

                            // need to manually set the content length of the request body
                            size_t numBytes = m_requestBody.size();
                            connection.SetRequestProperty("Content-Length", std::to_string(numBytes));

                            OutputStream outputStream{connection.GetOutputStream()};
                            WriteRequestBody(outputStream, m_requestBody);
                        }

                        connection.Connect();
                        if (connection.GetClass().IsAssignableFrom(HttpURLConnection::Class()))
                        {
                            m_statusCode = static_cast<UrlStatusCode>(((HttpURLConnection)connection).GetResponseCode());
                        }
                        else
                        {
                            m_statusCode = UrlStatusCode::Ok;
                        }

                        for (int n = 0;; ++n)
                        {
                            String key = connection.GetHeaderFieldKey(n);
                            String value = connection.GetHeaderField(n);
                            if ((jstring)key == nullptr || (jstring)value == nullptr)
                            {
                                break;
                            }

                            std::string lowerCaseKey = key;
                            ToLower(lowerCaseKey);

                            m_headers.insert({lowerCaseKey, value});
                        }

                        int contentLength = connection.GetContentLength();
                        if (contentLength < 0)
                        {
                            contentLength = 0;
                        }

                        InputStream inputStream{connection.GetInputStream()};
                        ByteArrayOutputStream byteArrayOutputStream{contentLength};

                        ByteArray byteArray{4096};
                        int bytesRead{};
                        while ((bytesRead = inputStream.Read(byteArray)) != -1)
                        {
                            byteArrayOutputStream.Write(byteArray, 0, bytesRead);
                        }

                        switch (m_responseType)
                        {
                            case UrlResponseType::String:
                            {
                                // TODO: use the charset from the content type?
                                m_responseString = byteArrayOutputStream.ToString("UTF-8");
                                break;
                            }
                            case UrlResponseType::Buffer:
                            {
                                m_responseBuffer = byteArrayOutputStream.ToByteArray();
                                break;
                            }
                            default:
                            {
                                throw std::runtime_error{"Invalid response type"};
                            }
                        }

                        // Must happen after getting the content to get the redirected URL.
                        m_responseUrl = connection.GetURL().ToString();
                    }
                }
                catch (const Throwable&)
                {
                    // Catch Java exceptions, but retain the default status code of 0 to indicate a client side error.
                }
            });
        }

        gsl::span<const std::byte> ResponseBuffer() const
        {
            return m_responseBuffer;
        }

    private:
        bool m_schemeIsApp{};
        std::string m_appPathOrUrl{};
        std::vector<std::byte> m_responseBuffer{};
    };
}

#include "UrlRequest_Shared.h"
