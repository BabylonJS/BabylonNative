// XMLHttpRequest conformance tests ported from the web-platform-tests project
// (https://github.com/web-platform-tests/wpt, directory xhr/), adapted to mocha/chai.
//
// Copyright © web-platform-tests contributors. Distributed under the 3-Clause BSD License; the
// full text is in NOTICE.md at the root of this repository.
//
// Each test names the WPT file it comes from. The requests go to the loopback HTTP server that the
// test host starts (UrlLib/TestSupport/LoopbackHttpServer.h), which serves the WPT resources these
// tests use under the same paths. Differences from the WPT sources:
// * Only asynchronous GET is used: the polyfill has no synchronous mode and UrlLib has no HEAD or
//   custom methods, so those rows are left out. There is no XMLHttpRequestUpload, so the upload
//   events are left out of the expected event sequences.
// * There is no ProgressEvent or Document in this environment, so `instanceof ProgressEvent` and
//   responseXML assertions are left out.
// * UrlLib delivers a response in one piece, so a response is observed as HEADERS_RECEIVED,
//   LOADING and one progress event back to back, not incrementally.

import { expect } from "chai";

// What the platform HTTP stack lets UrlLib observe; see the UrlLib README.
// The reason phrase of an HTTP/1.x status line: libcurl and WinRT report it, NSURLSession and the
// Android wrapper do not, and there statusText is the standard phrase for the status code.
const exposesReasonPhrase = hostPlatform === "Unix" || hostPlatform === "Win32";
// The body of a response with an error status is not read by the Win32 and Android backends.
const exposesErrorStatusBody = hostPlatform !== "Win32" && hostPlatform !== "Android";
// WinRT keeps the entity headers (Content-Length, ...) apart from the response headers, and
// UrlLib only copies Content-Type from them.
const exposesContentLengthHeader = hostPlatform !== "Win32";

const standardReasonPhrases: { [status: number]: string } = {
    200: "OK", 204: "No Content", 401: "Unauthorized", 402: "Payment Required", 404: "Not Found",
    502: "Bad Gateway", 503: "Service Unavailable", 699: "",
};

function expectedStatusText(status: number, phraseOnWire: string): string {
    return exposesReasonPhrase ? phraseOnWire : standardReasonPhrases[status];
}

// The subset of the testharness.js async_test API that the ported tests use. Exceptions thrown by
// an event handler do not reach mocha, so every handler is wrapped in a step that fails the test.
interface AsyncTest {
    step_func<T extends (...args: any[]) => void>(func: T): T;
    step_func_done<T extends (...args: any[]) => void>(func?: T): T;
    unreached_func(description: string): () => void;
    step_timeout(func: () => void, milliseconds: number): void;
    done(): void;
}

function async_test(body: (test: AsyncTest) => void): Promise<void> {
    return new Promise<void>((resolve, reject) => {
        const step = (func: (...args: any[]) => void, args: any[]) => {
            try {
                func(...args);
            } catch (e) {
                reject(e);
            }
        };
        const test: AsyncTest = {
            step_func: (func: any) => ((...args: any[]) => step(func, args)) as any,
            step_func_done: (func?: any) => ((...args: any[]) => {
                if (func) {
                    step(func, args);
                }
                resolve();
            }) as any,
            unreached_func: (description: string) => () => reject(new Error(description)),
            step_timeout: (func: () => void, milliseconds: number) => { setTimeout(() => step(func, []), milliseconds); },
            done: resolve,
        };
        step(() => body(test), []);
    });
}

// xhr/resources/xmlhttprequest-event-order.js: readystatechange is recorded as the readyState
// number, every other event as "type(loaded,total,lengthComputable)".
function prepare_xhr_for_event_order_test(xhr: XMLHttpRequest): Array<number | string> {
    const recorded: Array<number | string> = [];
    xhr.addEventListener("readystatechange", () => {
        recorded.push(xhr.readyState);
    });
    for (const type of ["loadstart", "progress", "abort", "timeout", "error", "load", "loadend"]) {
        xhr.addEventListener(type, (e: any) => {
            recorded.push(e.type + "(" + e.loaded + "," + e.total + "," + e.lengthComputable + ")");
        });
    }
    return recorded;
}

