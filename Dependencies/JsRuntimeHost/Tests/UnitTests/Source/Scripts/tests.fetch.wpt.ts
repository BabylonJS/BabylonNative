// fetch() conformance tests ported from the web-platform-tests project
// (https://github.com/web-platform-tests/wpt, directory fetch/api/), adapted to mocha/chai.
//
// Copyright © web-platform-tests contributors. Distributed under the 3-Clause BSD License; the
// full text is in NOTICE.md at the root of this repository.
//
// Each test names the WPT file it comes from. The requests go to the loopback HTTP server that the
// test host starts (UrlLib/TestSupport/LoopbackHttpServer.h), which serves the WPT resources these
// tests use under the same paths.

import { expect } from "chai";

// What the platform HTTP stack lets UrlLib observe; see tests.xmlHttpRequest.wpt.ts.
const exposesReasonPhrase = hostPlatform === "Unix" || hostPlatform === "Win32";
const exposesContentLengthHeader = hostPlatform !== "Win32";

async function rejection(promise: Promise<unknown>): Promise<any> {
    try {
        await promise;
    } catch (e) {
        return e;
    }
    return undefined;
}

describe("fetch (web-platform-tests)", function () {
    // The timeout of a test in the WPT harness.
    this.timeout(10000);

    const xhrResources = loopbackHttpOrigin + "/xhr/resources/";
    const fetchResources = loopbackHttpOrigin + "/fetch/api/resources/";

    // fetch/api/basic/header-value-combining.any.js. Left out, as in tests.xmlHttpRequest.wpt.ts:
    // header-content-length-twice.asis and headers-some-are-empty.asis.
    const combinedHeaders: Array<[string, string, string]> = [
        ["content-length", "0", "header-content-length"],
        ["double-trouble", ", ", "headers-double-empty"],
        ["foo-test", "1, 2, 3", "headers-basic"],
        ["www-authenticate", "1, 2, 3, 4", "headers-www-authenticate"],
    ];
    for (const [name, value, resource] of combinedHeaders) {
        it(`response.headers.get('${name}') expects ${value}`, async function () {
            if (name === "content-length" && !exposesContentLengthHeader) {
                this.skip();
            }
            const response = await fetch(xhrResources + resource + ".asis");
            expect(response.headers.get(name)).to.equal(value);
        });
    }

    // fetch/api/headers/headers-combine.any.js ("Iterate combined values in sorted order"), against
    // the headers of a response: names lower-cased and sorted, values combined. The polyfill's
    // headers object iterates through forEach() only.
    it("iterates response headers with combined values in sorted order", async function () {
        const response = await fetch(xhrResources + "headers.asis");
        const expected = [["__custom", "token"], ["also-here", "Mr. PB"], ["ewok", "lego"], ["foo-test", "1, 2"]];
        const visited: string[][] = [];
        response.headers.forEach((value: string, name: string) => {
            visited.push([name, value]);
        });
        expect(visited).to.deep.equal(expected);
    });

    // xhr/status-async.htm through fetch(): an HTTP/1.1 response keeps the reason phrase of its
    // status line. fetch/api/basic/status.h2.any.js (no reason phrase over HTTP/2, so statusText is
    // "") is covered at the UrlLib seam in UrlLib/Tests/HttpSemantics.cpp.
    it("statusText is the reason phrase of an HTTP/1.1 response", async function () {
        const ok = await fetch(xhrResources + "status.py?code=200&text=" + encodeURIComponent("OH HELLO"));
        expect(ok.status).to.equal(200);
        expect(ok.ok).to.equal(true);
        expect(ok.statusText).to.equal(exposesReasonPhrase ? "OH HELLO" : "OK");

        const notFound = await fetch(xhrResources + "status.py?code=404&text=" + encodeURIComponent("NOT HERE"));
        expect(notFound.status).to.equal(404);
        expect(notFound.ok).to.equal(false);
        expect(notFound.statusText).to.equal(exposesReasonPhrase ? "NOT HERE" : "Not Found");
    });

    // fetch/api/basic/response-url.sub.any.js. Left out: the URL with no path at all, where the
    // platform URL parsers differ on adding the "/".
    for (const [fetched, expected] of [["/ada", "/ada"], ["/#", "/"], ["/#ada", "/"]]) {
        it(`Testing response url getter with ${fetched}`, async function () {
            const response = await fetch(loopbackHttpOrigin + fetched);
            expect(response.url).to.equal(loopbackHttpOrigin + expected);
        });
    }

    // fetch/api/redirect/redirect-method.any.js
    it("Response.redirected should be false on not-redirected responses", async function () {
        const response = await fetch(xhrResources + "content.py");
        expect(response.status).to.equal(200);
        expect(response.type).to.equal("basic");
        expect(response.redirected).to.equal(false);
        expect(response.url).to.equal(xhrResources + "content.py");
    });

    // fetch/api/redirect/redirect-method.any.js
    for (const status of [301, 302, 303, 307]) {
        it(`Redirect ${status} with GET`, async function () {
            const response = await fetch(xhrResources + "redirect.py?code=" + status + "&location=content.py");
            expect(response.status).to.equal(200);
            expect(response.type).to.equal("basic");
            expect(response.headers.get("x-request-method")).to.equal("GET");
            expect(response.redirected).to.equal(true);
            expect(response.url).to.equal(xhrResources + "content.py");
        });
    }

    // fetch/api/redirect/redirect-count.any.js. WPT follows 20 redirects and expects the 21st to
    // fail, which is the limit of the Fetch standard; the platform HTTP stacks have their own
    // limits (NSURLSession stops earlier), so this follows a chain every stack accepts.
    for (const status of [301, 302, 303, 307, 308]) {
        it(`Redirect ${status} 5 times`, async function () {
            const maxCount = 5;
            const url = fetchResources + "redirect.py?redirect_status=" + status + "&max_count=" + maxCount + "&location=" + encodeURIComponent(fetchResources + "redirect.py");
            const response = await fetch(url);
            expect(response.status).to.equal(200);
            expect(response.redirected).to.equal(true);
            expect(await response.text()).to.equal(maxCount.toString());
        });
    }

    // Not from WPT (the Fetch standard, "fetch response handover"): an HTTP error status is a
    // response like any other, so the promise resolves.
    it("resolves for an HTTP error status", async function () {
        for (const status of [400, 404, 500, 503]) {
            const response = await fetch(xhrResources + "status.py?code=" + status);
            expect(response.status).to.equal(status);
            expect(response.ok).to.equal(false);
            expect(response.redirected).to.equal(false);
        }
    });

    // Not from WPT (the Fetch standard): a network error rejects with a TypeError.
    it("rejects with a TypeError for a refused connection", async function () {
        const error = await rejection(fetch(loopbackClosedPortOrigin + "/"));
        expect(error).to.be.an.instanceof(TypeError);
    });

    // fetch/api/basic/scheme-others.sub.any.js, for the first few of its schemes.
    for (const scheme of ["aaa", "cap", "cid"]) {
        it(`Fetching ${scheme}://127.0.0.1/ is KO`, async function () {
            const error = await rejection(fetch(loopbackHttpOrigin.replace("http", scheme) + "/"));
            expect(error).to.be.an.instanceof(TypeError);
        });
    }
});
