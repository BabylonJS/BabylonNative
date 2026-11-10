#include <UrlLib/UrlLib.h>
#include <gtest/gtest.h>

#include <atomic>
#include <optional>
#include <string>

namespace
{
    void Send(UrlLib::UrlRequest& request, bool cancelled = false)
    {
        auto complete = std::make_shared<std::atomic<bool>>(false);
        request.SendAsync().then(arcana::inline_scheduler, arcana::cancellation::none(),
            [complete, cancelled](const arcana::expected<void, std::exception_ptr>& result) {
                EXPECT_EQ(result.has_error(), cancelled);
                complete->store(true);
            });
        ASSERT_TRUE(complete->load());
    }

    struct DataCase
    {
        const char* url;
        std::string bytes;
        const char* contentType;
    };

    class DataUrl : public testing::TestWithParam<DataCase> {};
}

TEST_P(DataUrl, DecodesStringAndBuffer)
{
    const auto& value = GetParam();
    for (auto type : {UrlLib::UrlResponseType::String, UrlLib::UrlResponseType::Buffer})
    {
        UrlLib::UrlRequest request;
        request.Open(UrlLib::UrlMethod::Get, value.url);
        request.ResponseType(type);
        Send(request);
        EXPECT_EQ(request.StatusCode(), UrlLib::UrlStatusCode::Ok);
        EXPECT_EQ(request.StatusText(), "OK");
        EXPECT_EQ(request.GetResponseHeader("Content-Type"), std::optional<std::string>{value.contentType});
        EXPECT_TRUE(request.ErrorString().empty());
        EXPECT_TRUE(request.ErrorSymbol().empty());
        EXPECT_EQ(request.ErrorCode(), 0);
        if (type == UrlLib::UrlResponseType::String)
        {
            EXPECT_EQ(request.ResponseString(), value.bytes);
        }
        else
        {
            const auto body = request.ResponseBuffer();
            ASSERT_EQ(body.size(), value.bytes.size());
            if (!body.empty())
            {
                EXPECT_EQ(std::string(reinterpret_cast<const char*>(body.data()), body.size()), value.bytes);
            }
        }
    }
}

INSTANTIATE_TEST_SUITE_P(Supported, DataUrl, testing::Values(
    DataCase{"data:,hello%20world", "hello world", "text/plain;charset=US-ASCII"},
    DataCase{"data:,", "", "text/plain;charset=US-ASCII"},
    DataCase{"data:;base64,", "", "text/plain;charset=US-ASCII"},
    DataCase{"data:text/plain,hello+world%2C%25", "hello+world,%", "text/plain"},
    DataCase{"DATA:TEXT/PLAIN;CHARSET=UTF-8,%C3%A9", "\xC3\xA9", "text/plain;charset=UTF-8"},
    DataCase{"data:;charset=utf-8,%E2%82%AC", "\xE2\x82\xAC", "text/plain;charset=utf-8"},
    DataCase{"data:application/octet-stream,%00%7f%80%ff", std::string{"\0\x7f\x80\xff", 4}, "application/octet-stream"},
    DataCase{"data:image/png;base64,iVBORw0KGgo=", "\x89PNG\r\n\x1a\n", "image/png"},
    DataCase{"data:;BASE64,Zg==", "f", "text/plain;charset=US-ASCII"},
    DataCase{"data:;base64,Zg", "f", "text/plain;charset=US-ASCII"},
    DataCase{"data:;base64,Zm8", "fo", "text/plain;charset=US-ASCII"},
    DataCase{"data:;base64,Zm9v", "foo", "text/plain;charset=US-ASCII"},
    DataCase{"data:;base64,Zh==", "f", "text/plain;charset=US-ASCII"},
    DataCase{"data:;base64,Z%20g%3D%3D%0D%0A%09%0C", "f", "text/plain;charset=US-ASCII"},
    DataCase{"data:application/octet-stream; base64,%2F%2B%2F%2B", "\xff\xef\xfe", "application/octet-stream"},
    DataCase{"data:text/plain,x?y#ignored", "x?y", "text/plain"},
    DataCase{"data:text/plain,%23fragment%3Fquery", "#fragment?query", "text/plain"},
    DataCase{"data:text/plain,a,b,c", "a,b,c", "text/plain"},
    DataCase{"data:text/plain,%oops%2%", "%oops%2%", "text/plain"},
    DataCase{"data:text/plain,\xC3\xA9", "\xC3\xA9", "text/plain"},
    DataCase{"data:text/plain;note=\"a;b\",x", "x", "text/plain;note=\"a;b\""},
    DataCase{"data:text/plain;note=\"a\\\"b\",x", "x", "text/plain;note=\"a\\\"b\""},
    DataCase{"data:text/plain;base64=not-a-marker,Zg==", "Zg==", "text/plain;base64=not-a-marker"},
    DataCase{"data:;base64,Zg==#ignored", "f", "text/plain;charset=US-ASCII"}
));

