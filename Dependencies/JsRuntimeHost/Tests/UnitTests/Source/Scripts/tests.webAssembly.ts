import { expect } from "chai";

describe("WebAssembly", function () {
    this.timeout(30000);

    // Only the V8 AppRuntime pumps V8's foreground task queue, which is what lets these promises
    // settle. The other engines' runtimes have the same class of gap and hang here instead of
    // failing, so scope the suite rather than leave a 30s timeout on every non-V8 leg.
    beforeEach(function () {
        if (hostEngine !== "V8" || typeof WebAssembly === "undefined") {
            this.skip();
        }
    });

    // Minimal valid module: the 8-byte header (magic + version) and no sections.
    const emptyModule = new Uint8Array([0x00, 0x61, 0x73, 0x6D, 0x01, 0x00, 0x00, 0x00]);

    it("should settle the promise returned by WebAssembly.compile", async function () {
        const module = await WebAssembly.compile(emptyModule);
        expect(module).to.be.an.instanceof(WebAssembly.Module);
    });

    it("should settle the promise returned by WebAssembly.instantiate", async function () {
        const result = await WebAssembly.instantiate(emptyModule);
        expect(result.instance).to.be.an.instanceof(WebAssembly.Instance);
    });

    it("should reject the promise returned by WebAssembly.compile for invalid bytes", async function () {
        let threw = false;
        try {
            await WebAssembly.compile(new Uint8Array([0x00, 0x61, 0x73, 0x6D, 0xFF]));
        } catch (e) {
            threw = true;
        }
        expect(threw).to.equal(true);
    });
});
