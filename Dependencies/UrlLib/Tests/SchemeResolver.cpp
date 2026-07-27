#include <UrlLib/UrlLib.h>

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <cstddef>
#include <future>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{
    std::shared_ptr<const std::vector<std::byte>> MakeBody(const std::string& text)
    {
        const auto* first = reinterpret_cast<const std::byte*>(text.data());
        return std::make_shared<const std::vector<std::byte>>(first, first + text.size());
    }

    // Scheme resolution is synchronous: SendAsync() returns an already-completed task, so the inline
    // continuation runs before this returns. The flag is held by shared_ptr rather than captured by
    // reference so the continuation stays safe even if a caller uses this on a request that was NOT
    // diverted to a resolver and therefore completes later on the real transport.
    bool SendCompletesSynchronously(UrlLib::UrlRequest& request)
    {
        auto completed = std::make_shared<std::atomic<bool>>(false);
        request.SendAsync().then(arcana::inline_scheduler, arcana::cancellation::none(),
            [completed](const arcana::expected<void, std::exception_ptr>&) { completed->store(true); });
        return completed->load();
    }

    // Sends a request that is expected to be served by a scheme resolver, so it settles inline and
    // leaves the request fully populated by the time this returns.
    void Send(UrlLib::UrlRequest& request)
    {
        ASSERT_TRUE(SendCompletesSynchronously(request));
    }

    // Sends a request that goes to the real platform transport and blocks until it settles. A
    // request must never be abandoned while in flight: backends complete on their own thread and
    // write the response into the request's impl through a raw `this`, so letting the request go out
    // of scope first would corrupt whatever memory the impl's allocation is reused for. Returns
    // false if the request did not settle within the timeout.
    bool SendAndWait(UrlLib::UrlRequest& request, std::chrono::seconds timeout = std::chrono::seconds{60})
    {
        auto settled = std::make_shared<std::promise<void>>();
        auto future = settled->get_future();
        request.SendAsync().then(arcana::inline_scheduler, arcana::cancellation::none(),
            [settled](const arcana::expected<void, std::exception_ptr>&) { settled->set_value(); });
        return future.wait_for(timeout) == std::future_status::ready;
    }
}

// A handled resolver populates status, status text, the content-type header and the response body.
TEST(SchemeResolver, HandledResolverPopulatesStringResponse)
{
    const std::string scheme = "urllibtest-string";
    UrlLib::UrlRequest::RegisterSchemeResolver(scheme, [](const std::string&) {
        UrlLib::UrlSchemeResolverResult result;
        result.handled = true;
        result.statusCode = UrlLib::UrlStatusCode::Ok;
        result.statusText = "OK";
        result.contentType = "text/plain";
        result.body = MakeBody("hello");
        return result;
    });

    UrlLib::UrlRequest request;
    request.Open(UrlLib::UrlMethod::Get, scheme + ":anything");
    request.ResponseType(UrlLib::UrlResponseType::String);
    Send(request);

    EXPECT_EQ(request.StatusCode(), UrlLib::UrlStatusCode::Ok);
    EXPECT_EQ(request.StatusText(), "OK");
    EXPECT_EQ(request.ResponseString(), "hello");
    EXPECT_EQ(request.ResponseUrl(), scheme + ":anything");
    ASSERT_TRUE(request.GetResponseHeader("content-type").has_value());
    EXPECT_EQ(*request.GetResponseHeader("content-type"), "text/plain");
    EXPECT_TRUE(request.ErrorSymbol().empty());
    EXPECT_TRUE(request.ErrorString().empty());

    UrlLib::UrlRequest::UnregisterSchemeResolver(scheme);
}

// A handled resolver serves the raw bytes for a Buffer response type.
TEST(SchemeResolver, HandledResolverPopulatesBufferResponse)
{
    const std::string scheme = "urllibtest-buffer";
    UrlLib::UrlRequest::RegisterSchemeResolver(scheme, [](const std::string&) {
        UrlLib::UrlSchemeResolverResult result;
        result.handled = true;
        result.statusCode = UrlLib::UrlStatusCode::Ok;
        result.body = MakeBody("world!");
        return result;
    });

    UrlLib::UrlRequest request;
    request.Open(UrlLib::UrlMethod::Get, scheme + ":anything");
    request.ResponseType(UrlLib::UrlResponseType::Buffer);
    Send(request);

    EXPECT_EQ(request.StatusCode(), UrlLib::UrlStatusCode::Ok);
    const auto buffer = request.ResponseBuffer();
    ASSERT_EQ(buffer.size(), static_cast<size_t>(6));
    EXPECT_EQ(std::string(reinterpret_cast<const char*>(buffer.data()), buffer.size()), "world!");

    UrlLib::UrlRequest::UnregisterSchemeResolver(scheme);
}

