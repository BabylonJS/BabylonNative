#include <UrlLib/UrlLib.h>
#include <arcana/threading/task.h>
#include <arcana/threading/task_schedulers.h>
#include <android/asset_manager.h>
#include <AndroidExtensions/Globals.h>
#include <AndroidExtensions/JavaWrappers.h>

using namespace android::global;
using namespace android::net;
using namespace java::lang;
using namespace java::io;
using namespace java::net;

namespace UrlLib
{
    namespace
    {
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

    class UrlRequest::Impl
    {
    public:
        ~Impl()
        {
            Abort();
        }

        void Abort()
        {
            m_cancellationSource.cancel();
        }

        void Open(UrlMethod method, std::string url)
        {
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

        UrlResponseType ResponseType() const
        {
            return m_responseType;
        }

        void ResponseType(UrlResponseType value)
        {
            m_responseType = value;
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
                        connection.Connect();

                        if (connection.GetClass().IsAssignableFrom(HttpURLConnection::Class()))
                        {
                            m_statusCode = static_cast<UrlStatusCode>(((HttpURLConnection)connection).GetResponseCode());
                        }
                        else
                        {
                            m_statusCode = UrlStatusCode::Ok;
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

        UrlStatusCode StatusCode() const
        {
            return m_statusCode;
        }

        gsl::cstring_span<> ResponseUrl()
        {
            return m_responseUrl;
        }

        gsl::cstring_span<> ResponseString()
        {
            return m_responseString;
        }

        gsl::span<const std::byte> ResponseBuffer() const
        {
            return m_responseBuffer;
        }

    private:
        arcana::cancellation_source m_cancellationSource{};
        UrlResponseType m_responseType{UrlResponseType::String};
        UrlMethod m_method{UrlMethod::Get};
        bool m_schemeIsApp{};
        std::string m_appPathOrUrl{};
        UrlStatusCode m_statusCode{UrlStatusCode::None};
        std::string m_responseUrl{};
        std::string m_responseString{};
        std::vector<std::byte> m_responseBuffer{};
    };
}

#include <Shared/UrlRequest.h>
