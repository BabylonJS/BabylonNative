import * as Mocha from "mocha";
import { expect } from "chai";

Mocha.setup('bdd');
// @ts-ignore
Mocha.reporter('spec');

declare const hostPlatform: string;
declare const setExitCode: (code: number) => void;


describe("AbortController", function () {
    it("should not throw while aborting with no callbacks", function () {
        const controller = new AbortController();
        expect(controller.signal.aborted).to.equal(false);

        // Trigger with no callbacks
        controller.abort();

        expect(controller.signal.aborted).to.equal(true);
    });

    it("should not throw while aborting and correctly trigger both callbacks", function (done) {
        const controller = new AbortController();
        expect(controller.signal.aborted).to.equal(false);

        let cb1 = false, cb2 = false;

        // Expect aborted to be true after abort()
        controller.signal.onabort = () => {
            expect(controller.signal.aborted).to.equal(true);
            cb1 = true;

            if (cb1 && cb2) {
                done();
            }
        }

        controller.signal.addEventListener("abort", () => {
            expect(controller.signal.aborted).to.equal(true);
            cb2 = true;

            if (cb1 && cb2) {
                done();
            }
        })

        controller.abort();
    });

    it("should remove listener and not invoke callback function", function () {
        const controller = new AbortController();
        expect(controller.signal.aborted).to.equal(false);

        // If this function is unsuccessfully removed it will assert when called
        const onAbort = () => {
            expect(controller.signal.aborted).to.equal(false);
        };

        controller.signal.addEventListener("abort", onAbort);
        controller.signal.removeEventListener("abort", onAbort);

        controller.abort();

        expect(controller.signal.aborted).to.equal(true);
    });
});

describe("XMLHTTPRequest", function () {
    function createRequest(method: string, url: string, body: any = undefined, responseType: any = undefined): Promise<XMLHttpRequest> {
        return new Promise((resolve) => {
            const xhr = new XMLHttpRequest();
            xhr.open(method, url);
            if (responseType !== undefined) {
                xhr.responseType = responseType;
            }
            xhr.addEventListener("loadend", () => resolve(xhr));
            xhr.send(body);
        });
    }

    function createRequestWithHeaders(method: string, url: string, headers: any, body?: string): Promise<XMLHttpRequest> {
        return new Promise((resolve) => {
            const xhr = new XMLHttpRequest();
            xhr.open(method, url);
            headers.forEach((value: string, key: string) => xhr.setRequestHeader(key, value));
            xhr.addEventListener("loadend", () => resolve(xhr));
            xhr.send(body);
        });
    }

    this.timeout(0);

    it("should have readyState=4 when load ends", async function () {
        const xhr = await createRequest("GET", "https://github.com/");
        expect(xhr.readyState).to.equal(4);
    });

    it("should have status=200 for a file that exists", async function () {
        const xhr = await createRequest("GET", "https://github.com/");
        expect(xhr.status).to.equal(200);
    });

    it("should load URLs with escaped unicode characters", async function () {
        const xhr = await createRequest("GET", "https://raw.githubusercontent.com/BabylonJS/Assets/master/meshes/%CF%83%CF%84%CF%81%CE%BF%CE%B3%CE%B3%CF%85%CE%BB%CE%B5%CE%BC%CE%AD%CE%BD%CE%BF%CF%82%20%25%20%CE%BA%CF%8D%CE%B2%CE%BF%CF%82.glb");
        expect(xhr.status).to.equal(200);
    });

    it("should load URLs with unescaped unicode characters", async function () {
        const xhr = await createRequest("GET", "https://raw.githubusercontent.com/BabylonJS/Assets/master/meshes/στρογγυλεμένος%20%25%20κύβος.glb");
        expect(xhr.status).to.equal(200);
    });

    it("should load URLs with unescaped unicode characters and spaces", async function () {
        const xhr = await createRequest("GET", "https://raw.githubusercontent.com/BabylonJS/Assets/master/meshes/στρογγυλεμένος %25 κύβος.glb");
        expect(xhr.status).to.equal(200);
    });

    it("should have status=404 for a file that does not exist", async function () {
        const xhr = await createRequest("GET", "https://github.com/babylonJS/BabylonNative404");
        expect(xhr.status).to.equal(404);
    });

    it("should throw something when opening //", async function () {
        function openDoubleSlash() {
            const xhr = new XMLHttpRequest();
            xhr.open("GET", "//");
            xhr.send();
        }
        expect(openDoubleSlash).to.throw();
    });

    it("should throw something when opening a url with no scheme", function () {
        function openNoProtocol() {
            const xhr = new XMLHttpRequest();
            xhr.open("GET", "noscheme.glb");
            xhr.send();
        }
        expect(openNoProtocol).to.throw();
    });

    it("should throw something when sending before opening", function () {
        function sendWithoutOpening() {
            const xhr = new XMLHttpRequest();
            xhr.send();
        }
        expect(sendWithoutOpening).to.throw();
    });

    // TODO: httpbin server seems to be flaky right now. Re-enable these tests later.
    // if (hostPlatform !== "Unix") {
    //     it("should make a POST request with no body successfully", async function () {
    //         const xhr = await createRequest("POST", "https://httpbin.org/post");
    //         expect(xhr).to.have.property("readyState", 4);
    //         expect(xhr).to.have.property("status", 200);
    //     });

    //     it("should make a POST request with body successfully", async function () {
    //         const xhr = await createRequest("POST", "https://httpbin.org/post", "sampleBody");
    //         expect(xhr).to.have.property("readyState", 4);
    //         expect(xhr).to.have.property("status", 200);
    //     });
    // }

    // it("should make a GET request with headers successfully", async function () {
    //     const headersMap = new Map([["foo", "3"], ["bar", "3"]]);
    //     const xhr = await createRequestWithHeaders("GET", "https://httpbin.org/get", headersMap);
    //     expect(xhr).to.have.property("readyState", 4);
    //     expect(xhr).to.have.property("status", 200);
    // });

    // if (hostPlatform !== "Unix") {
    //     it("should make a POST request with body and headers successfully", async function () {
    //         const headersMap = new Map([["foo", "3"], ["bar", "3"]]);
    //         const xhr = await createRequestWithHeaders("POST", "https://httpbin.org/post", headersMap, "testBody");
    //         expect(xhr).to.have.property("readyState", 4);
    //         expect(xhr).to.have.property("status", 200);
    //     });
    // }

    if (hostPlatform === "macOS" || hostPlatform === "Unix" || hostPlatform === "Win32") {
        it("should load URL pointing to symlink", async function () {
            const xhr = await createRequest("GET", "app:///Scripts/symlink_1.js");
            expect(xhr).to.have.property("responseText", "var symlink_target_js = true;");
        });

        it("should load URL pointing to symlink that points to a symlink", async function () {
            const xhr = await createRequest("GET", "app:///Scripts/symlink_2.js");
            expect(xhr).to.have.property("responseText", "var symlink_target_js = true;");
        });
    }

    it("should load URL as array buffer", async function () {
        const xhr = await createRequest("GET", "app:///Scripts/symlink_target.js", undefined, "arraybuffer");
        var expected = new Uint8Array("var symlink_target_js = true;".split("").map(x => x.charCodeAt(0)));
        var response = new Uint8Array(xhr.response);
        expect(response).to.eql(expected);
    });
});