// A resolver that reports handled == false (e.g. a revoked blob: URL) surfaces as a transport-style
// error: status stays None (0), an error symbol is recorded, and the response buffer is empty.
TEST(SchemeResolver, NotHandledResolverSurfacesTransportError)
{
    const std::string scheme = "urllibtest-missing";
    UrlLib::UrlRequest::RegisterSchemeResolver(scheme, [](const std::string&) {
        return UrlLib::UrlSchemeResolverResult{}; // handled == false
    });

    UrlLib::UrlRequest request;
    request.Open(UrlLib::UrlMethod::Get, scheme + ":gone");
    request.ResponseType(UrlLib::UrlResponseType::Buffer);
    Send(request);

    EXPECT_EQ(request.StatusCode(), UrlLib::UrlStatusCode::None);
    EXPECT_EQ(request.ErrorSymbol(), "SchemeResolverNotFound");
    EXPECT_FALSE(request.ErrorString().empty());
    EXPECT_TRUE(request.ResponseBuffer().empty());

    UrlLib::UrlRequest::UnregisterSchemeResolver(scheme);
}

// The resolver is consumed on the first SendAsync(); a second send does not re-run it (no repeated
// side effects) while the already-resolved response remains available.
TEST(SchemeResolver, ResolverRunsExactlyOncePerRequest)
{
    const std::string scheme = "urllibtest-once";
    std::atomic<int> calls{0};
    UrlLib::UrlRequest::RegisterSchemeResolver(scheme, [&calls](const std::string&) {
        ++calls;
        UrlLib::UrlSchemeResolverResult result;
        result.handled = true;
        result.statusCode = UrlLib::UrlStatusCode::Ok;
        result.body = MakeBody("once");
        return result;
    });

    UrlLib::UrlRequest request;
    request.Open(UrlLib::UrlMethod::Get, scheme + ":anything");
    request.ResponseType(UrlLib::UrlResponseType::Buffer);
    Send(request);
    Send(request);

    EXPECT_EQ(calls.load(), 1);
    EXPECT_EQ(request.ResponseBuffer().size(), static_cast<size_t>(4));

    UrlLib::UrlRequest::UnregisterSchemeResolver(scheme);
}


// A handled resolver that leaves statusText empty still reports the canonical reason phrase, since
// StatusText() falls back to the code->phrase table for the resolver path exactly as it does for
// the transport path (HTTP/2+ status lines carry no reason phrase).
TEST(SchemeResolver, EmptyStatusTextFallsBackToReasonPhrase)
{
    const std::string scheme = "urllibtest-nostatustext";
    UrlLib::UrlRequest::RegisterSchemeResolver(scheme, [](const std::string&) {
        UrlLib::UrlSchemeResolverResult result;
        result.handled = true;
        result.statusCode = UrlLib::UrlStatusCode::Ok;
        result.body = MakeBody("x");
        return result; // statusText intentionally left empty
    });

    UrlLib::UrlRequest request;
    request.Open(UrlLib::UrlMethod::Get, scheme + ":anything");
    Send(request);

    EXPECT_EQ(request.StatusCode(), UrlLib::UrlStatusCode::Ok);
    EXPECT_EQ(request.StatusText(), "OK");

    UrlLib::UrlRequest::UnregisterSchemeResolver(scheme);
}

