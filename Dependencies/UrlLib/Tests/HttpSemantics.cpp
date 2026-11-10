// Tests for the HTTP response rules the backends share (Source/UrlRequest_Http.h). They are pure
// functions, so the behavior that needs an HTTP/2 or HTTP/3 server to observe end to end is
// covered here on every platform: web-platform-tests xhr/status.h2.window.js and
// fetch/api/basic/status.h2.any.js require statusText to be "" for a response over HTTP/2.

#include <UrlRequest_Http.h>

#include <gtest/gtest.h>

#include <string>
#include <unordered_map>

using namespace UrlLib::Http;

// xhr/status.h2.window.js, fetch/api/basic/status.h2.any.js: no reason phrase over HTTP/2 (and
// HTTP/3), whatever the status code.
TEST(HttpSemantics, StatusTextIsEmptyWithoutReasonPhraseOnTheWire)
{
    for (const int status : {200, 210, 400, 404, 410, 500, 502})
    {
        EXPECT_EQ(StatusText(ReasonPhrase::None, "", status), "") << status;
        // A phrase a platform API made up for the response is not reported either.
        EXPECT_EQ(StatusText(ReasonPhrase::None, "OK", status), "") << status;
    }
}

// xhr/status-async.htm: the phrase of an HTTP/1.x status line is reported as sent.
TEST(HttpSemantics, StatusTextIsTheReasonPhraseOnTheWire)
{
    EXPECT_EQ(StatusText(ReasonPhrase::OnWire, "OK", 200), "OK");
    EXPECT_EQ(StatusText(ReasonPhrase::OnWire, "UNICORNSWIN", 204), "UNICORNSWIN");
    EXPECT_EQ(StatusText(ReasonPhrase::OnWire, "402 TEH AWESOME", 402), "402 TEH AWESOME");
    EXPECT_EQ(StatusText(ReasonPhrase::OnWire, "lowercase", 502), "lowercase");
    EXPECT_EQ(StatusText(ReasonPhrase::OnWire, "WAY OUTTA RANGE", 699), "WAY OUTTA RANGE");
    // "HTTP/1.1 404 " is a valid status line with an empty phrase.
    EXPECT_EQ(StatusText(ReasonPhrase::OnWire, "", 404), "");
}

// Where the platform does not report the phrase, the standard phrase for the code stands in.
TEST(HttpSemantics, StatusTextFallsBackToTheStandardPhrase)
{
    EXPECT_EQ(StatusText(ReasonPhrase::Unknown, "", 200), "OK");
    EXPECT_EQ(StatusText(ReasonPhrase::Unknown, "", 404), "Not Found");
    EXPECT_EQ(StatusText(ReasonPhrase::Unknown, "", 503), "Service Unavailable");
    EXPECT_EQ(StatusText(ReasonPhrase::Unknown, "", 699), "");
    // A phrase the caller supplied (a scheme resolver's statusText) wins over the table.
    EXPECT_EQ(StatusText(ReasonPhrase::Unknown, "Fine", 200), "Fine");
}

TEST(HttpSemantics, RecognizesHttp2AndLaterProtocolNames)
{
    EXPECT_TRUE(IsHttp2OrLater("h2"));
    EXPECT_TRUE(IsHttp2OrLater("h2c"));
    EXPECT_TRUE(IsHttp2OrLater("h3"));
    EXPECT_TRUE(IsHttp2OrLater("h3-29"));
    EXPECT_FALSE(IsHttp2OrLater("http/1.1"));
    EXPECT_FALSE(IsHttp2OrLater("http/1.0"));
    EXPECT_FALSE(IsHttp2OrLater("h"));
    EXPECT_FALSE(IsHttp2OrLater(""));
}

TEST(HttpSemantics, ParsesHttp1StatusLines)
{
    {
        const StatusLine line = ParseStatusLine("HTTP/1.1 200 OK\r\n");
        EXPECT_EQ(line.majorVersion, 1);
        EXPECT_EQ(line.reasonPhrase, ReasonPhrase::OnWire);
        EXPECT_EQ(line.phrase, "OK");
    }
    {
        const StatusLine line = ParseStatusLine("HTTP/1.0 402 402 TEH AWESOME\r\n");
        EXPECT_EQ(line.majorVersion, 1);
        EXPECT_EQ(line.reasonPhrase, ReasonPhrase::OnWire);
        EXPECT_EQ(line.phrase, "402 TEH AWESOME");
    }
    {
        // No phrase, with and without the space that should precede it.
        EXPECT_EQ(ParseStatusLine("HTTP/1.1 404 \r\n").reasonPhrase, ReasonPhrase::OnWire);
        EXPECT_EQ(ParseStatusLine("HTTP/1.1 404 \r\n").phrase, "");
        EXPECT_EQ(ParseStatusLine("HTTP/1.1 404\r\n").phrase, "");
        EXPECT_EQ(ParseStatusLine("HTTP/1.1 404").phrase, "");
    }
}

// libcurl writes "HTTP/2 200" and "HTTP/3 200" for responses that have no status line.
TEST(HttpSemantics, ParsesHttp2AndHttp3StatusLines)
{
    for (const char* text : {"HTTP/2 200 \r\n", "HTTP/2 200\r\n", "HTTP/3 404 \r\n", "HTTP/3 404"})
    {
        const StatusLine line = ParseStatusLine(text);
        EXPECT_GE(line.majorVersion, 2) << text;
        EXPECT_EQ(line.reasonPhrase, ReasonPhrase::None) << text;
        EXPECT_EQ(line.phrase, "") << text;
        EXPECT_EQ(StatusText(line.reasonPhrase, line.phrase, 200), "") << text;
    }
}

TEST(HttpSemantics, DoesNotTakeAHeaderForAStatusLine)
{
    EXPECT_EQ(ParseStatusLine("Content-Type: text/plain\r\n").majorVersion, 0);
    EXPECT_EQ(ParseStatusLine("HTTP-Custom: 1\r\n").majorVersion, 0);
    EXPECT_EQ(ParseStatusLine("HTTP/").majorVersion, 0);
    EXPECT_EQ(ParseStatusLine("\r\n").majorVersion, 0);
    EXPECT_EQ(ParseStatusLine("").majorVersion, 0);
}

TEST(HttpSemantics, RemovesTheFragment)
{
    EXPECT_EQ(WithoutFragment("http://127.0.0.1/a?b=c#d"), "http://127.0.0.1/a?b=c");
    EXPECT_EQ(WithoutFragment("http://127.0.0.1/#"), "http://127.0.0.1/");
    EXPECT_EQ(WithoutFragment("http://127.0.0.1/a"), "http://127.0.0.1/a");
    EXPECT_EQ(WithoutFragment(""), "");
}

// xhr/getallresponseheaders.htm and fetch/api/basic/header-value-combining.any.js: names compare
// case-insensitively and the values of a repeated name are joined with ", " in arrival order.
TEST(HttpSemantics, CombinesRepeatedHeaders)
{
    std::unordered_map<std::string, std::string> headers{};
    AddHeader(headers, "foo-TEST", "1");
    AddHeader(headers, "FOO-test", "2");
    AddHeader(headers, "__Custom", "token");
    AddHeader(headers, "double-trouble", "");
    AddHeader(headers, "double-trouble", "");

    ASSERT_EQ(headers.size(), static_cast<size_t>(3));
    EXPECT_EQ(headers["foo-test"], "1, 2");
    EXPECT_EQ(headers["__custom"], "token");
    EXPECT_EQ(headers["double-trouble"], ", ");
}