describe("setTimeout", function () {
    this.timeout(1000);

    it("should return an id greater than zero", function () {
        const id = setTimeout(() => { }, 0);
        expect(id).to.be.greaterThan(0);
    });

    it("should return an id greater than zero when given an undefined function", function () {
        const id = setTimeout(undefined as any, 0);
        expect(id).to.be.greaterThan(0);
    });

    it("should call the given function after the given delay", function (done) {
        const startTime = new Date().getTime();
        setTimeout(() => {
            try {
                expect(new Date().getTime() - startTime).to.be.at.least(10);
                done();
            }
            catch (e) {
                done(e);
            }
        }, 10);
    });

    it("should call the given nested function after the given delay", function (done) {
        const startTime = new Date().getTime();
        setTimeout(() => {
            setTimeout(() => {
                try {
                    expect(new Date().getTime() - startTime).to.be.at.least(20);
                    done();
                }
                catch (e) {
                    done(e);
                }
            }, 10);
        }, 10);
    });

    it("should call the given function after the given delay when the delay is a string representing a valid number", function (done) {
        const startTime = new Date().getTime();
        setTimeout(() => {
            try {
                expect(new Date().getTime() - startTime).to.be.at.least(10);
                done();
            }
            catch (e) {
                done(e);
            }
        }, "10" as any);
    });

    it("should call the given function after zero milliseconds when the delay is a string representing an invalid number", function (done) {
        setTimeout(() => {
            done();
        }, "a" as any);
    });

    it("should call the given function after other tasks execute when the given delay is zero", function (done) {
        let trailingCodeExecuted = false;
        setTimeout(() => {
            try {
                expect(trailingCodeExecuted).to.be.true;
                done();
            }
            catch (e) {
                done(e);
            }
        }, 0);
        trailingCodeExecuted = true;
    });

    it("should call the given function after other tasks execute when the given delay is undefined", function (done) {
        let trailingCodeExecuted = false;
        setTimeout(() => {
            try {
                expect(trailingCodeExecuted).to.be.true;
                done();
            }
            catch (e) {
                done(e);
            }
        }, undefined);
        trailingCodeExecuted = true;
    });

    // See https://github.com/BabylonJS/JsRuntimeHost/issues/9
    // it("should call the given functions in the correct order", function (done) {
    //     const called = [];
    //     for (let i = 9; i >= 0; i--) {
    //         setTimeout(() => {
    //             called.push(i);
    //             if (called.length === 10) {
    //                 try {
    //                     expect(called).to.deep.equal([0, 1, 2, 3, 4, 5, 6, 7, 8, 9]);
    //                     done();
    //                 }
    //                 catch (e) {
    //                     done(e);
    //                 }
    //             }
    //         }, i * 10);
    //     }
    // });
});

describe("clearTimeout", function () {
    this.timeout(1000);

    it("should stop the timeout matching the given timeout id", function (done) {
        const id = setTimeout(() => {
            done(new Error("Timeout was not cleared"));
        }, 0);
        clearTimeout(id);
        setTimeout(done, 100);
    });

    it("should do nothing if the given timeout id is undefined", function (done) {
        setTimeout(() => { done(); }, 0);
        clearTimeout(undefined);
    });

    it("should be interchangeable with clearInterval", function (done) {
        const id = setTimeout(() => {
            done(new Error("Interval was not cleared"));
        }, 0);
        clearInterval(id);
        setTimeout(done, 100);
    });
});

