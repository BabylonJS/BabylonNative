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

    // Stable symbolic names for the NSURLErrorDomain codes most likely to be diagnostic in
    // the field. Anything else gets a synthesized "NSURLError_<n>" token. (The numeric code
    // is always reported alongside, so unmapped codes lose nothing but readability.)
    std::string NSURLErrorSymbol(NSInteger code)
    {
        switch (code)
        {
            case NSURLErrorUnknown: return "NSURLErrorUnknown";
            case NSURLErrorCancelled: return "NSURLErrorCancelled";
            case NSURLErrorBadURL: return "NSURLErrorBadURL";
            case NSURLErrorTimedOut: return "NSURLErrorTimedOut";
            case NSURLErrorUnsupportedURL: return "NSURLErrorUnsupportedURL";
            case NSURLErrorCannotFindHost: return "NSURLErrorCannotFindHost";
            case NSURLErrorCannotConnectToHost: return "NSURLErrorCannotConnectToHost";
            case NSURLErrorNetworkConnectionLost: return "NSURLErrorNetworkConnectionLost";
            case NSURLErrorDNSLookupFailed: return "NSURLErrorDNSLookupFailed";
            case NSURLErrorHTTPTooManyRedirects: return "NSURLErrorHTTPTooManyRedirects";
            case NSURLErrorResourceUnavailable: return "NSURLErrorResourceUnavailable";
            case NSURLErrorNotConnectedToInternet: return "NSURLErrorNotConnectedToInternet";
            case NSURLErrorRedirectToNonExistentLocation: return "NSURLErrorRedirectToNonExistentLocation";
            case NSURLErrorBadServerResponse: return "NSURLErrorBadServerResponse";
            case NSURLErrorUserCancelledAuthentication: return "NSURLErrorUserCancelledAuthentication";
            case NSURLErrorUserAuthenticationRequired: return "NSURLErrorUserAuthenticationRequired";
            case NSURLErrorZeroByteResource: return "NSURLErrorZeroByteResource";
            case NSURLErrorCannotDecodeRawData: return "NSURLErrorCannotDecodeRawData";
            case NSURLErrorCannotDecodeContentData: return "NSURLErrorCannotDecodeContentData";
            case NSURLErrorCannotParseResponse: return "NSURLErrorCannotParseResponse";
            case NSURLErrorAppTransportSecurityRequiresSecureConnection: return "NSURLErrorAppTransportSecurityRequiresSecureConnection";
            case NSURLErrorFileDoesNotExist: return "NSURLErrorFileDoesNotExist";
            case NSURLErrorFileIsDirectory: return "NSURLErrorFileIsDirectory";
            case NSURLErrorNoPermissionsToReadFile: return "NSURLErrorNoPermissionsToReadFile";
            case NSURLErrorDataLengthExceedsMaximum: return "NSURLErrorDataLengthExceedsMaximum";
            case NSURLErrorSecureConnectionFailed: return "NSURLErrorSecureConnectionFailed";
            case NSURLErrorServerCertificateHasBadDate: return "NSURLErrorServerCertificateHasBadDate";
            case NSURLErrorServerCertificateUntrusted: return "NSURLErrorServerCertificateUntrusted";
            case NSURLErrorServerCertificateHasUnknownRoot: return "NSURLErrorServerCertificateHasUnknownRoot";
            case NSURLErrorServerCertificateNotYetValid: return "NSURLErrorServerCertificateNotYetValid";
            case NSURLErrorClientCertificateRejected: return "NSURLErrorClientCertificateRejected";
            case NSURLErrorClientCertificateRequired: return "NSURLErrorClientCertificateRequired";
            case NSURLErrorCannotLoadFromNetwork: return "NSURLErrorCannotLoadFromNetwork";
            default: return "NSURLError_" + std::to_string(static_cast<long>(code));
        }
    }

    // Safely convert an NSString* to std::string. [nil UTF8String] (and, rarely, a UTF-8
    // encoding failure) yields a null pointer, and std::string{nullptr} is undefined
    // behavior, so fall back to an empty string.
    std::string ToStdString(NSString* string)
    {
        const char* utf8{[string UTF8String]};
        return utf8 != nullptr ? std::string{utf8} : std::string{};
    }
}

