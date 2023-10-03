#include "UrlRequest_Base.h"

#include <Unknwn.h>
#include <PathCch.h>
#include <arcana/threading/task_conversions.h>
#include <arcana/threading/task_schedulers.h>
#include <robuffer.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Web.Http.h>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Web.Http.Headers.h>
#include <winapifamily.h>

namespace UrlLib
{
    using namespace winrt::Windows;

    namespace
    {
        Web::Http::HttpMethod ConvertHttpMethod(UrlMethod method)
        {
            switch (method)
            {
                case UrlMethod::Get:
                    return Web::Http::HttpMethod::Get();
                case UrlMethod::Post:
                    return Web::Http::HttpMethod::Post();
                default:
                    throw std::runtime_error("Unsupported method");
            }
        }

        winrt::hstring GetInstalledLocation();

        std::wstring GetLocalPath(Foundation::Uri url)
        {
            std::wstring path{std::wstring_view{Foundation::Uri::UnescapeComponent(url.Path())}.substr(1)};
            std::replace(path.begin(), path.end(), '/', '\\');
            return std::move(path);
        }
    }

    class UrlRequest::Impl : public ImplBase
    {
    public:
        void Open(UrlMethod method, const std::string& url)
        {
            m_method = method;
            m_uri = Foundation::Uri{winrt::to_hstring(url)};
        }

        arcana::task<void, std::exception_ptr> SendAsync();

        gsl::span<const std::byte> ResponseBuffer() const;

    private:
        arcana::task<void, std::exception_ptr> LoadHttpAsync()
        {
            Web::Http::HttpRequestMessage requestMessage;
            requestMessage.RequestUri(m_uri);
            requestMessage.Method(ConvertHttpMethod(m_method));

            std::string contentType;

            for (auto request : m_requestHeaders)
            {
                // content type needs to be set separately
                if (request.first == "Content-Type")
                {
                    contentType = request.second;
                }
                else
                {
                    requestMessage.Headers().Append(winrt::to_hstring(request.first), winrt::to_hstring(request.second));
                }
            }

            m_requestHeaders.clear();

            // check the method
            if (m_method == UrlMethod::Post)
            {
                // if post, set the content type
                requestMessage.Content(Web::Http::HttpStringContent(
                    winrt::to_hstring(m_requestBody),
                    winrt::Windows::Storage::Streams::UnicodeEncoding::Utf8,
                    winrt::to_hstring(contentType))
                );
            }

            Web::Http::HttpClient client;
            return arcana::create_task<std::exception_ptr>(client.SendRequestAsync(requestMessage))
                .then(arcana::inline_scheduler, m_cancellationSource, [this](Web::Http::HttpResponseMessage responseMessage)
                {
                    m_statusCode = static_cast<UrlStatusCode>(responseMessage.StatusCode());
                    if (!responseMessage.IsSuccessStatusCode())
                    {
                        return arcana::task_from_result<std::exception_ptr>();
                    }

                    m_responseUrl = winrt::to_string(responseMessage.RequestMessage().RequestUri().RawUri());

                    for (auto&& iter : responseMessage.Headers())
                    {
                        m_headers.insert(std::make_pair(winrt::to_string(iter.Key()), winrt::to_string(iter.Value())));
                    }
                    // process the content type response header
                    std::string contentTypeValue = winrt::to_string(responseMessage.Content().Headers().ContentType().ToString());
                    std::string contentTypeKey = "content-type";
                    m_headers.insert(std::make_pair(contentTypeKey, contentTypeValue));

                    switch (m_responseType)
                    {
                        case UrlResponseType::String:
                        {
                            return arcana::create_task<std::exception_ptr>(responseMessage.Content().ReadAsStringAsync())
                                .then(arcana::inline_scheduler, m_cancellationSource, [this](winrt::hstring string)
                                {
                                    m_responseString = winrt::to_string(string);
                                    m_statusCode = UrlStatusCode::Ok;
                                });
                        }
                        case UrlResponseType::Buffer:
                        {
                            return arcana::create_task<std::exception_ptr>(responseMessage.Content().ReadAsBufferAsync())
                                .then(arcana::inline_scheduler, m_cancellationSource, [this](Storage::Streams::IBuffer buffer)
                                {
                                    m_responseBuffer = std::move(buffer);
                                    m_statusCode = UrlStatusCode::Ok;
                                });
                        }
                        default:
                        {
                            throw std::runtime_error{"Invalid response type"};
                        }
                    }
                });
        }

#if !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
        arcana::task<void, std::exception_ptr> LoadFileAsync(Storage::StorageFile file);
#else
        arcana::task<void, std::exception_ptr> LoadFileAsync(const std::wstring& path);

        std::vector<std::byte> m_fileResponseBuffer;
#endif

        Storage::Streams::IBuffer m_responseBuffer{};
        Foundation::Uri m_uri{nullptr};
    };
}