describe("XMLHttpRequest (web-platform-tests)", function () {
    // The timeout of a test in the WPT harness.
    this.timeout(10000);

    const resources = loopbackHttpOrigin + "/xhr/resources/";

    // Response header values combined per name, shared by getAllResponseHeaders() and
    // getResponseHeader(). Left out: header-content-length-twice.asis and
    // headers-some-are-empty.asis, where the platform HTTP stacks disagree on what they report for
    // a repeated Content-Length and for control characters in a value.
    const combinedHeaders: Array<[string, string, string]> = [
        ["content-length", "0", "header-content-length"],
        ["double-trouble", ", ", "headers-double-empty"],
        ["foo-test", "1, 2, 3", "headers-basic"],
        ["www-authenticate", "1, 2, 3, 4", "headers-www-authenticate"],
    ];

    describe("getAllResponseHeaders()", function () {
        // xhr/getallresponseheaders.htm
        it("returns the headers lower-cased, combined and sorted", function () {
            return async_test((t) => {
                const client = new XMLHttpRequest();
                client.onload = t.step_func_done(() => {
                    expect(client.getAllResponseHeaders()).to.equal("also-here: Mr. PB\r\newok: lego\r\nfoo-test: 1, 2\r\n__custom: token\r\n");
                });
                client.onerror = t.unreached_func("unexpected error");
                client.open("GET", resources + "headers.asis");
                client.send(null);
            });
        });

        // xhr/getallresponseheaders.htm
        for (const [name, value, resource] of combinedHeaders) {
            it(`returns "${name}: ${value}" for ${resource}.asis`, function () {
                if (name === "content-length" && !exposesContentLengthHeader) {
                    this.skip();
                }
                return async_test((t) => {
                    const client = new XMLHttpRequest();
                    client.onload = t.step_func_done(() => {
                        expect(client.getAllResponseHeaders()).to.equal(name + ": " + value + "\r\n");
                    });
                    client.onerror = t.unreached_func("unexpected error");
                    client.open("GET", resources + resource + ".asis");
                    client.send();
                });
            });
        }

        // xhr/getallresponseheaders-status.htm
        it("excludes the status line and is empty before the headers are received", function () {
            return async_test((t) => {
                const client = new XMLHttpRequest();
                expect(client.getAllResponseHeaders()).to.equal("");
                client.onreadystatechange = t.step_func(() => {
                    const headers = client.getAllResponseHeaders().toLowerCase();
                    if (client.readyState === 1) {
                        expect(headers).to.equal("");
                    }
                    if (client.readyState > 1) {
                        expect(headers.indexOf("200 ok")).to.equal(-1);
                        expect(headers.indexOf("http/1.")).to.equal(-1);
                    }
                    if (client.readyState === 4) {
                        t.done();
                    }
                });
                client.open("GET", resources + "headers.py");
                client.send(null);
            });
        });
    });

    describe("getResponseHeader()", function () {
        // xhr/getresponseheader.any.js
        for (const [name, value, resource] of combinedHeaders) {
            it(`getResponseHeader('${name}') expects ${value}`, function () {
                if (name === "content-length" && !exposesContentLengthHeader) {
                    this.skip();
                }
                return async_test((t) => {
                    const client = new XMLHttpRequest();
                    client.onload = t.step_func_done(() => {
                        expect(client.getResponseHeader(name)).to.equal(value);
                    });
                    client.onerror = t.unreached_func("unexpected error");
                    client.open("GET", resources + resource + ".asis");
                    client.send();
                });
            });
        }

        // xhr/getresponseheader-case-insensitive.htm
        it("matches the header name case-insensitively", function () {
            return async_test((t) => {
                const client = new XMLHttpRequest();
                client.onreadystatechange = t.step_func(() => {
                    if (client.readyState === 4) {
                        expect(client.getResponseHeader("x-custom-header")).to.equal("test");
                        expect(client.getResponseHeader("X-Custom-Header")).to.equal("test");
                        expect(client.getResponseHeader("X-CUSTOM-HEADER")).to.equal("test");
                        expect(client.getResponseHeader("X-custom-HEADER")).to.equal("test");
                        expect(client.getResponseHeader("X-CUSTOM-header-COMMA")).to.equal("1, 2");
                        expect(client.getResponseHeader("X-CUSTOM-no-such-header-in-response")).to.equal(null);
                        expect(client.getResponseHeader("CONTENT-TYPE")).to.equal("text/plain");
                        t.done();
                    }
                });
                client.open("GET", resources + "headers.py");
                client.send(null);
            });
        });
    });

    describe("status and statusText", function () {
        // xhr/status-async.htm. The loopback server speaks HTTP/1.1, which carries a reason phrase;
        // xhr/status.h2.window.js (no reason phrase over HTTP/2, so statusText is "") is covered at
        // the UrlLib seam in UrlLib/Tests/HttpSemantics.cpp.
        const statuses: Array<[number, string, string, string]> = [
            [204, "UNICORNSWIN", "", ""],
            [401, "OH HELLO", "Not today.", ""],
            [402, "FIVE BUCKS", "<x>402</x>", "text/xml"],
            [402, "FREE", "Nice!", "text/doesnotmatter"],
            [402, "402 TEH AWESOME", "", ""],
            [502, "YO", "", ""],
            [502, "lowercase", "SWEET POTATO", "text/plain"],
            [503, "HOUSTON WE HAVE A", "503", "text/plain"],
            [699, "WAY OUTTA RANGE", "699", "text/plain"],
        ];
        for (const [code, text, content, type] of statuses) {
            it(`reports status ${code} "${text}"`, function () {
                return async_test((t) => {
                    const client = new XMLHttpRequest();
                    client.onreadystatechange = t.step_func(() => {
                        if (client.readyState > 1) {
                            expect(client.status).to.equal(code);
                            expect(client.statusText).to.equal(expectedStatusText(code, text));
                            expect(client.getResponseHeader("X-Request-Method")).to.equal("GET");
                            if (client.readyState === 4) {
                                if (code < 400 || exposesErrorStatusBody) {
                                    expect(client.responseText).to.equal(content);
                                }
                                t.done();
                            }
                        } else {
                            expect(client.status).to.equal(0);
                            expect(client.statusText).to.equal("");
                        }
                    });
                    client.open("GET", resources + "status.py?code=" + encodeURIComponent(code) + "&text=" + encodeURIComponent(text) + "&content=" + encodeURIComponent(content) + "&type=" + encodeURIComponent(type));
                    client.send(null);
                });
            });
        }

        // xhr/status-error.htm
        for (const code of [200, 400, 401, 404, 410, 500, 699]) {
            it(`does not fire error for HTTP status ${code}`, function () {
                return async_test((t) => {
                    const client = new XMLHttpRequest();
                    client.open("GET", resources + "status.py?code=" + code);
                    client.onreadystatechange = t.step_func(() => {
                        expect(client.response).to.equal("", "response data");
                        expect(client.status).to.equal(code, "response status");
                        if (client.readyState === client.DONE) {
                            // Give extra time for a bogus error event to pop up.
                            t.step_timeout(() => { t.done(); }, 100);
                        }
                    });
                    client.onerror = t.unreached_func("HTTP error should not throw error event");
                    client.send();
                });
            });
        }
    });

    describe("responseURL", function () {
        // xhr/responseurl.html
        it("404 response has proper responseURL", function () {
            return async_test((t) => {
                const client = new XMLHttpRequest();
                expect(client.responseURL).to.equal("");
                client.onloadend = t.step_func_done(() => {
                    expect(client.status).to.equal(404);
                    expect(client.responseURL).to.equal(resources + "foo.html");
                });
                client.open("GET", resources + "foo.html");
                client.send();
            });
        });

        // xhr/responseurl.html
        it("redirected response has proper responseURL", function () {
            return async_test((t) => {
                const client = new XMLHttpRequest();
                expect(client.responseURL).to.equal("");
                const target = "well-formed.xml";
                client.onloadend = t.step_func_done(() => {
                    expect(client.status).to.equal(200);
                    expect(client.responseURL).to.equal(resources + target);
                });
                client.open("GET", resources + "redirect.py?location=" + target);
                client.send();
            });
        });

        // Not from WPT: the XMLHttpRequest standard serializes the response URL without its fragment.
        it("excludes the fragment", function () {
            return async_test((t) => {
                const client = new XMLHttpRequest();
                client.onloadend = t.step_func_done(() => {
                    expect(client.status).to.equal(200);
                    expect(client.responseURL).to.equal(resources + "well-formed.xml");
                });
                client.open("GET", resources + "well-formed.xml#fragment");
                client.send();
            });
        });

        // xhr/responseurl-after-abort.any.js
        it("abort() during DONE clears responseURL", function () {
            return async_test((t) => {
                const client = new XMLHttpRequest();
                client.onload = t.step_func_done(() => {
                    expect(client.readyState).to.equal(4);
                    expect(client.responseURL).to.not.equal("", "responseURL before abort()");
                    client.abort();
                    expect(client.readyState).to.equal(0);
                    expect(client.status).to.equal(0, "status");
                    expect(client.statusText).to.equal("", "statusText");
                    expect(client.getAllResponseHeaders()).to.equal("", "getAllResponseHeaders()");
                    expect(client.responseURL).to.equal("", "responseURL");
                });
                client.open("GET", resources + "well-formed.xml");
                client.send(null);
            });
        });

        // xhr/responseurl-after-abort.any.js
        it("responseURL reflects the new response after abort() and reuse", function () {
            return async_test((t) => {
                const url = resources + "well-formed.xml";
                const client = new XMLHttpRequest();
                client.open("GET", url);
                client.send(null);
                client.abort();
                expect(client.status).to.equal(0, "status");
                expect(client.statusText).to.equal("", "statusText");
                expect(client.getAllResponseHeaders()).to.equal("", "getAllResponseHeaders()");
                expect(client.responseURL).to.equal("", "responseURL");
                // A reused XMLHttpRequest must report the new response's URL, not the aborted one.
                client.onload = t.step_func_done(() => {
                    expect(client.status).to.equal(200);
                    expect(client.responseURL).to.equal(url + "?reused");
                });
                client.open("GET", url + "?reused");
                client.send(null);
            });
        });
    });

    describe("events", function () {
        // xhr/event-load.any.js
        it("fires load right after readystatechange to DONE", function () {
            return async_test((t) => {
                const client = new XMLHttpRequest();
                let done = false;
                client.onload = t.step_func((e: any) => {
                    expect(e.type).to.equal("load");
                    expect(client.readyState).to.equal(4);
                    done = true;
                    t.done();
                });
                client.onreadystatechange = t.step_func(() => {
                    if (client.readyState !== 4) {
                        return;
                    }
                    t.step_timeout(() => {
                        expect(done, "Didn't get load event within 4ms of readystatechange==4").to.equal(true);
                    }, 4);
                });
                client.open("GET", resources + "well-formed.xml");
                client.send(null);
            });
        });

        // xhr/event-loadstart.any.js
        it("fires loadstart in the OPENED state", function () {
            return async_test((t) => {
                const client = new XMLHttpRequest();
                let done = false;
                client.onloadstart = t.step_func((e: any) => {
                    expect(e.type).to.equal("loadstart");
                    expect(client.readyState).to.equal(1);
                    done = true;
                    t.done();
                });
                t.step_timeout(() => {
                    expect(done, "onloadstart not called after 500 ms").to.equal(true);
                }, 500);
                client.open("GET", resources + "well-formed.xml");
                client.send(null);
            });
        });

        // xhr/event-loadend.any.js
        it("fires loadend", function () {
            return async_test((t) => {
                const client = new XMLHttpRequest();
                let done = false;
                client.onloadend = t.step_func((e: any) => {
                    expect(e.type).to.equal("loadend");
                    done = true;
                    t.done();
                });
                client.onreadystatechange = t.step_func(() => {
                    if (client.readyState !== 4) {
                        return;
                    }
                    t.step_timeout(() => {
                        expect(done, "onloadend not called after 100 ms").to.equal(true);
                    }, 100);
                });
                client.open("GET", resources + "well-formed.xml");
                client.send(null);
            });
        });

        // xhr/event-readystatechange-loaded.any.js. WPT expects LOADING more than once for a
        // trickled response; UrlLib delivers the response in one piece, so it is seen exactly once.
        it("goes through OPENED, HEADERS_RECEIVED, LOADING and DONE in order", function () {
            return async_test((t) => {
                const client = new XMLHttpRequest();
                const states: number[] = [];
                client.onreadystatechange = t.step_func(() => {
                    states.push(client.readyState);
                    if (client.readyState === 4) {
                        expect(states).to.deep.equal([1, 2, 3, 4]);
                        t.done();
                    }
                });
                client.open("GET", resources + "well-formed.xml");
                client.send(null);
            });
        });

        // xhr/send-response-event-order.htm, as a GET that asks content.py for the same 12 bytes.
        it("fires the events of a successful request in order", function () {
            return async_test((t) => {
                const xhr = new XMLHttpRequest();
                const recorded = prepare_xhr_for_event_order_test(xhr);
                xhr.addEventListener("loadend", t.step_func_done(() => {
                    expect(recorded).to.deep.equal([1, "loadstart(0,0,false)", 2, 3, "progress(12,12,true)", 4, "load(12,12,true)", "loadend(12,12,true)"]);
                }));
                xhr.open("GET", resources + "content.py?content=Test%20Message", true);
                xhr.send();
            });
        });

        // xhr/event-error.sub.any.js, with a refused connection in place of a host that does not resolve.
        it("fires error for a network error", function () {
            return async_test((t) => {
                const client = new XMLHttpRequest();
                client.onerror = t.step_func_done((e: any) => {
                    expect(e.type).to.equal("error");
                });
                client.open("GET", loopbackClosedPortOrigin + "/");
                client.send("null");
            });
        });

        // xhr/send-network-error-async-events.sub.htm
        it("fires the events of a network error in order", function () {
            return async_test((t) => {
                const xhr = new XMLHttpRequest();
                const expected = ["loadstart", 4, "error", "loadend"];
                const actual: Array<number | string> = [];
                xhr.onreadystatechange = t.step_func(() => {
                    if (xhr.readyState === 4) {
                        actual.push(xhr.readyState);
                    }
                });
                xhr.onloadstart = t.step_func((e: any) => { actual.push(e.type); });
                xhr.onloadend = t.step_func_done((e: any) => {
                    actual.push(e.type);
                    expect(actual).to.deep.equal(expected);
                });
                xhr.onerror = t.step_func((e: any) => { actual.push(e.type); });
                xhr.open("GET", loopbackClosedPortOrigin + "/", true);
                xhr.send();
            });
        });

        // xhr/xmlhttprequest-network-error.htm, with a refused connection in place of a redirect loop.
        it("has empty members during a network error", function () {
            return async_test((t) => {
                const client = new XMLHttpRequest();
                client.onreadystatechange = t.step_func(() => {
                    expect(client.status).to.equal(0, "status");
                    expect(client.statusText).to.equal("", "statusText");
                    expect(client.getAllResponseHeaders()).to.equal("", "getAllResponseHeaders");
                    expect(client.getResponseHeader("content-type")).to.equal(null, "getResponseHeader");
                    expect(client.responseText).to.equal("", "responseText");
                    expect(client.responseURL).to.equal("", "responseURL");
                    if (client.readyState === 4) {
                        t.done();
                    }
                });
                client.open("GET", loopbackClosedPortOrigin + "/");
                client.send(null);
            });
        });
    });

    describe("abort()", function () {
        // xhr/event-abort.any.js
        it("fires abort", function () {
            return async_test((t) => {
                const client = new XMLHttpRequest();
                let done = false;
                client.onabort = t.step_func(() => {
                    done = true;
                    t.done();
                });
                client.open("GET", resources + "well-formed.xml");
                client.send(null);
                client.abort();
                t.step_timeout(() => {
                    expect(done, "onabort not called after 4 ms").to.equal(true);
                }, 4);
            });
        });

        // xhr/abort-after-send.any.js
        it("after send() fires readystatechange, abort and loadend synchronously and resets the request", function () {
            return async_test((t) => {
                const client = new XMLHttpRequest();
                let control_flag = false;
                const recorded = prepare_xhr_for_event_order_test(client);
                client.addEventListener("readystatechange", t.step_func(() => {
                    if (client.readyState === 4) {
                        control_flag = true;
                        expect(client.responseText).to.equal("");
                        expect(client.status).to.equal(0);
                        expect(client.statusText).to.equal("");
                        expect(client.getAllResponseHeaders()).to.equal("");
                        expect(client.getResponseHeader("Content-Type")).to.equal(null);
                    }
                }));
                client.open("GET", resources + "well-formed.xml", true);
                client.send(null);
                client.abort();
                expect(control_flag).to.equal(true);
                expect(client.readyState).to.equal(0);
                expect(recorded).to.deep.equal([1, "loadstart(0,0,false)", 4, "abort(0,0,false)", "loadend(0,0,false)"]);
                // Not in WPT: the aborted transfer must not produce more events when it settles.
                t.step_timeout(() => {
                    expect(recorded).to.have.lengthOf(5);
                    expect(client.readyState).to.equal(0);
                    t.done();
                }, 200);
            });
        });

        // xhr/abort-event-order.htm
        it("in loadstart fires readystatechange, abort and loadend and leaves the request UNSENT", function () {
            return async_test((t) => {
                const xhr = new XMLHttpRequest();
                const recorded = prepare_xhr_for_event_order_test(xhr);
                xhr.addEventListener("loadstart", t.step_func(() => {
                    expect(xhr.readyState, "Loadstart event should fire in readyState 1").to.equal(1);
                    xhr.abort();
                    expect(recorded).to.deep.equal([1, "loadstart(0,0,false)", 4, "abort(0,0,false)", "loadend(0,0,false)"]);
                    expect(xhr.readyState, "state should be UNSENT").to.equal(0);
                    t.done();
                }));
                xhr.open("GET", resources + "content.py", true);
                xhr.send();
            });
        });

        // xhr/abort-during-done.window.js (async)
        it("during DONE resets the request without firing readystatechange", function () {
            return async_test((t) => {
                const client = new XMLHttpRequest();
                const result: number[] = [];
                const expected = [1, 2, 3, 4];
                client.onreadystatechange = t.step_func(() => {
                    result.push(client.readyState);
                    if (client.readyState === 4) {
                        expect(client.readyState).to.equal(4);
                        expect(client.status).to.equal(200);
                        client.abort();
                        expect(client.readyState).to.equal(0);
                        expect(client.status).to.equal(0);
                        expect(client.statusText).to.equal("");
                        expect(client.getAllResponseHeaders()).to.equal("");
                        t.step_timeout(() => {
                            expect(result).to.deep.equal(expected);
                            t.done();
                        }, 100); // wait a bit in case XHR timeout causes spurious event
                    }
                });
                client.open("GET", resources + "well-formed.xml");
                client.send(null);
            });
        });

        // Not from WPT: abort() must interrupt a transfer that is waiting for the server.
        it("interrupts a request that is waiting for a response", function () {
            return async_test((t) => {
                const client = new XMLHttpRequest();
                const recorded = prepare_xhr_for_event_order_test(client);
                client.open("GET", resources + "delay.py?ms=20000");
                client.send(null);
                setTimeout(t.step_func(() => {
                    client.abort();
                    expect(recorded).to.deep.equal([1, "loadstart(0,0,false)", 4, "abort(0,0,false)", "loadend(0,0,false)"]);
                    expect(client.readyState).to.equal(0);
                    expect(client.status).to.equal(0);
                    t.done();
                }), 100);
            });
        });
    });

    describe("timeout", function () {
        // xhr/event-timeout.any.js
        it("fires timeout", function () {
            return async_test((t) => {
                const client = new XMLHttpRequest();
                let done = false;
                client.ontimeout = t.step_func(() => {
                    expect(client.readyState).to.equal(4);
                    done = true;
                    t.done();
                });
                client.timeout = 5;
                client.open("GET", resources + "delay.py?ms=20000");
                client.send(null);
                t.step_timeout(() => {
                    expect(done, "ontimeout not called.").to.equal(true);
                }, 1000);
            });
        });
    });

    describe("redirects", function () {
        // xhr/send-redirect.htm, for the statuses the Fetch standard defines as redirects. Left
        // out: the other 3xx statuses, which must not be followed but which the platform HTTP
        // stacks treat differently, and the POST / HEAD rows.
        function redirect(code: number, redirectLocation: string) {
            it(`follows a ${code} redirect to ${decodeURIComponent(redirectLocation)}`, function () {
                return async_test((t) => {
                    const client = new XMLHttpRequest();
                    client.onreadystatechange = t.step_func(() => {
                        if (client.readyState === 4) {
                            expect(client.getResponseHeader("x-request-method")).to.equal("GET");
                            expect(client.status).to.equal(200);
                            t.done();
                        }
                    });
                    client.open("GET", resources + `redirect.py?location=${redirectLocation}&code=${code}`);
                    client.send(null);
                });
            });
        }

        const redirectedLocation = encodeURIComponent("redirect.py?location=content.py");
        for (const code of [301, 302, 303, 307, 308]) {
            redirect(code, "content.py");
        }
        for (const code of [301, 302, 303, 307]) {
            redirect(code, redirectedLocation);
        }
    });
});