describe("setInterval", function () {
    this.timeout(1000);

    it("should return an id greater than zero", function () {
        const id = setInterval(() => { }, 0);
        clearInterval(id);
        expect(id).to.be.greaterThan(0);
    });

    it("should call the given function at the given interval", function (done) {
        let startTime = new Date().getTime();
        let tickCount = 0;
        const id = setInterval(() => {
            try {
                tickCount++;
                expect(new Date().getTime() - startTime).to.be.at.least(tickCount * 10);
                if (tickCount > 2) {
                    clearInterval(id);
                    done();
                }
            }
            catch (e) {
                console.log(`finished with error: ${e}`);
                clearInterval(id);
                done(e);
            }
        }, 10);
    });
});

describe("clearInterval", function () {
    this.timeout(1000);

    it("should stop the interval matching the given interval id", function (done) {
        const id = setInterval(() => {
            done(new Error("Interval was not cleared"));
        }, 0);
        clearInterval(id);
        setTimeout(done, 100);
    });

    it("should do nothing if the given interval id is undefined", function (done) {
        setTimeout(() => { done(); }, 0);
        clearInterval(undefined);
    });

    it("should be interchangeable with clearTimeout", function (done) {
        const id = setInterval(() => {
            done(new Error("Interval was not cleared"));
        }, 0);
        clearTimeout(id);
        setTimeout(done, 100);
    });
});

// Websocket
if (hostPlatform !== "Unix") {
    describe("WebSocket", function () {
        it("should connect correctly with one websocket connection", function (done) {
            const ws = new WebSocket("wss://ws.postman-echo.com/raw");
            const testMessage = "testMessage";
            ws.onopen = () => {
                try {
                    expect(ws).to.have.property("readyState", 1);
                    expect(ws).to.have.property("url", "wss://ws.postman-echo.com/raw");
                    ws.send(testMessage);
                }
                catch (e) {
                    done(e);
                }
            };

            ws.onmessage = (msg) => {
                try {
                    expect(msg.data).to.equal(testMessage);
                    ws.close();
                }
                catch (e) {
                    done(e);
                }
            };

            ws.onclose = () => {
                try {
                    expect(ws).to.have.property("readyState", 3);
                    done();
                }
                catch (e) {
                    done(e);
                }
            };

            ws.onerror = (ev) => {
                done(new Error("WebSocket failed"));
            };
        });

        it("should connect correctly with multiple websocket connections", function (done) {
            const testMessage1 = "testMessage1";
            const testMessage2 = "testMessage2";

            const ws1 = new WebSocket("wss://ws.postman-echo.com/raw");
            ws1.onopen = () => {
                const ws2 = new WebSocket("wss://ws.postman-echo.com/raw");
                ws2.onopen = () => {
                    try {
                        expect(ws2).to.have.property("readyState", 1);
                        expect(ws2).to.have.property("url", "wss://ws.postman-echo.com/raw");
                        ws2.send(testMessage2);
                    }
                    catch (e) {
                        done(e);
                    }
                };

                ws2.onmessage = (msg) => {
                    try {
                        expect(msg.data).to.equal(testMessage2);
                        ws2.close();
                    }
                    catch (e) {
                        done(e);
                    }
                };

                ws2.onclose = () => {
                    try {
                        expect(ws2).to.have.property("readyState", 3);
                        ws1.send(testMessage1);
                    }
                    catch (e) {
                        done(e);
                    }
                };

                ws2.onerror = (ev) => {
                    done(new Error("Websocket failed"));
                };
            }

            ws1.onmessage = (msg) => {
                try {
                    expect(msg.data).to.equal(testMessage1);
                    ws1.close();
                }
                catch (e) {
                    done(e);
                }
            }

            ws1.onclose = () => {
                try {
                    expect(ws1).to.have.property("readyState", 3);
                    done();
                }
                catch (e) {
                    done(e);
                }
            }

            ws1.onerror = (ev) => {
                done(new Error("Websocket failed"));
            };
        });

        // TODO: This is not working reliably: see https://github.com/BabylonJS/JsRuntimeHost/issues/131
        // it("should trigger error callback with invalid server", function (done) {
        //     const ws = new WebSocket("wss://caddddfd-ee88-4771-b293-8a8e13b330ee.com");
        //     ws.onerror = () => {
        //         done();
        //     };
        // });

        it("should trigger error callback with invalid domain", function (done) {
            this.timeout(10000);
            const ws = new WebSocket("wss://example");
            ws.onerror = () => {
                done();
            };
        });
    })
}