// A resolver that throws must not let the exception escape SendAsync(); it is reported through the
// same transport-style error surface as a failed request.
TEST(SchemeResolver, ThrowingResolverSurfacesTransportError)
{
    const std::string scheme = "urllibtest-throws";
    UrlLib::UrlRequest::RegisterSchemeResolver(scheme, [](const std::string&) -> UrlLib::UrlSchemeResolverResult {
        throw std::runtime_error{"resolver blew up"};
    });

    UrlLib::UrlRequest request;
    request.Open(UrlLib::UrlMethod::Get, scheme + ":anything");
    request.ResponseType(UrlLib::UrlResponseType::Buffer);
    EXPECT_NO_THROW(Send(request));

    EXPECT_EQ(request.StatusCode(), UrlLib::UrlStatusCode::None);
    EXPECT_EQ(request.ErrorSymbol(), "SchemeResolverThrew");
    EXPECT_NE(std::string{request.ErrorString()}.find("resolver blew up"), std::string::npos);
    EXPECT_TRUE(request.ResponseBuffer().empty());

    UrlLib::UrlRequest::UnregisterSchemeResolver(scheme);
}

// Removal is explicit: a null resolver is rejected rather than silently unregistering the scheme.
TEST(SchemeResolver, RegisteringNullResolverThrows)
{
    const std::string scheme = "urllibtest-null";
    UrlLib::UrlRequest::RegisterSchemeResolver(scheme, [](const std::string&) {
        UrlLib::UrlSchemeResolverResult result;
        result.handled = true;
        result.statusCode = UrlLib::UrlStatusCode::Ok;
        return result;
    });

    EXPECT_THROW(UrlLib::UrlRequest::RegisterSchemeResolver(scheme, {}), std::invalid_argument);
    EXPECT_THROW(UrlLib::UrlRequest::RegisterSchemeResolver("", [](const std::string&) {
        return UrlLib::UrlSchemeResolverResult{};
    }), std::invalid_argument);

    // The rejected registration left the existing resolver in place.
    UrlLib::UrlRequest request;
    request.Open(UrlLib::UrlMethod::Get, scheme + ":anything");
    Send(request);
    EXPECT_EQ(request.StatusCode(), UrlLib::UrlStatusCode::Ok);

    UrlLib::UrlRequest::UnregisterSchemeResolver(scheme);
}

// After unregistering, the scheme is no longer diverted: the resolver is never invoked again and the
// request falls through to the platform transport. The transport's outcome for an unknown scheme is
// platform-specific (it may throw at Open, fail synchronously, or fail asynchronously), so this
// asserts only the property under test -- that the resolver is no longer consulted.
TEST(SchemeResolver, UnregisterStopsDivertingScheme)
{
    const std::string scheme = "urllibtest-unregister";
    auto calls = std::make_shared<std::atomic<int>>(0);
    UrlLib::UrlRequest::RegisterSchemeResolver(scheme, [calls](const std::string&) {
        ++*calls;
        UrlLib::UrlSchemeResolverResult result;
        result.handled = true;
        result.statusCode = UrlLib::UrlStatusCode::Ok;
        result.body = MakeBody("still here");
        return result;
    });

    UrlLib::UrlRequest resolved;
    resolved.Open(UrlLib::UrlMethod::Get, scheme + ":anything");
    Send(resolved);
    EXPECT_EQ(resolved.StatusCode(), UrlLib::UrlStatusCode::Ok);
    EXPECT_EQ(calls->load(), 1);

    UrlLib::UrlRequest::UnregisterSchemeResolver(scheme);

    // Unregistering an unknown scheme is a no-op, not an error.
    EXPECT_NO_THROW(UrlLib::UrlRequest::UnregisterSchemeResolver(scheme));

    UrlLib::UrlRequest afterUnregister;
    try
    {
        afterUnregister.Open(UrlLib::UrlMethod::Get, scheme + ":anything");

        // Not diverted, so this now goes to the real transport. Wait for it to settle before
        // leaving scope: some backends (e.g. NSURLSession) neither cancel nor keep the impl alive
        // for an abandoned request, and their completion handler would then write the response
        // into freed memory. The transport's verdict on an unknown scheme is platform-specific, so
        // only assert that it is not the resolver's response.
        if (SendAndWait(afterUnregister))
        {
            EXPECT_NE(afterUnregister.StatusCode(), UrlLib::UrlStatusCode::Ok);
        }
        else
        {
            ADD_FAILURE() << "transport request did not settle; leaving it in flight would be unsafe";
        }
    }
    catch (...)
    {
        // Some backends reject an unknown scheme outright at Open(); that is also "not diverted".
    }

    EXPECT_EQ(calls->load(), 1); // the resolver was not consulted after unregistering
}
