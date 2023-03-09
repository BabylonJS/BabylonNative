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
    it("should not throw while aborting and correctly trigger callback", function (done) {
        const controller = new AbortController();
        expect(controller.signal.aborted).to.equal(false);

        // Expect aborted to be true after abort()
        controller.signal.onabort = () => {
            expect(controller.signal.aborted).to.equal(true);
            done();
        }
        controller.abort();
    })
})

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