// URL
describe("URL", function () {

    // Currently all of the properties that the polyfill has implemented
    interface URLCheckOptions {
        href: string;
        hostname: string;
        origin: string;
        pathname: string;
        search: string;
    }

    function checkURL(url: URL, { href, hostname, origin, pathname, search }: URLCheckOptions): void {
        expect(url).to.have.property("hostname", hostname);
        expect(url).to.have.property("href", href);
        expect(url).to.have.property("origin", origin);
        expect(url).to.have.property("pathname", pathname);
        expect(url).to.have.property("search", search);
    }

    it("should load URL with no pathname / search", function () {
        // Standard URL (No pathname, no search)
        const url = new URL("https://httpbin.org");
        checkURL(url, {
            href: "https://httpbin.org/",
            hostname: "httpbin.org",
            origin: "https://httpbin.org",
            pathname: "/",
            search: ""
        });
    });

    it("should load URL with pathname (no search)", function () {
        // Augment URL with pathname (no search)
        const url = new URL("https://httpbin.org/en-US/docs");
        checkURL(url, {
            href: "https://httpbin.org/en-US/docs",
            hostname: "httpbin.org",
            origin: "https://httpbin.org",
            pathname: "/en-US/docs",
            search: ""
        });
    });

    it("should load URL with pathname and search", function () {
        // Augment URL with pathname and search
        const url = new URL("https://httpbin.org/en-US/docs?foo=1&bar=2");
        checkURL(url, {
            href: "https://httpbin.org/en-US/docs?foo=1&bar=2",
            hostname: "httpbin.org",
            origin: "https://httpbin.org",
            pathname: "/en-US/docs",
            search: "?foo=1&bar=2"
        });
    });

    it("should load URL with pathname and search with multiple key value pairs", function () {
        const url = new URL("https://httpbin.org/en-US/docs?c=3&b=2&a=1&d=4");
        checkURL(url, {
            href: "https://httpbin.org/en-US/docs?c=3&b=2&a=1&d=4",
            hostname: "httpbin.org",
            origin: "https://httpbin.org",
            pathname: "/en-US/docs",
            search: "?c=3&b=2&a=1&d=4"
        });
    });

    it("should update href after URLSearchParams are changed", function () {
        // Augment URL with pathname and search
        const url = new URL("https://httpbin.org/en-US/docs?foo=1&bar=2");
        url.searchParams.set("foo", 999 as any);
        // href should change to reflect searchParams change
        checkURL(url, {
            href: "https://httpbin.org/en-US/docs?foo=999&bar=2",
            hostname: "httpbin.org",
            origin: "https://httpbin.org",
            pathname: "/en-US/docs",
            search: "?foo=999&bar=2"
        });
    });

    it("should update href after URLSearchParams are changed (Starting with 0 params)", function () {
        // Augment URL with pathname and search
        const url = new URL("https://httpbin.org/en-US/docs");
        url.searchParams.set("foo", "999");
        // href should change to reflect searchParams change
        checkURL(url, {
            href: "https://httpbin.org/en-US/docs?foo=999",
            hostname: "httpbin.org",
            origin: "https://httpbin.org",
            pathname: "/en-US/docs",
            search: "?foo=999"
        });
    });

    // -------------------------------- URL Properties --------------------------------

    it("should parse protocol correctly", function () {
        const url = new URL("https://example.com/path");
        expect(url.protocol).to.equal("https:");
    });

    it("should parse port correctly", function () {
        const url = new URL("https://example.com:8080/path");
        expect(url.port).to.equal("8080");
        expect(url.host).to.equal("example.com:8080");
    });

    it("should return empty port when not specified", function () {
        const url = new URL("https://example.com/path");
        expect(url.port).to.equal("");
        expect(url.host).to.equal("example.com");
    });

    it("should parse hash correctly", function () {
        const url = new URL("https://example.com/path#section1");
        expect(url.hash).to.equal("#section1");
    });

    it("should return empty hash when not present", function () {
        const url = new URL("https://example.com/path");
        expect(url.hash).to.equal("");
    });

    it("should parse username and password correctly", function () {
        const url = new URL("https://user:pass@example.com/path");
        expect(url.username).to.equal("user");
        expect(url.password).to.equal("pass");
    });

    it("should return empty username and password when not present", function () {
        const url = new URL("https://example.com/path");
        expect(url.username).to.equal("");
        expect(url.password).to.equal("");
    });

    it("should parse URL with all components", function () {
        const url = new URL("https://user:pass@example.com:8080/path/to/resource?foo=1&bar=2#section");
        expect(url.protocol).to.equal("https:");
        expect(url.username).to.equal("user");
        expect(url.password).to.equal("pass");
        expect(url.hostname).to.equal("example.com");
        expect(url.port).to.equal("8080");
        expect(url.host).to.equal("example.com:8080");
        expect(url.pathname).to.equal("/path/to/resource");
        expect(url.search).to.equal("?foo=1&bar=2");
        expect(url.hash).to.equal("#section");
        expect(url.origin).to.equal("https://example.com:8080");
    });

    // -------------------------------- URL Property Setters --------------------------------

    it("should update protocol via setter", function () {
        const url = new URL("https://example.com/path");
        url.protocol = "http:";
        expect(url.protocol).to.equal("http:");
        expect(url.href).to.equal("http://example.com/path");
    });

    it("should add colon to protocol if missing", function () {
        const url = new URL("https://example.com/path");
        url.protocol = "http";
        expect(url.protocol).to.equal("http:");
    });

    it("should update hostname via setter", function () {
        const url = new URL("https://example.com/path");
        url.hostname = "newhost.com";
        expect(url.hostname).to.equal("newhost.com");
        expect(url.href).to.equal("https://newhost.com/path");
    });

    it("should update host via setter (hostname only)", function () {
        const url = new URL("https://example.com:8080/path");
        url.host = "newhost.com";
        expect(url.hostname).to.equal("newhost.com");
        expect(url.port).to.equal("");
        expect(url.host).to.equal("newhost.com");
    });

    it("should update host via setter (hostname and port)", function () {
        const url = new URL("https://example.com/path");
        url.host = "newhost.com:9090";
        expect(url.hostname).to.equal("newhost.com");
        expect(url.port).to.equal("9090");
        expect(url.host).to.equal("newhost.com:9090");
    });

    it("should update port via setter", function () {
        const url = new URL("https://example.com/path");
        url.port = "3000";
        expect(url.port).to.equal("3000");
        expect(url.href).to.equal("https://example.com:3000/path");
    });

    it("should update pathname via setter", function () {
        const url = new URL("https://example.com/path");
        url.pathname = "/new/path";
        expect(url.pathname).to.equal("/new/path");
        expect(url.href).to.equal("https://example.com/new/path");
    });

    it("should add leading slash to pathname if missing", function () {
        const url = new URL("https://example.com/path");
        url.pathname = "new/path";
        expect(url.pathname).to.equal("/new/path");
    });

    it("should update search via setter", function () {
        const url = new URL("https://example.com/path");
        url.search = "?newkey=newvalue";
        expect(url.search).to.equal("?newkey=newvalue");
        expect(url.href).to.equal("https://example.com/path?newkey=newvalue");
    });

    it("should update search via setter without leading question mark", function () {
        const url = new URL("https://example.com/path");
        url.search = "key=value";
        expect(url.search).to.equal("?key=value");
    });

    it("should update hash via setter", function () {
        const url = new URL("https://example.com/path");
        url.hash = "#newsection";
        expect(url.hash).to.equal("#newsection");
        expect(url.href).to.equal("https://example.com/path#newsection");
    });

    it("should add leading hash if missing", function () {
        const url = new URL("https://example.com/path");
        url.hash = "newsection";
        expect(url.hash).to.equal("#newsection");
    });

    it("should update username via setter", function () {
        const url = new URL("https://example.com/path");
        url.username = "newuser";
        expect(url.username).to.equal("newuser");
        expect(url.href).to.equal("https://newuser@example.com/path");
    });

    it("should update password via setter", function () {
        const url = new URL("https://user@example.com/path");
        url.password = "newpass";
        expect(url.password).to.equal("newpass");
        expect(url.href).to.equal("https://user:newpass@example.com/path");
    });

    it("should update href via setter", function () {
        const url = new URL("https://example.com/path");
        url.href = "https://newsite.com/newpath?q=1#hash";
        expect(url.hostname).to.equal("newsite.com");
        expect(url.pathname).to.equal("/newpath");
        expect(url.search).to.equal("?q=1");
        expect(url.hash).to.equal("#hash");
    });

    // -------------------------------- URL Instance Methods --------------------------------

    it("should return href from toString()", function () {
        const url = new URL("https://example.com/path?foo=1#section");
        expect(url.toString()).to.equal("https://example.com/path?foo=1#section");
    });

    it("should return href from toJSON()", function () {
        const url = new URL("https://example.com/path?foo=1#section");
        expect(url.toJSON()).to.equal("https://example.com/path?foo=1#section");
    });

    // -------------------------------- URL Static Methods --------------------------------

    it("should return true from canParse() for valid absolute URL", function () {
        expect(URL.canParse("https://example.com")).to.equal(true);
    });

    it("should return false from canParse() for invalid URL", function () {
        expect(URL.canParse("not-a-url")).to.equal(false);
    });

    it("should return true from canParse() for relative URL with valid base", function () {
        expect(URL.canParse("/path", "https://example.com")).to.equal(true);
    });

    it("should return false from canParse() for relative URL with invalid base", function () {
        expect(URL.canParse("/path", "not-a-url")).to.equal(false);
    });

    it("should return false from canParse() with no arguments", function () {
        // @ts-expect-error - testing no arguments
        expect(URL.canParse()).to.equal(false);
    });

    it("should return URL object from parse() for valid URL", function () {
        const url = URL.parse("https://example.com/path");
        expect(url).to.not.equal(null);
        expect(url!.hostname).to.equal("example.com");
        expect(url!.pathname).to.equal("/path");
    });

    // TODO: Uncomment this once the the Node-API implementation for Chakra supports throwing errors from constructors.
    // it("should return null from parse() for invalid URL", function () {
    //     const url = URL.parse("not-a-url");
    //     expect(url).to.equal(null);
    // });

    it("should return URL object from parse() with valid base", function () {
        const url = URL.parse("/path", "https://example.com");
        expect(url).to.not.equal(null);
        expect(url!.hostname).to.equal("example.com");
        expect(url!.pathname).to.equal("/path");
    });

    // TODO: Uncomment this once the the Node-API implementation for Chakra supports throwing errors from constructors.
    // it("should return null from parse() with invalid base", function () {
    //     const url = URL.parse("/path", "not-a-url");
    //     expect(url).to.equal(null);
    // });

    // -------------------------------- URL Base URL Resolution --------------------------------

    it("should resolve relative URL with base URL", function () {
        const url = new URL("/path/to/resource", "https://example.com");
        expect(url.href).to.equal("https://example.com/path/to/resource");
    });

    it("should resolve relative path against base URL", function () {
        const url = new URL("resource.html", "https://example.com/path/to/");
        expect(url.href).to.equal("https://example.com/path/to/resource.html");
    });

    it("should resolve relative path against base URL with file", function () {
        const url = new URL("resource.html", "https://example.com/path/to/index.html");
        expect(url.href).to.equal("https://example.com/path/to/resource.html");
    });

    it("should resolve query-only relative URL", function () {
        const url = new URL("?newquery=1", "https://example.com/path");
        expect(url.href).to.equal("https://example.com/path?newquery=1");
    });

    it("should resolve hash-only relative URL", function () {
        const url = new URL("#newhash", "https://example.com/path?query=1");
        expect(url.href).to.equal("https://example.com/path?query=1#newhash");
    });

    it("should resolve protocol-relative URL", function () {
        const url = new URL("//other.com/path", "https://example.com");
        expect(url.href).to.equal("https://other.com/path");
    });

    it("should use absolute URL and ignore base when URL is absolute", function () {
        const url = new URL("https://other.com/otherpath", "https://example.com/path");
        expect(url.href).to.equal("https://other.com/otherpath");
    });

    it("should resolve dot segments in path (single dot)", function () {
        const url = new URL("https://example.com/a/b/./c");
        expect(url.pathname).to.equal("/a/b/c");
    });

    it("should resolve dot segments in path (double dot)", function () {
        const url = new URL("https://example.com/a/b/../c");
        expect(url.pathname).to.equal("/a/c");
    });

    it("should resolve complex dot segments", function () {
        const url = new URL("https://example.com/a/b/c/../d/./e/../f");
        expect(url.pathname).to.equal("/a/b/d/f");
    });

    it("should resolve relative path with dot segments", function () {
        const url = new URL("../sibling/file.html", "https://example.com/path/to/current/");
        expect(url.pathname).to.equal("/path/to/sibling/file.html");
    });

    // -------------------------------- URL Error Handling --------------------------------

    // TODO: Uncomment this once the the Node-API implementation for Chakra supports throwing errors from constructors.
    // it("should throw for invalid URL without base", function () {
    //     expect(() => new URL("not-a-valid-url")).to.throw();
    // });

    // TODO: Uncomment this once the the Node-API implementation for Chakra supports throwing errors from constructors.
    // it("should throw for relative URL without base", function () {
    //     expect(() => new URL("/path/to/resource")).to.throw();
    // });

    // TODO: Uncomment this once the the Node-API implementation for Chakra supports throwing errors from constructors.
    // it("should throw for empty URL constructor", function () {
    //     // @ts-expect-error - testing no arguments
    //     expect(() => new URL()).to.throw();
    // });

    // -------------------------------- URL Different Schemes --------------------------------

    it("should parse file:// URL", function () {
        const url = new URL("file:///path/to/file.txt");
        expect(url.protocol).to.equal("file:");
        expect(url.pathname).to.equal("/path/to/file.txt");
    });

    it("should parse ftp:// URL", function () {
        const url = new URL("ftp://ftp.example.com/file.zip");
        expect(url.protocol).to.equal("ftp:");
        expect(url.hostname).to.equal("ftp.example.com");
    });

    it("should parse ws:// URL", function () {
        const url = new URL("ws://example.com/socket");
        expect(url.protocol).to.equal("ws:");
        expect(url.hostname).to.equal("example.com");
    });

    it("should parse wss:// URL", function () {
        const url = new URL("wss://example.com/socket");
        expect(url.protocol).to.equal("wss:");
        expect(url.hostname).to.equal("example.com");
    });

    it("should parse custom scheme URL", function () {
        const url = new URL("app:///Scripts/test.js");
        expect(url.protocol).to.equal("app:");
        expect(url.pathname).to.equal("/Scripts/test.js");
    });
});

