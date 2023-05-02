// Set this to true to make attaching a debugging easier.
const waitForDebugger = false;

// TODO: use premade reporter (once Console Polyfill is fixed)
function BabylonReporter(runner) {
    const stats = runner.stats;

    runner.on("pass", test => {
        console.log(`Passed: ${test.fullTitle()}`);
    });

    runner.on("fail", (test, err) => {
        console.log(`Failed: ${test.fullTitle()} with error: ${err.message}`);
    });

    runner.on("end", () => {
        console.log(`Tests passed: ${stats.passes}/${stats.tests}`);
    });
}

mocha.setup({ ui: "bdd", reporter: BabylonReporter });

const expect = chai.expect;

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

        controller.signal.addEventListener('abort', () => {
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

        controller.signal.addEventListener('abort', onAbort);
        controller.signal.removeEventListener('abort', onAbort);

        controller.abort();

        expect(controller.signal.aborted).to.equal(true);
    });
});

describe("XMLHTTPRequest", function () {
    function createRequest(method, url, body) {
        return new Promise((resolve) => {
            const xhr = new XMLHttpRequest();
            xhr.open(method, url);
            xhr.addEventListener("loadend", () => resolve(xhr));
            xhr.send(body);
        });
    }

    function createRequestWithHeaders(method, url, headers, body) {
        return new Promise((resolve) => {
            const xhr = new XMLHttpRequest();
            xhr.open(method, url);
            headers.forEach((value, key) => xhr.setRequestHeader(key, value));
            xhr.addEventListener("loadend", () => resolve(xhr));
            xhr.send(body);
        });
    }

    this.timeout(0);

    it("should have readyState=4 when load ends", async function () {
        const xhr = await createRequest("GET", "https://httpbin.org/get");
        expect(xhr.readyState).to.equal(4);
    });

    it("should have status=200 for a file that exists", async function () {
        const xhr = await createRequest("GET", "https://httpbin.org/status/200");
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
        const xhr = await createRequest("GET", "https://httpbin.org/status/404");
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

    it("should make a POST request with no body successfully", async function () {
        const xhr = await createRequest("POST", "https://httpbin.org/post");
        expect(xhr).to.have.property('readyState', 4);
        expect(xhr).to.have.property('status', 200);
    });

    it("should make a POST request with body successfully", async function () {
        const xhr = await createRequest("POST", "https://httpbin.org/post", "sampleBody");
        expect(xhr).to.have.property('readyState', 4);
        expect(xhr).to.have.property('status', 200);
    });

    it("should make a GET request with headers successfully", async function () {
        const headersMap = new Map([['foo', '3'], ['bar', '3']]);
        const xhr = await createRequestWithHeaders("GET", "https://httpbin.org/get", headersMap);
        expect(xhr).to.have.property('readyState', 4);
        expect(xhr).to.have.property('status', 200);
    });

    it("should make a POST request with body and headers successfully", async function () {
        const headersMap = new Map([['foo', '3'], ['bar', '3']]);
        const xhr = await createRequestWithHeaders("POST", "https://httpbin.org/post", headersMap, "testBody");
        expect(xhr).to.have.property('readyState', 4);
        expect(xhr).to.have.property('status', 200);
    });
});

describe("setTimeout", function () {
    this.timeout(1000);

    it("should return an id greater than zero", function () {
        const id = setTimeout(() => { }, 0);
        expect(id).to.be.greaterThan(0);
    });

    it("should return an id greater than zero when given an undefined function", function () {
        const id = setTimeout(undefined, 0);
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
        }, "10");
    });

    it("should call the given function after zero milliseconds when the delay is a string representing an invalid number", function (done) {
        setTimeout(() => {
            done();
        }, "a");
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
    this.timeout(0);
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
});


// Websocket

describe("WebSocket", function () {
    it("should connect correctly with one websocket connection", function (done) {
        const ws = new WebSocket('wss://ws.postman-echo.com/raw');
        const testMessage = "testMessage";
        ws.onopen = () => {
            try {
                expect(ws).to.have.property('readyState', 1);
                expect(ws).to.have.property('url', 'wss://ws.postman-echo.com/raw');
                ws.send(testMessage);
            }
            catch (e) {
                done(e);
            }
        }

        ws.onmessage = (msg) => {
            try {
                expect(msg.data).to.equal(testMessage);
                ws.close();
            }
            catch (e) {
                done(e);
            }
        }

        ws.onclose = () => {
            try {
                expect(ws).to.have.property('readyState', 3);
                done();
            }
            catch (e) {
                done(e);
            }
        }
    });

    it("should connect correctly with multiple websocket connections", function (done) {
        const testMessage1 = "testMessage1";
        const testMessage2 = "testMessage2";

        const ws1 = new WebSocket('wss://ws.postman-echo.com/raw');
        ws1.onopen = () => {
            const ws2 = new WebSocket('wss://ws.postman-echo.com/raw');
            ws2.onopen = () => {
                try {
                    expect(ws2).to.have.property('readyState', 1);
                    expect(ws2).to.have.property('url', 'wss://ws.postman-echo.com/raw');
                    ws2.send(testMessage2);
                }
                catch (e) {
                    done(e);
                }
            }

            ws2.onmessage = (msg) => {
                try {
                    expect(msg.data).to.equal(testMessage2);
                    ws2.close();
                }
                catch (e) {
                    done(e);
                }
            }

            ws2.onclose = () => {
                try {
                    expect(ws2).to.have.property('readyState', 3);
                    ws1.send(testMessage1);
                }
                catch (e) {
                    done(e);
                }
            }
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
                expect(ws1).to.have.property('readyState', 3);
                done();
            }
            catch (e) {
                done(e);
            }
        }
    });

    it("should trigger error callback with invalid server", function (done) {
        const ws = new WebSocket('wss://example.com');
        ws.onerror = () => {
            done();
        }
    });

    it("should trigger error callback with invalid domain", function (done) {
        this.timeout(10000);
        const ws = new WebSocket('wss://example');
        ws.onerror = () => {
            done();
        }
    });
})


// URL
describe("URL", function () {

    // Currently all of the properties that the polyfill has implemented
    function checkURL(url, { href, hostname, origin, pathname, search }) {
        expect(url).to.have.property("hostname", hostname);
        expect(url).to.have.property("href", href);
        expect(url).to.have.property("origin", origin);
        expect(url).to.have.property("pathname", pathname);
        expect(url).to.have.property("search", search);
    }

    // Base URLs:
    const baseUrl = 'http://httpbin.org';

    it('should load URL with no pathname / search', function () {
        // Standard URL (No pathname, no search)
        const url = new URL(baseUrl);
        checkURL(url, {
            href: 'http://httpbin.org',
            hostname: 'httpbin.org',
            origin: 'http://httpbin.org',
            pathname: '',
            search: ''
        });
    });

    it("should load URL with pathname (no search)", function () {
        // Augment URL with pathname (no search)
        const url = new URL(`${baseUrl}/en-US/docs`);
        checkURL(url, {
            href: 'http://httpbin.org/en-US/docs',
            hostname: 'httpbin.org',
            origin: 'http://httpbin.org',
            pathname: '/en-US/docs',
            search: ''
        });
    });

    it("should load URL with pathname and search", function () {
        // Augment URL with pathname and search
        const url = new URL(`${baseUrl}/en-US/docs?foo=1&bar=2`);
        checkURL(url, {
            href: 'http://httpbin.org/en-US/docs?foo=1&bar=2',
            hostname: 'httpbin.org',
            origin: 'http://httpbin.org',
            pathname: '/en-US/docs',
            search: '?foo=1&bar=2'
        });
    });

    it("should load URL with pathname and search with multiple key value pairs", function () {
        const url = new URL(`${baseUrl}/en-US/docs?c=3&b=2&a=1&d=4`);
        checkURL(url, {
            href: 'http://httpbin.org/en-US/docs?c=3&b=2&a=1&d=4',
            hostname: 'httpbin.org',
            origin: 'http://httpbin.org',
            pathname: '/en-US/docs',
            search: '?c=3&b=2&a=1&d=4'
        });
    });

    it("should update href after URLSearchParams are changed", function () {
        // Augment URL with pathname and search
        const url = new URL(`${baseUrl}/en-US/docs?foo=1&bar=2`);
        url.searchParams.set('foo', 999);
        // href should change to reflect searchParams change
        checkURL(url, {
            href: 'http://httpbin.org/en-US/docs?foo=999&bar=2',
            hostname: 'httpbin.org',
            origin: 'http://httpbin.org',
            pathname: '/en-US/docs',
            search: '?foo=999&bar=2'
        });
    });

    it("should update href after URLSearchParams are changed (Starting with 0 params)", function () {
        // Augment URL with pathname and search
        const url = new URL(`${baseUrl}/en-US/docs`);
        url.searchParams.set('foo', 999);
        // href should change to reflect searchParams change
        checkURL(url, {
            href: 'http://httpbin.org/en-US/docs?foo=999',
            hostname: 'httpbin.org',
            origin: 'http://httpbin.org',
            pathname: '/en-US/docs',
            search: '?foo=999'
        });
    });
});

// URLSearchParams
describe("URLSearchParams", function () {

    // -------------------------------- URLSearchParams Get --------------------------------

    it("should retrieve null from empty searchParams", function () {
        // Get Empty
        const params = new URLSearchParams('');

        expect(params.get('foo')).to.equal(null);
    });

    it("should retrieve value from searchParams", function () {
        // Get Value
        const params = new URLSearchParams('?foo=1');

        expect(params.get('foo')).to.equal('1');
    });

    // -------------------------------- URLSearchParams Set --------------------------------

    const paramsSet = new URLSearchParams('');

    it("should throw exception when trying to set with less than 2 parameters", function () {
        expect(() => paramsSet.set()).to.throw();
    });

    it("should add a number and retrieve it as a string from searchParams", function () {
        // Set Number
        paramsSet.set('foo', 400);
        expect(paramsSet.get('foo')).to.equal('400');
    });

    it("should add a string and retrieve it as a string from searchParams", function () {
        // Set String
        paramsSet.set('bar', '50');
        expect(paramsSet.get('bar')).to.equal('50');
    });

    it("should add a boolean and retrieve it as a string from searchParams", function () {
        // Set Boolean
        paramsSet.set('baz', true);
        expect(paramsSet.get('baz')).to.equal('true');
    });

    it("should set an existing number and retrieve it as a string from searchParams", function () {
        // Set Existing Value
        paramsSet.set('foo', 9999);
        expect(paramsSet.get('foo')).to.equal('9999');
    });

    // -------------------------------- URLSearchParams Has --------------------------------

    it("should check value is in searchParams (True)", function () {
        // Check existing value
        const paramsHas = new URLSearchParams('?foo=1');
        expect(paramsHas.has('foo')).to.equal(true);
    });

    it("should check value is in searchParams (False)", function () {
        // Check non-existing value
        const paramsHas = new URLSearchParams('?foo=1');
        expect(paramsHas.has('Microsoft')).to.equal(false);
    });

    it("should check empty searchParams for value (False)", function () {
        // Check empty params
        const paramsEmpty = new URLSearchParams('');
        expect(paramsEmpty.has('foo')).to.equal(false);
    });

    // -------------------------------- URLSearchParams Construction --------------------------------

    it("should retrieve search params set at construction", function () {
        // Retrieve params via url.search, passed into ctor
        const url = new URL('https://example.com?foo=1&bar=2');
        const params1 = new URLSearchParams(url.search);
        expect(params1.get('foo')).to.equal('1');
        expect(params1.get('bar')).to.equal('2');
    });

    it("should retrieve search params from url.searchParams object", function () {
        // Get the URLSearchParams object directly from an URL object
        const url = new URL('https://example.com?foo=1&bar=2');
        const params1a = url.searchParams;
        expect(params1a.get('foo')).to.equal('1');
        expect(params1a.get('bar')).to.equal('2');
    });

    it("should retrieve search params string constructed URLSearchParams", function () {
        // Pass in a string literal
        const params2 = new URLSearchParams("foo=1&bar=2");
        expect(params2.get('foo')).to.equal('1');
        expect(params2.get('bar')).to.equal('2');
    });
});

function runTests() {
    mocha.run(failures => {
        // Test program will wait for code to be set before exiting
        if (failures > 0) {
            // Failure
            SetExitCode(1);
        } else {
            // Success
            SetExitCode(0);
        }
    });
}

if (waitForDebugger) {
    function waitLoop() {
        const start = Date.now();
        debugger;
        if (Date.now() - start < 100) {
            setTimeout(waitLoop, 200);
        } else {
            runTests();
        }
    }

    console.log("Waiting for debugger...");
    waitLoop();
} else {
    runTests();
}
