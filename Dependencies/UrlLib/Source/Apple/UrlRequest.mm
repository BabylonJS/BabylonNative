#if ! __has_feature(objc_arc)
#error "ARC is off"
#endif

#include <UrlLib/UrlLib.h>
#include <arcana/threading/task.h>
#include <arcana/threading/task_schedulers.h>

#import <Foundation/Foundation.h>

namespace UrlLib
{
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
            NSString* urlString = [NSString stringWithUTF8String:url.data()];
            NSURL* nsURL{[NSURL URLWithString:urlString]};
            if (!nsURL || !nsURL.scheme)
            {
                throw std::runtime_error{"URL does not have a valid scheme"};
            }
            NSString* scheme{nsURL.scheme};
            if ([scheme isEqual:@"app"])
            {
                NSString* path{[[NSBundle mainBundle] pathForResource:nsURL.path ofType:nil]};
                if (path == nil)
                {
                    throw std::runtime_error{"No file exists at local path"};
                }
                nsURL = [NSURL fileURLWithPath:path];
            }
            m_nsURL = nsURL; // Only store the URL if we didn't throw
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
            if (m_nsURL == nil)
            {
                // Complete the task, but retain the default status code of 0 to indicate a client side error.
                return arcana::task_from_result<std::exception_ptr>();
            }
            NSURLSession* session{[NSURLSession sharedSession]};
            NSURLRequest* request{[NSURLRequest requestWithURL:m_nsURL]};

            __block arcana::task_completion_source<void, std::exception_ptr> taskCompletionSource{};

            id completionHandler{^(NSData* data, NSURLResponse* response, NSError* error)
            {
                if (error != nil)
                {
                    // Complete the task, but retain the default status code of 0 to indicate a client side error.
                    // TODO: Consider logging or otherwise exposing the error message in some way via: [[error localizedDescription] UTF8String]
                    taskCompletionSource.complete();
                    return;
                }
                
                if ([response class] == [NSHTTPURLResponse class])
                {
                    NSHTTPURLResponse* httpResponse{(NSHTTPURLResponse*)response};
                    m_statusCode = static_cast<UrlStatusCode>(httpResponse.statusCode);
                }
                else
                {
                    m_statusCode = UrlStatusCode::Ok;
                }

                if (data != nil)
                {
                    switch (m_responseType)
                    {
                        case UrlResponseType::String:
                        {
                            m_responseString = std::string{static_cast<const char*>(data.bytes), data.length};
                            break;
                        }
                        case UrlResponseType::Buffer:
                        {
                            m_responseBuffer = data;
                            break;
                        }
                        default:
                        {
                            taskCompletionSource.complete(arcana::make_unexpected(std::make_exception_ptr(std::runtime_error{"Invalid response type"})));
                        }
                    }
                }
                
                taskCompletionSource.complete();
            }};

            NSURLSessionDataTask* task{[session dataTaskWithRequest:request completionHandler:completionHandler]};
            [task resume];

            return taskCompletionSource.as_task();
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
            if (m_responseBuffer)
            {
                return {reinterpret_cast<const std::byte*>(m_responseBuffer.bytes), static_cast<long>(m_responseBuffer.length)};
            }

            return {};
        }

    private:
        arcana::cancellation_source m_cancellationSource{};
        UrlResponseType m_responseType{UrlResponseType::String};
        UrlMethod m_method{UrlMethod::Get};
        NSURL* m_nsURL{};
        UrlStatusCode m_statusCode{UrlStatusCode::None};
        std::string m_responseUrl{};
        std::string m_responseString{};
        NSData* m_responseBuffer{};
    };
}

#include <Shared/UrlRequest.h>