// URLSearchParams
describe("URLSearchParams", function () {

    // -------------------------------- URLSearchParams Get --------------------------------

    it("should retrieve null from empty searchParams", function () {
        // Get Empty
        const params = new URLSearchParams("");

        expect(params.get("foo")).to.equal(null);
    });

    it("should retrieve value from searchParams", function () {
        // Get Value
        const params = new URLSearchParams("?foo=1");

        expect(params.get("foo")).to.equal("1");
    });

    // -------------------------------- URLSearchParams Set --------------------------------

    const paramsSet = new URLSearchParams("");

    it("should throw exception when trying to set with less than 2 parameters", function () {
        // `set` expects parameters, none given.
        // @ts-expect-error
        expect(() => paramsSet.set()).to.throw();
    });

    it("should add a number and retrieve it as a string from searchParams", function () {
        // Set Number
        paramsSet.set("foo", 400 as any);
        expect(paramsSet.get("foo")).to.equal("400");
    });

    it("should add a string and retrieve it as a string from searchParams", function () {
        // Set String
        paramsSet.set("bar", "50");
        expect(paramsSet.get("bar")).to.equal("50");
    });

    it("should add a boolean and retrieve it as a string from searchParams", function () {
        // Set Boolean
        paramsSet.set("baz", true as any);
        expect(paramsSet.get("baz")).to.equal("true");
    });

    it("should set an existing number and retrieve it as a string from searchParams", function () {
        // Set Existing Value
        paramsSet.set("foo", 9999 as any);
        expect(paramsSet.get("foo")).to.equal("9999");
    });

    // -------------------------------- URLSearchParams Has --------------------------------

    it("should check value is in searchParams (True)", function () {
        // Check existing value
        const paramsHas = new URLSearchParams("?foo=1");
        expect(paramsHas.has("foo")).to.equal(true);
    });

    it("should check value is in searchParams (False)", function () {
        // Check non-existing value
        const paramsHas = new URLSearchParams("?foo=1");
        expect(paramsHas.has("Microsoft")).to.equal(false);
    });

    it("should check empty searchParams for value (False)", function () {
        // Check empty params
        const paramsEmpty = new URLSearchParams("");
        expect(paramsEmpty.has("foo")).to.equal(false);
    });

    // -------------------------------- URLSearchParams Construction --------------------------------

    it("should retrieve search params set at construction", function () {
        // Retrieve params via url.search, passed into ctor
        const url = new URL("https://example.com?foo=1&bar=2");
        const params1 = new URLSearchParams(url.search);
        expect(params1.get("foo")).to.equal("1");
        expect(params1.get("bar")).to.equal("2");
    });

    it("should retrieve search params from url.searchParams object", function () {
        // Get the URLSearchParams object directly from an URL object
        const url = new URL("https://example.com?foo=1&bar=2");
        const params1a = url.searchParams;
        expect(params1a.get("foo")).to.equal("1");
        expect(params1a.get("bar")).to.equal("2");
    });

    it("should retrieve search params string constructed URLSearchParams", function () {
        // Pass in a string literal
        const params2 = new URLSearchParams("foo=1&bar=2");
        expect(params2.get("foo")).to.equal("1");
        expect(params2.get("bar")).to.equal("2");
    });
});