namespace UrlLib
{
    class UrlRequest::Impl : public ImplBase
    {
    public:
        void Open(UrlMethod method, const std::string& url)
        {
            ResetForOpen();
            m_responseBuffer = nil;

            m_method = method;
            m_url = [NSURL URLWithString:[[NSString stringWithUTF8String:url.data()] stringByAddingPercentEncodingWithAllowedCharacters:URLAllowedCharacterSet]];
            if (!m_url || !m_url.scheme)
            {
                throw std::runtime_error{"URL does not have a valid scheme: '" + url + "'"};
            }
            NSString* scheme{m_url.scheme};
            if ([scheme isEqual:@"app"])
            {
                NSString* path = [[NSBundle mainBundle] pathForResource:[m_url.path substringFromIndex:1] ofType:nil];
                if (path == nil)
                {
                    // No bundled resource at this path. Don't throw -- let SendAsync's existing
                    // `if (m_url == nil)` branch complete the task and retain the default status
                    // code of 0 to indicate a client side error. This matches Win32 / UWP / Unix
                    // semantics for missing local files. Record why so ErrorString() consumers
                    // can distinguish a missing bundled asset from a network failure.
                    SetError("urllib", "AppResourceNotFound", 0, "no bundled resource for '" + url + "'");
                    m_url = nil;
                    return;
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

            NSMutableURLRequest* mutableRequest{[request mutableCopy]};

            // set header requests
            for (auto request: m_requestHeaders)
            {
                [mutableRequest setValue:@(request.second.data()) forHTTPHeaderField:@(request.first.data())];
            }

            if (m_method == UrlMethod::Post)
            {
                mutableRequest.HTTPMethod = @"POST";
                // set the body
                NSString* stringBody = [NSString stringWithUTF8String:m_requestBody.data()];
                NSData* requestBodyData = [stringBody dataUsingEncoding:NSUTF8StringEncoding];
                mutableRequest.HTTPBody = requestBodyData;
            }

            request = [mutableRequest copy];

            __block arcana::task_completion_source<void, std::exception_ptr> taskCompletionSource{};

            id completionHandler{^(NSData* data, NSURLResponse* response, NSError* error)
            {
                if (error != nil)
                {
                    // Complete the task, but retain the default status code of 0 to indicate a
                    // client side error -- and record what actually went wrong so consumers can
                    // surface it. NSURLErrorDomain codes get stable symbols; other domains pass
                    // through verbatim. One level of NSUnderlyingErrorKey is appended because
                    // that is where CFNetwork/POSIX specifics (e.g. "Connection refused") live.
                    // Note the human-readable detail is localized by the OS; the domain, symbol,
                    // and numeric code are the stable, filterable parts.
                    std::string domain{"nsurl"};
                    std::string symbol;
                    if ([error.domain isEqualToString:NSURLErrorDomain])
                    {
                        symbol = NSURLErrorSymbol(error.code);
                    }
                    else
                    {
                        domain = ToStdString(error.domain);
                        symbol = "NSError_" + std::to_string(static_cast<long>(error.code));
                    }

                    std::string detail{ToStdString([error localizedDescription])};

                    // Walk the underlying-error chain (bounded), appending only levels that carry
                    // a different numeric code, so POSIX-level specifics like "Connection refused"
                    // surface while the kCFErrorDomainCFNetwork echo of the same code (Apple keeps
                    // NSURL and CFNetwork codes aligned) is skipped.
                    NSInteger previousCode = error.code;
                    NSError* underlying = error.userInfo[NSUnderlyingErrorKey];
                    for (int depth = 0; underlying != nil && depth < 3; ++depth)
                    {
                        if (underlying.code != previousCode)
                        {
                            detail += " <- ";
                            detail += ToStdString(underlying.domain);
                            detail += "(" + std::to_string(static_cast<long>(underlying.code)) + "): ";
                            detail += ToStdString([underlying localizedDescription]);
                        }
                        previousCode = underlying.code;
                        underlying = underlying.userInfo[NSUnderlyingErrorKey];
                    }

                    SetError(domain, symbol, static_cast<int32_t>(error.code), detail);
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
