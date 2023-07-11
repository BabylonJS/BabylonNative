#if !__has_feature(objc_arc)
#error "ARC is off"
#endif

#include "UrlRequest_Base.h"

#import <Foundation/Foundation.h>

namespace
{
    auto URLAllowedCharacterSet = []()
    {
        NSRange range;
        range.location = 0x21;
        range.length = 0x7e - range.location + 1;
        return [NSCharacterSet characterSetWithRange:range];
    }();
}

namespace UrlLib
{
    class UrlRequest::Impl : public ImplBase
    {
    public:
        void Open(UrlMethod method, const std::string& url)
        {
            m_method = method;
            m_url = [NSURL URLWithString:[[NSString stringWithUTF8String:url.data()] stringByAddingPercentEncodingWithAllowedCharacters:URLAllowedCharacterSet]];
            if (!m_url || !m_url.scheme)
            {
                throw std::runtime_error{"URL does not have a valid scheme"};
            }
            NSString* scheme{m_url.scheme};
            if ([scheme isEqual:@"app"])
            {
                NSString* path = [[NSBundle mainBundle] pathForResource:[m_url.path substringFromIndex:1] ofType:nil];
                if (path == nil)
                {
                    throw std::runtime_error{"No file exists at local path"};
                }
                m_url = [NSURL fileURLWithPath:path];
            }
        }

        arcana::task<void, std::exception_ptr> SendAsync()
        {
            if (m_url == nil)
            {
                // Complete the task, but retain the default status code of 0 to indicate a client side error.
                return arcana::task_from_result<std::exception_ptr>();
            }
            NSURLSession* session{[NSURLSession sharedSession]};
            NSURLRequest* request{[NSURLRequest requestWithURL:m_url]};

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
                    
                    for (id key in httpResponse.allHeaderFields)
                    {
                        id value = [httpResponse.allHeaderFields objectForKey:key];
                        m_headers.insert({ToLower([key UTF8String]), [value UTF8String]});
                    }
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

        gsl::span<const std::byte> ResponseBuffer() const
        {
            if (m_responseBuffer)
            {
                return {reinterpret_cast<const std::byte*>(m_responseBuffer.bytes), m_responseBuffer.length};
            }

            return {};
        }

    private:
        NSURL* m_url{};
        NSData* m_responseBuffer{};
    };
}

#include "UrlRequest_Shared.h"
