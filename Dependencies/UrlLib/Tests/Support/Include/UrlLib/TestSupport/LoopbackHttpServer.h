// A minimal HTTP/1.1 server on 127.0.0.1 for tests, so XMLHttpRequest / fetch / UrlRequest
// behavior can be asserted without reaching a public host. It is limited to plain BSD sockets /
// Winsock so it builds for every platform that runs the unit tests, and the socket headers stay
// in LoopbackHttpServer.cpp.
//
// It serves a fixed set of resources. Their paths and behavior mirror the web-platform-tests
// (WPT) resources that the ported tests request, so a test reads the same here as it does in WPT
// (https://github.com/web-platform-tests/wpt, 3-Clause BSD License; see NOTICE.md):
//
//   /xhr/resources/headers.asis, headers-basic.asis, headers-www-authenticate.asis,
//       header-content-length.asis, headers-double-empty.asis
//                                    fixed raw responses (odd header casing, duplicate names)
//   /xhr/resources/headers.py        200 with a few custom headers, body "TEST"
//   /xhr/resources/status.py         ?code=&text=&content=&type=  any status line and body
//   /xhr/resources/redirect.py       ?location=&code=             redirect, relative to this path
//   /xhr/resources/content.py        ?content=                    echoes request details
//   /xhr/resources/delay.py          ?ms=                         "TEST_DELAY" after a delay
//   /xhr/resources/well-formed.xml   a small static document
//   /fetch/api/resources/redirect.py ?redirect_status=&location=&max_count=   redirect chain
//
// Anything else is a 404. Every response closes its connection, which keeps the server to one
// request per socket.

#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace UrlLib::TestSupport
{
    class LoopbackHttpServer final
    {
    public:
        // Starts listening on an ephemeral port of 127.0.0.1.
        LoopbackHttpServer();
        ~LoopbackHttpServer();

        LoopbackHttpServer(const LoopbackHttpServer&) = delete;
        LoopbackHttpServer& operator=(const LoopbackHttpServer&) = delete;

        // False when the listening socket could not be created.
        bool Valid() const;

        uint16_t Port() const;

        // "http://127.0.0.1:<port>", without a trailing slash.
        std::string Origin() const;

        // "http://127.0.0.1:<port>" for a port that refuses connections: the port is reserved by
        // binding to it and released again, so nothing is listening. Empty on failure.
        static std::string ClosedPortOrigin();

    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;
    };
}