TEST(DataUrlFailures, MalformedUrlsReportErrorsWithoutPartialResponse)
{
    for (const auto* url : {
        "data:", "data:text/plain", "data:text/plain#fragment,not-body",
        "data:invalid,x", "data:text/,x", "data:/plain,x", "data:text/plain;broken,x",
        "data:text/plain;charset=,x", "data:text/plain;charset=\"broken,x",
        "data:;base64,A", "data:;base64,Zg=", "data:;base64,Zg===", "data:;base64,Zm=8",
        "data:;base64,!!!!", "data:;base64,Zg==junk", "data:;base64,_w==",
        "data:;base64,%FF", "data:;base64,====", "data:;base64,Zg==?query",
        "data:text/plain;note=\"a\\\nb\",x"})
    {
        SCOPED_TRACE(url);
        UrlLib::UrlRequest request;
        request.Open(UrlLib::UrlMethod::Get, url);
        request.ResponseType(UrlLib::UrlResponseType::Buffer);
        Send(request);
        EXPECT_EQ(request.StatusCode(), UrlLib::UrlStatusCode::None);
        EXPECT_TRUE(request.StatusText().empty());
        EXPECT_EQ(request.ErrorSymbol(), "DataUrlInvalid");
        EXPECT_FALSE(request.ErrorString().empty());
        EXPECT_TRUE(request.GetAllResponseHeaders().empty());
        EXPECT_TRUE(request.ResponseBuffer().empty());
        EXPECT_TRUE(request.ResponseString().empty());
    }
}

TEST(DataUrlLifecycle, RejectsPost)
{
    UrlLib::UrlRequest request;
    request.Open(UrlLib::UrlMethod::Post, "data:,hello");
    request.SetRequestBody("ignored");
    Send(request);
    EXPECT_EQ(request.StatusCode(), UrlLib::UrlStatusCode::None);
    EXPECT_EQ(request.ErrorSymbol(), "DataUrlUnsupportedMethod");
    EXPECT_TRUE(request.GetAllResponseHeaders().empty());
    EXPECT_TRUE(request.ResponseString().empty());
}

TEST(DataUrlLifecycle, AbortBeforeSendReturnsCancelledTask)
{
    UrlLib::UrlRequest request;
    request.Open(UrlLib::UrlMethod::Get, "data:,hello");
    request.Abort();
    Send(request, true);
    EXPECT_EQ(request.StatusCode(), UrlLib::UrlStatusCode::None);
    EXPECT_TRUE(request.ResponseString().empty());
    EXPECT_TRUE(request.GetAllResponseHeaders().empty());
}