describe("Console", function () {
    it("should log a simple console log string without error", function () {
        expect(() => console.log("I am a test string")).to.not.throw();
    });
    it("should log sequence of strings", function () {
        expect(() => console.log("I", "am", "a", "test", "string", 2, 2.345, { foo: "bar" })).to.not.throw();
    });
    it("Should log string substitutions", function () {
        expect(() => console.log("String sub: %s, float sub: %f, int sub: %d", "string", 3.1457, 3.1457)).to.not.throw();
    });
    it("Should allow numbers to substitute strings", function () {
        expect(() => console.log("Print these numbers! %s %s", 1.2345, 1)).to.not.throw();
    });
    it("Should allow strings to substitute numbers", function () {
        expect(() => console.log("Print these strings! %f %d", "foo", "bar")).to.not.throw();
    });
    it("Should allow for less substitution arguments than parameters", function () {
        expect(() => console.log("%s", "I am a string", 12345)).to.not.throw();
    });
    it("Should allow for more substitution arguments than parameters", function () {
        expect(() => console.log("%s %s", "I am a string")).to.not.throw();
    });
    it("Should allow a substitution argument with no parameter", function () {
        expect(() => console.log("%s")).to.not.throw();
    });
    it("Should ignore invalid substitution arguments", function () {
        expect(() => console.log("%y %s %k", "I am a string")).to.not.throw();
        expect(() => console.log("%y%s%k", "I am a string")).to.not.throw();
        expect(() => console.log("%", 756)).to.not.throw();
        expect(() => console.log("%%", 756)).to.not.throw();
    });
    it("Should allow logging NaN", function () {
        expect(() => console.log("%d %f %s", NaN, NaN, NaN)).to.not.throw();
        expect(() => console.log("%d %f %s", 0 / 0, 0 / 0, 0 / 0)).to.not.throw();
    });
});

