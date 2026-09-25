import { expect } from "chai";

// Websocket
if (hostPlatform !== "Unix") {
    describe("WebSocket", function () {
        this.timeout(10000);

        it("should connect correctly with one websocket connection", function (done) {
            const ws = new WebSocket("wss://ws.postman-echo.com/raw");
            const testMessage = "testMessage";
            let error: unknown;

            ws.onopen = () => {
                try {
                    expect(ws).to.have.property("readyState", 1);
                    expect(ws).to.have.property("url", "wss://ws.postman-echo.com/raw");
                    ws.send(testMessage);
                }
                catch (e) {
                    error = e;
                    ws.close();
                }
            };

            ws.onmessage = (msg) => {
                try {
                    expect(msg.data).to.equal(testMessage);
                }
                catch (e) {
                    error = e;
                }
                ws.close();
            };

            ws.onclose = () => {
                if (!error) {
                    try {
                        expect(ws).to.have.property("readyState", 3);
                    }
                    catch (e) {
                        error = e;
                    }
                }
                done(error);
            };

            ws.onerror = () => {
                error = new Error("WebSocket failed");
            };
        });

        it("should connect correctly with multiple websocket connections", function (done) {
            this.timeout(10000);
            const testMessage1 = "testMessage1";
            const testMessage2 = "testMessage2";
            let error: unknown;

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
                        error = e;
                        ws2.close();
                    }
                };

                ws2.onmessage = (msg) => {
                    try {
                        expect(msg.data).to.equal(testMessage2);
                    }
                    catch (e) {
                        error = e;
                    }
                    ws2.close();
                };

                ws2.onclose = () => {
                    if (!error) {
                        try {
                            expect(ws2).to.have.property("readyState", 3);
                            ws1.send(testMessage1);
                        }
                        catch (e) {
                            error = e;
                            ws1.close();
                        }
                    }
                    else {
                        ws1.close();
                    }
                };

                ws2.onerror = () => {
                    error = new Error("WebSocket failed");
                };
            }

            ws1.onmessage = (msg) => {
                try {
                    expect(msg.data).to.equal(testMessage1);
                }
                catch (e) {
                    error = e;
                }
                ws1.close();
            }

            ws1.onclose = () => {
                if (!error) {
                    try {
                        expect(ws1).to.have.property("readyState", 3);
                    }
                    catch (e) {
                        error = e;
                    }
                }
                done(error);
            }

            ws1.onerror = () => {
                error = new Error("WebSocket failed");
            };
        });

        it("should trigger error callback with invalid server", function (done) {
            this.timeout(10000);
            // Random UUID-based hostname so the domain is guaranteed unregistered
            // (RFC-reserved `.invalid` causes a >10s DNS path on Win32 x86 Chakra).
            const ws = new WebSocket("wss://caddddfd-ee88-4771-b293-8a8e13b330ee.com");
            let errorFired = false;
            ws.onerror = () => {
                errorFired = true;
            };
            ws.onclose = () => {
                try {
                    expect(errorFired).to.be.true;
                    done();
                }
                catch (e) {
                    done(e);
                }
            };
        });

        it("should trigger error callback with invalid domain", function (done) {
            this.timeout(10000);
            const ws = new WebSocket("wss://example");
            let errorFired = false;
            ws.onerror = () => {
                errorFired = true;
            };
            ws.onclose = () => {
                try {
                    expect(errorFired).to.be.true;
                    done();
                }
                catch (e) {
                    done(e);
                }
            };
        });
    })
}