TEST(DataUrlLifecycle, ReopenClearsErrorBodyAndHeaders)
{
    UrlLib::UrlRequest request;
    request.Open(UrlLib::UrlMethod::Get, "data:;base64,!!!");
    Send(request);
    ASSERT_FALSE(request.ErrorString().empty());
    request.Open(UrlLib::UrlMethod::Get, "data:,hello#fragment");
    Send(request);
    EXPECT_TRUE(request.ErrorString().empty());
    EXPECT_EQ(request.ResponseUrl(), "data:,hello");
    EXPECT_EQ(request.ResponseString(), "hello");
    Send(request);
    EXPECT_EQ(request.ResponseString(), "hello");
    request.Abort();
    EXPECT_EQ(request.StatusCode(), UrlLib::UrlStatusCode::Ok);
    request.Open(UrlLib::UrlMethod::Get, "data:,");
    EXPECT_EQ(request.StatusCode(), UrlLib::UrlStatusCode::None);
    EXPECT_TRUE(request.ResponseString().empty());
    EXPECT_TRUE(request.ResponseBuffer().empty());
    EXPECT_TRUE(request.GetAllResponseHeaders().empty());
}

TEST(DataUrlLifecycle, UnregisterDoesNotRemoveBuiltin)
{
    UrlLib::UrlRequest::UnregisterSchemeResolver("data");
    UrlLib::UrlRequest request;
    request.Open(UrlLib::UrlMethod::Get, "data:,builtin");
    Send(request);
    EXPECT_EQ(request.StatusCode(), UrlLib::UrlStatusCode::Ok);
    EXPECT_EQ(request.ResponseString(), "builtin");
}

TEST(DataUrlLifecycle, CustomOverrideAndRestoration)
{
    UrlLib::UrlRequest::RegisterSchemeResolver("DATA", [](const std::string&) {
        UrlLib::UrlSchemeResolverResult result;
        result.handled = true;
        result.statusCode = static_cast<UrlLib::UrlStatusCode>(201);
        return result;
    });
    UrlLib::UrlRequest overridden;
    overridden.Open(UrlLib::UrlMethod::Get, "data:,builtin");
    UrlLib::UrlRequest::UnregisterSchemeResolver("data");
    Send(overridden);
    EXPECT_EQ(static_cast<int>(overridden.StatusCode()), 201);
    EXPECT_TRUE(overridden.ResponseString().empty());
    overridden.Open(UrlLib::UrlMethod::Get, "data:,builtin");
    Send(overridden);
    EXPECT_EQ(overridden.StatusCode(), UrlLib::UrlStatusCode::Ok);
    EXPECT_EQ(overridden.ResponseString(), "builtin");
}

TEST(DataUrlBytes, EntireByteRange)
{
    UrlLib::UrlRequest request;
    request.Open(UrlLib::UrlMethod::Get,
        "data:application/octet-stream;base64,"
        "AAECAwQFBgcICQoLDA0ODxAREhMUFRYXGBkaGxwdHh8gISIjJCUmJygpKissLS4v"
        "MDEyMzQ1Njc4OTo7PD0+P0BBQkNERUZHSElKS0xNTk9QUVJTVFVWV1hZWltcXV5f"
        "YGFiY2RlZmdoaWprbG1ub3BxcnN0dXZ3eHl6e3x9fn+AgYKDhIWGh4iJiouMjY6P"
        "kJGSk5SVlpeYmZqbnJ2en6ChoqOkpaanqKmqq6ytrq+wsbKztLW2t7i5uru8vb6/"
        "wMHCw8TFxsfIycrLzM3Oz9DR0tPU1dbX2Nna29zd3t/g4eLj5OXm5+jp6uvs7e7v"
        "8PHy8/T19vf4+fr7/P3+/w==");
    request.ResponseType(UrlLib::UrlResponseType::Buffer);
    Send(request);
    ASSERT_EQ(request.StatusCode(), UrlLib::UrlStatusCode::Ok);
    const auto bytes = request.ResponseBuffer();
    ASSERT_EQ(bytes.size(), 256u);
    for (size_t index = 0; index < bytes.size(); ++index)
    {
        EXPECT_EQ(bytes[index], static_cast<std::byte>(index));
    }
}