describe("Blob", function () {
    let emptyBlobs: Blob[], helloBlobs: Blob[], stringBlob: Blob, typedArrayBlob: Blob, arrayBufferBlob: Blob, blobBlob: Blob;

    before(function () {
        emptyBlobs = [new Blob([]), new Blob([])];
        stringBlob = new Blob(["Hello"]);
        typedArrayBlob = new Blob([new Uint8Array([72, 101, 108, 108, 111])]),
        arrayBufferBlob = new Blob([new Uint8Array([72, 101, 108, 108, 111]).buffer]),
        blobBlob = new Blob([new Blob(["Hello"])]),
        helloBlobs = [stringBlob, typedArrayBlob, arrayBufferBlob, blobBlob]
    });

    // -------------------------------- Blob Construction --------------------------------
    it("creates empty blobs", function () {
        for (const blob of emptyBlobs) {
            expect(blob.size).to.equal(0);
            expect(blob.type).to.equal("");
        }
    });

    it("creates blob from string array", function () {
        expect(stringBlob.size).to.equal(5);
        expect(stringBlob.type).to.equal("");
    });

    it("creates blob from TypedArray", function () {
        expect(typedArrayBlob.size).to.equal(5);
        expect(typedArrayBlob.type).to.equal("");
    });

    it("creates blob from ArrayBuffer", function () {
        expect(arrayBufferBlob.size).to.equal(5);
        expect(arrayBufferBlob.type).to.equal("");
    });

    it("creates blob from another Blob", function () {
        expect(blobBlob.size).to.equal(5);
        expect(blobBlob.type).to.equal("");
    });

    it("applies MIME type from options", function () {
        const modelGltfJson = new Blob(["glTF"], { type: "model/gltf+json" })
        expect(modelGltfJson.type).to.equal("model/gltf+json");
    });

    // -------------------------------- Blob.text() --------------------------------
    it("returns empty string for empty blobs", async function () {
        for (const blob of emptyBlobs) {
            const text = await blob.text();
            expect(text).to.equal("");
        }
    });

    it("returns correct string content for non-empty blobs", async function () {
        for (const blob of helloBlobs) {
            const text = await blob.text();
            expect(text).to.equal("Hello");
        }
    });

    it("handles multi-byte UTF-8 characters", async function () {
        const utf8Blob = new Blob(["你好, 世界"]);
        const text = await utf8Blob.text();
        expect(text).to.equal("你好, 世界");
    });

    it("preserves line endings like default transparent mode", async function () {
        const lineEndingsBlob = new Blob(["Hello\nWorld"]);
        const text = await lineEndingsBlob.text();
        expect(text).to.equal("Hello\nWorld");
    });

    // -------------------------------- Blob.bytes() --------------------------------
    it("returns empty Uint8Array for empty blobs", async function () {
        for (const blob of emptyBlobs) {
            const bytes = await blob.bytes();
            expect(bytes).to.be.instanceOf(Uint8Array);
            expect(bytes.length).to.equal(0);
        }
    });

    it("returns correct byte content from non-empty blobs", async function () {
        for (const blob of helloBlobs) {
            const bytes = await blob.bytes();
            expect(bytes).to.be.instanceOf(Uint8Array);
            expect(bytes.length).to.equal(5);
            expect(bytes[0]).to.equal(72); // 'H'
            expect(bytes[4]).to.equal(111); // 'o'
        }
    });

    // -------------------------------- Blob.arrayBuffer() --------------------------------
    it("returns empty buffer for empty blobs", async function () {
        for (const blob of emptyBlobs) {
            const buffer = await blob.arrayBuffer();
            expect(buffer).to.be.instanceOf(ArrayBuffer);
            expect(buffer.byteLength).to.equal(0);
        }
    });

    it("returns correct buffer content for non-empty blobs", async function () {
        for (const blob of helloBlobs) {
            const buffer = await blob.arrayBuffer();
            expect(buffer).to.be.instanceOf(ArrayBuffer);
            expect(buffer.byteLength).to.equal(5);

            const view = new Uint8Array(buffer);
            expect(view[0]).to.equal(72); // 'H'
            expect(view[4]).to.equal(111); // 'o'

        }
    });
});

