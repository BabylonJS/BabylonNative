import { expect } from "chai";

describe("fetch", function () {
    this.timeout(30000);

    it("should resolve with ok=true and status=200 for a resource that exists", async function () {
        const response = await fetch("https://github.com/");
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
});
