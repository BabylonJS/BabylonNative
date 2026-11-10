// Tests for what UrlRequest reports about an HTTP response -- status text, response URL and the
// redirected flag -- against an HTTP/1.1 server on the loopback interface, so they do not depend
// on a public host.

#include <UrlLib/UrlLib.h>
#include <UrlLib/TestSupport/LoopbackHttpServer.h>

#include <arcana/threading/cancellation.h>

#include <gtest/gtest.h>

#include <chrono>
#include <future>
#include <memory>
#include <string>

namespace
{
    // Sends the request and blocks until it settles. False on timeout.
    [[nodiscard]] bool SendAndWait(UrlLib::UrlRequest& request)
    {
        auto done = std::make_shared<std::promise<void>>();
        auto future = done->get_future();

        request.SendAsync().then(arcana::inline_scheduler, arcana::cancellation::none(),
            [done](const arcana::expected<void, std::exception_ptr>&) {
                done->set_value();
            });

        if (future.wait_for(std::chrono::seconds{30}) != std::future_status::ready)
        {
            request.Abort();
            return false;
        }

        return true;
    }

    class UrlRequestHttpResponse : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            ASSERT_TRUE(m_server.Valid());
        }

        std::string Url(const std::string& pathAndQuery) const
        {
            return m_server.Origin() + pathAndQuery;
        }

        UrlLib::TestSupport::LoopbackHttpServer m_server{};
    };
}

// libcurl and Windows report the reason phrase of an HTTP/1.x status line; NSURLSession does not,
// and there the status text is the standard phrase for the status code.
TEST_F(UrlRequestHttpResponse, StatusTextOfAnHttp1Response)
{
    UrlLib::UrlRequest request{};
    request.Open(UrlLib::UrlMethod::Get, Url("/xhr/resources/status.py?code=200&text=Fine&type=text%2Fplain&content=body"));
    ASSERT_TRUE(SendAndWait(request));

    EXPECT_EQ(static_cast<int>(request.StatusCode()), 200);
    EXPECT_EQ(request.ResponseString(), "body");
#if defined(__APPLE__)
    EXPECT_EQ(request.StatusText(), "OK");
#else
    EXPECT_EQ(request.StatusText(), "Fine");
#endif
}

TEST_F(UrlRequestHttpResponse, StatusTextOfAnErrorStatus)
{
    UrlLib::UrlRequest request{};
    request.Open(UrlLib::UrlMethod::Get, Url("/xhr/resources/status.py?code=404&text=NOT%20HERE&type=text%2Fplain"));
    ASSERT_TRUE(SendAndWait(request));

    // An HTTP error status is a response, not a transport failure.
    EXPECT_EQ(static_cast<int>(request.StatusCode()), 404);
    EXPECT_TRUE(request.ErrorString().empty()) << request.ErrorString();
#if defined(__APPLE__)
    EXPECT_EQ(request.StatusText(), "Not Found");
#else
    EXPECT_EQ(request.StatusText(), "NOT HERE");
#endif
}

TEST_F(UrlRequestHttpResponse, ResponseUrlOfADirectResponseExcludesTheFragment)
{
    UrlLib::UrlRequest request{};
    EXPECT_EQ(request.ResponseUrl(), "");

    request.Open(UrlLib::UrlMethod::Get, Url("/xhr/resources/well-formed.xml#fragment"));
    EXPECT_EQ(request.ResponseUrl(), "");
    ASSERT_TRUE(SendAndWait(request));

    EXPECT_EQ(static_cast<int>(request.StatusCode()), 200);
    EXPECT_EQ(request.ResponseUrl(), Url("/xhr/resources/well-formed.xml"));
    EXPECT_FALSE(request.Redirected());
}

TEST_F(UrlRequestHttpResponse, ResponseUrlOfAnErrorStatus)
{
    UrlLib::UrlRequest request{};
    request.Open(UrlLib::UrlMethod::Get, Url("/xhr/resources/foo.html"));
    ASSERT_TRUE(SendAndWait(request));

    EXPECT_EQ(static_cast<int>(request.StatusCode()), 404);
    EXPECT_EQ(request.ResponseUrl(), Url("/xhr/resources/foo.html"));
    EXPECT_FALSE(request.Redirected());
}

TEST_F(UrlRequestHttpResponse, ResponseUrlAfterRedirects)
{
    // redirect.py -> redirect.py -> content.py
    UrlLib::UrlRequest request{};
    request.Open(UrlLib::UrlMethod::Get, Url("/xhr/resources/redirect.py?code=301&location=redirect.py%3Flocation%3Dcontent.py"));
    ASSERT_TRUE(SendAndWait(request));

    EXPECT_EQ(static_cast<int>(request.StatusCode()), 200);
    EXPECT_EQ(request.ResponseUrl(), Url("/xhr/resources/content.py"));
    EXPECT_TRUE(request.Redirected());
}

TEST_F(UrlRequestHttpResponse, ResponseUrlIsEmptyAfterATransportFailure)
{
    const std::string closedPort = UrlLib::TestSupport::LoopbackHttpServer::ClosedPortOrigin();
    ASSERT_FALSE(closedPort.empty());

    UrlLib::UrlRequest request{};
    request.Open(UrlLib::UrlMethod::Get, closedPort + "/");
    ASSERT_TRUE(SendAndWait(request));

    EXPECT_EQ(request.StatusCode(), UrlLib::UrlStatusCode::None);
    EXPECT_EQ(request.ResponseUrl(), "");
    EXPECT_EQ(request.StatusText(), "");
    EXPECT_FALSE(request.Redirected());
}
