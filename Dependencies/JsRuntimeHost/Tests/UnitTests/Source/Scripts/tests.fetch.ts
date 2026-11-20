import { expect } from "chai";

describe("fetch", function () {
    this.timeout(30000);

    it("should resolve with ok=true and status=200 for a resource that exists", async function () {
        const response = await fetch("https://github.com/");
        expect(response).to.be.instanceOf(Response);
        expect(response.headers).to.be.instanceOf(Headers);
        expect(response.body).to.be.instanceOf(ReadableStream);
        expect(response.ok).to.equal(true);
        expect(response.status).to.equal(200);
    });

    it("should resolve (not reject) with ok=false and status=404 for a resource that does not exist", async function () {
        const response = await fetch("https://github.com/babylonJS/BabylonNative404");
        expect(response.ok).to.equal(false);
        expect(response.status).to.equal(404);
    });

    it("should expose statusText", async function () {
        const okResponse = await fetch("https://github.com/");
        expect(okResponse.statusText).to.equal("OK");
        const notFoundResponse = await fetch("https://github.com/babylonJS/BabylonNative404");
        expect(notFoundResponse.statusText).to.equal("Not Found");
    });

    it("text() should return the body as a string", async function () {
        const response = await fetch("app:///Assets/symlink_target.js");
        expect(await response.text()).to.equal("var symlink_target_js = true;");
    });

    it("should resolve percent-encoded data URLs locally", async function () {
        const url = "data:text/plain;charset=utf-8,hello%20native%20fetch%21";
        const response = await fetch(url);
        expect(response.ok).to.equal(true);
        expect(response.status).to.equal(200);
        expect(response.url).to.equal(url);
        expect(response.headers.get("content-type")).to.equal("text/plain;charset=utf-8");
        expect(await response.text()).to.equal("hello native fetch!");
    });

    it("should decode base64 data URLs without using the network transport", async function () {
        const response = await fetch("data:application/octet-stream;base64,AAEC/w==");
        const clone = response.clone();
        expect(new Uint8Array(await response.arrayBuffer())).to.eql(new Uint8Array([0, 1, 2, 255]));
        const blob = await clone.blob();
        expect(blob.type).to.equal("application/octet-stream");
        expect(new Uint8Array(await blob.arrayBuffer())).to.eql(new Uint8Array([0, 1, 2, 255]));
    });

    // The data: scheme is resolved by UrlLib for every consumer, not only fetch (#67): XMLHttpRequest
    // is the path Babylon.js' asset and texture loaders take.
    it("resolves data: URLs through XMLHttpRequest for asset-style loads", async function () {
        const xhr = await new Promise<XMLHttpRequest>((resolve) => {
            const req = new XMLHttpRequest();
            req.open("GET", "data:text/plain;charset=utf-8,hello%20world");
            req.addEventListener("loadend", () => resolve(req));
            req.send();
        });
        expect(xhr.status).to.equal(200);
        expect(xhr.responseText).to.equal("hello world");
        expect((xhr.getResponseHeader("content-type") || "").toLowerCase()).to.contain("text/plain");
    });

    it("decodes base64 data: URLs into an ArrayBuffer through XMLHttpRequest", async function () {
        const xhr = await new Promise<XMLHttpRequest>((resolve) => {
            const req = new XMLHttpRequest();
            req.responseType = "arraybuffer";
            req.open("GET", "data:application/octet-stream;base64,AQID");
            req.addEventListener("loadend", () => resolve(req));
            req.send();
        });
        expect(xhr.status).to.equal(200);
        expect(Array.from(new Uint8Array(xhr.response))).to.deep.equal([1, 2, 3]);
    });

    it("surfaces a malformed data: URL as a network error", async function () {
        const xhr = await new Promise<XMLHttpRequest>((resolve) => {
            const req = new XMLHttpRequest();
            req.open("GET", "data:text/plain;base64,@@@");
            req.addEventListener("loadend", () => resolve(req));
            req.send();
        });
        expect(xhr.status).to.equal(0);
    });

    it("round-trips a data: URL through Blob and an object URL", async function () {
        const response = await fetch("data:application/octet-stream;base64,AQID");
        const blob = await response.blob();
        expect(blob.size).to.equal(3);
        expect(blob.type).to.equal("application/octet-stream");
        const objectUrl = URL.createObjectURL(blob);
        try {
            const again = await fetch(objectUrl);
            expect(Array.from(new Uint8Array(await again.arrayBuffer()))).to.deep.equal([1, 2, 3]);
        } finally {
            URL.revokeObjectURL(objectUrl);
        }
    });

    it("streams a base64 gzip data: URL through DecompressionStream", async function () {
        if (typeof DecompressionStream !== "function") {
            this.skip(); // the compression polyfill is optional
        }
        // gzip.compress(b"hello gzip", mtime=0)
        const response = await fetch("data:application/gzip;base64,H4sIAAAAAAAC/8tIzcnJV0ivyiwAABlq0t8KAAAA");
        const decompressed = response.body.pipeThrough(new DecompressionStream("gzip"));
        expect(await new Response(decompressed).text()).to.equal("hello gzip");
    });

    // Adapted from WPT fetch/data-urls/processing.any.js and resources/data-urls.json.
    const dataUrlCases: Array<[string, string, number[]]> = [
        ["data:,", "text/plain;charset=US-ASCII", []],
        ["data:,%FF", "text/plain;charset=US-ASCII", [255]],
        ["data:text/plain,X", "text/plain", [88]],
        ["data:,X#fragment", "text/plain;charset=US-ASCII", [88]],
        ["data:;BASe64,WA", "text/plain;charset=US-ASCII", [88]],
        ["data:  ;charset=x   ;  base64,W%20A", "text/plain;charset=x", [88]]
    ];

    for (const [url, expectedType, expectedBody] of dataUrlCases) {
        it(`should process WPT data URL case ${JSON.stringify(url)}`, async function () {
            const response = await fetch(url);
            expect(response.headers.get("content-type")).to.equal(expectedType);
            expect(Array.from(new Uint8Array(await response.arrayBuffer()))).to.eql(expectedBody);
        });
    }

    // Adapted from WPT's forgiving-base64 vectors and Chromium's DataURL tests.
    const base64Cases: Array<[string, number[]]> = [
        ["abcd", [105, 183, 29]],
        ["ab%09%0A%0C%0D%20cd", [105, 183, 29]],
        ["ab==", [105]],
        ["/A", [252]],
        ["YR", [97]]
    ];

    for (const [encoded, expectedBody] of base64Cases) {
        it(`should forgiving-base64 decode ${JSON.stringify(encoded)}`, async function () {
            const response = await fetch(`data:application/octet-stream;base64,${encoded}`);
            expect(Array.from(new Uint8Array(await response.arrayBuffer()))).to.eql(expectedBody);
        });
    }

    for (const encoded of ["a", "ab===", "ab%0Bcd", "=a", "a=b"]) {
        it(`should reject invalid WPT base64 case ${JSON.stringify(encoded)}`, async function () {
            let error: unknown;
            try {
                await fetch(`data:application/octet-stream;base64,${encoded}`);
            } catch (caught) {
                error = caught;
            }
            expect(error).to.be.instanceOf(TypeError);
        });
    }

    it("arrayBuffer() should return the body as bytes", async function () {
        const response = await fetch("app:///Assets/symlink_target.js");
        const expected = new Uint8Array("var symlink_target_js = true;".split("").map(x => x.charCodeAt(0)));
        expect(new Uint8Array(await response.arrayBuffer())).to.eql(expected);
    });

    it("json() should parse a JSON body", async function () {
        const response = await fetch("app:///Assets/sample.json");
        const json = await response.json();
        expect(json.name).to.equal("fetch-polyfill-test");
        expect(json.value).to.equal(42);
        expect(json.nested.items).to.eql([1, 2, 3]);
    });

    it("json() should reject when the body is not valid JSON", async function () {
        const response = await fetch("app:///Assets/symlink_target.js");
        let rejected = false;
        try {
            await response.json();
        } catch {
            rejected = true;
        }
        expect(rejected).to.equal(true);
    });

    it("blob() should return a Blob with the body bytes", async function () {
        const response = await fetch("app:///Assets/symlink_target.js");
        const blob = await response.blob();
        expect(blob.size).to.equal("var symlink_target_js = true;".length);
        expect(await blob.text()).to.equal("var symlink_target_js = true;");
    });

    it("headers.get() should be case-insensitive and headers.has() should work", async function () {
        const response = await fetch("https://github.com/");
        expect(response.headers.has("Content-Type")).to.equal(true);
        expect(response.headers.get("CONTENT-TYPE")).to.equal(response.headers.get("content-type"));
    });

    it("clone() should produce an independently readable response", async function () {
        const response = await fetch("app:///Assets/symlink_target.js");
        const clone = response.clone();
        expect(await response.text()).to.equal("var symlink_target_js = true;");
        expect(await clone.text()).to.equal("var symlink_target_js = true;");
    });

    it("should accept a method in the init object", async function () {
        const response = await fetch("https://github.com/", { method: "GET" });
        expect(response.status).to.equal(200);
    });

    it("should reject when no arguments are provided", async function () {
        let rejected = false;
        try {
            await (fetch as any)();
        } catch {
            rejected = true;
        }
        expect(rejected).to.equal(true);
    });

    it("should reject a transport failure with a TypeError carrying detail on cause", async function () {
        this.timeout(30000);
        let error: any;
        try {
            // Nothing listens on this loopback port, so the connection is refused -- a transport
            // failure (status 0), distinct from an HTTP error status.
            await fetch("http://127.0.0.1:1/");
        } catch (e) {
            error = e;
        }
        expect(error, "fetch should have rejected").to.not.equal(undefined);
        // Spec-conformant shape: network errors reject with a TypeError whose message is stable
        // (browsers/Node/undici all keep it constant so crash-report grouping stays intact).
        expect(error).to.be.an.instanceof(TypeError);
        expect(error.message).to.equal("fetch failed");
        // The variable detail rides on `cause` (Node/undici shape), never on the stable message.
        expect(error.cause, "error.cause should be populated").to.be.an("object");
        expect(error.cause.url).to.contain("127.0.0.1");
        expect(error.cause.status).to.equal(0);
        // On backends where UrlLib populates transport detail (Apple/Linux) `code`/`detail` are
        // present stable tokens; on backends that don't yet (Windows/Android) they are absent --
        // the stable observable shape above is preserved either way.
        if (error.cause.code !== undefined) {
            expect(error.cause.code).to.be.a("string").and.not.equal("");
            expect(error.cause.detail).to.be.a("string").and.not.equal("");
        }
    });

    it("should reject a missing app:// asset with a TypeError (distinct from a network failure)", async function () {
        let error: any;
        try {
            await fetch("app:///does_not_exist.js");
        } catch (e) {
            error = e;
        }
        expect(error, "fetch should have rejected").to.not.equal(undefined);
        expect(error).to.be.an.instanceof(TypeError);
        expect(error.message).to.equal("fetch failed");
        expect(error.cause.url).to.contain("does_not_exist.js");
    });

    it("should reject immediately with an AbortError when the signal is already aborted", async function () {
        const controller = new AbortController();
        controller.abort();

        let error: any;
        try {
            await fetch("https://github.com/", { signal: controller.signal } as any);
        } catch (e) {
            error = e;
        }
        expect(error, "fetch should have rejected").to.not.equal(undefined);
        expect(error.name).to.equal("AbortError");
    });

    it("should reject with an AbortError when aborted in-flight", async function () {
        this.timeout(30000);
        const controller = new AbortController();
        const promise = fetch("https://github.com/", { signal: controller.signal } as any);
        // Abort before the response can arrive.
        controller.abort();

        let error: any;
        try {
            await promise;
        } catch (e) {
            error = e;
        }
        expect(error, "fetch should have rejected").to.not.equal(undefined);
        expect(error.name).to.equal("AbortError");
    });

    it("should preserve a primitive abort reason when aborted in-flight", async function () {
        this.timeout(30000);
        const controller = new AbortController();
        const promise = fetch("https://github.com/", { signal: controller.signal } as any);
        controller.abort("visualization-stop");

        let error: any;
        try {
            await promise;
        } catch (e) {
            error = e;
        }
        expect(error).to.equal("visualization-stop");
    });
});