describe("Performance", function () {
    this.timeout(1000);

    it("should have performance global defined", function () {
        expect(performance).to.not.be.undefined;
    });

    it("should return a number from performance.now()", function () {
        const now = performance.now();
        expect(now).to.be.a("number");
    });

    it("should return a non-negative value from performance.now()", function () {
        const now = performance.now();
        expect(now).to.be.at.least(0);
    });

    it("should return increasing values from performance.now()", function (done) {
        const first = performance.now();
        setTimeout(() => {
            const second = performance.now();
            expect(second).to.be.greaterThan(first);
            done();
        }, 10);
    });

    it("should measure elapsed time accurately", function (done) {
        const start = performance.now();
        const delay = 50;
        setTimeout(() => {
            const elapsed = performance.now() - start;
            // Allow some tolerance (elapsed should be at least the delay, but could be slightly more)
            expect(elapsed).to.be.at.least(delay - 5);
            expect(elapsed).to.be.lessThan(delay + 100); // generous upper bound
            done();
        }, delay);
    });

    it("should return sub-millisecond precision", function () {
        // Call performance.now() multiple times rapidly and check if we get fractional values
        const samples: number[] = [];
        for (let i = 0; i < 100; i++) {
            samples.push(performance.now());
        }
        // At least some samples should have fractional parts (sub-millisecond precision)
        const hasFractional = samples.some(s => s % 1 !== 0);
        expect(hasFractional).to.equal(true);
    });
});

describe("TextDecoder", function () {
    it("should decode a Uint8Array to a string", function () {
        const decoder = new TextDecoder();
        const encoded = new Uint8Array([72, 101, 108, 108, 111]); // "Hello"
        const result = decoder.decode(encoded);
        expect(result).to.equal("Hello");
    });

    it("should decode an empty Uint8Array to an empty string", function () {
        const decoder = new TextDecoder();
        const result = decoder.decode(new Uint8Array([]));
        expect(result).to.equal("");
    });

    it("should decode an ArrayBuffer to a string", function () {
        const decoder = new TextDecoder();
        const buffer = new Uint8Array([87, 111, 114, 108, 100]).buffer; // "World"
        const result = decoder.decode(buffer);
        expect(result).to.equal("World");
    });

    it("should decode a TypedArray subarray with non-zero byteOffset", function () {
        const decoder = new TextDecoder();
        const full = new Uint8Array([88, 72, 105]); // "XHi"
        const sub = full.subarray(1); // [72, 105] -> "Hi"
        const result = decoder.decode(sub);
        expect(result).to.equal("Hi");
    });
});

function runTests() {
    mocha.run((failures: number) => {
        // Test program will wait for code to be set before exiting
        if (failures > 0) {
            // Failure
            setExitCode(1);
        } else {
            // Success
            setExitCode(0);
        }
    });
}

runTests();
