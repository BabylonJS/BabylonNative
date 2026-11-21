import { expect } from "chai";

describe("File", function () {
    // -------------------------------- Construction --------------------------------
    it("creates an empty File", function () {
        const file = new File([], "empty.txt");
        expect(file.size).to.equal(0);
        expect(file.type).to.equal("");
        expect(file.name).to.equal("empty.txt");
    });

    it("creates a File from a string array", function () {
        const file = new File(["Hello"], "hello.txt");
        expect(file.size).to.equal(5);
        expect(file.name).to.equal("hello.txt");
    });

    it("creates a File from a TypedArray", function () {
        const data = new Uint8Array([72, 101, 108, 108, 111]); // "Hello"
        const file = new File([data], "typed.bin");
        expect(file.size).to.equal(5);
        expect(file.name).to.equal("typed.bin");
    });

    it("creates a File from an ArrayBuffer", function () {
        const buffer = new Uint8Array([72, 101, 108, 108, 111]).buffer;
        const file = new File([buffer], "buffer.bin");
        expect(file.size).to.equal(5);
    });

    it("creates a File from a Blob", function () {
        const blob = new Blob(["Hello"]);
        const file = new File([blob], "from-blob.txt");
        expect(file.size).to.equal(5);
    });

    it("applies MIME type from options", function () {
        const file = new File(["{}"], "data.json", { type: "application/json" });
        expect(file.type).to.equal("application/json");
    });

    it("defaults lastModified to a recent timestamp when not provided", function () {
        const before = Date.now();
        const file = new File([], "x.txt");
        const after = Date.now();
        expect(file.lastModified).to.be.a("number");
        // Allow small clock-skew slack on either side.
        expect(file.lastModified).to.be.at.least(before - 1000);
        expect(file.lastModified).to.be.at.most(after + 1000);
    });

    it("honors lastModified from options", function () {
        const file = new File([], "x.txt", { lastModified: 12345 });
        expect(file.lastModified).to.equal(12345);
    });

    it("coerces a non-string name to a string", function () {
        const file = new File([], 42 as any);
        expect(file.name).to.equal("42");
    });

    it("coerces undefined and null name per WebIDL USVString", function () {
        // Per the WHATWG File constructor's WebIDL signature, name is a
        // non-optional USVString; ToString is applied regardless of input
        // type, so passing undefined/null yields the string "undefined" /
        // "null" rather than an empty string.
        expect(new File([], undefined as any).name).to.equal("undefined");
        expect(new File([], null as any).name).to.equal("null");
    });

    // TODO(JsRH#175): Re-enable once the Chakra Node-API shim surfaces
    // exceptions thrown from class constructor callbacks back to JS.
    // it("throws when fewer than 2 arguments are passed", function () {
    //     // File requires both fileBits and fileName per the WebIDL bindings.
    //     // Browsers throw TypeError on missing arguments; the native polyfill
    //     // must match that surface so consumers don't accidentally create a
    //     // File with empty name when their call site is misspelled.
    //     // Note: we only assert *that* it throws (not the specific error
    //     // type), because the JSI napi shim wraps thrown Napi::TypeError as
    //     // a generic JS Error when surfacing it across the host boundary.
    //     expect(() => new (File as any)()).to.throw();
    //     expect(() => new (File as any)([])).to.throw();
    // });

    // -------------------------------- Read API --------------------------------
    it("returns text via .text()", async function () {
        const file = new File(["Hello"], "hello.txt");
        const text = await file.text();
        expect(text).to.equal("Hello");
    });

    it("returns bytes via .bytes()", async function () {
        const file = new File(["Hello"], "hello.txt");
        const bytes = await file.bytes();
        expect(bytes).to.be.instanceOf(Uint8Array);
        expect(bytes.length).to.equal(5);
        expect(bytes[0]).to.equal(72); // 'H'
        expect(bytes[4]).to.equal(111); // 'o'
    });

    it("returns an ArrayBuffer via .arrayBuffer()", async function () {
        const file = new File(["Hello"], "hello.txt");
        const buffer = await file.arrayBuffer();
        expect(buffer).to.be.instanceOf(ArrayBuffer);
        expect(buffer.byteLength).to.equal(5);
    });

    it("handles multi-byte UTF-8 content", async function () {
        const file = new File(["你好, 世界"], "utf8.txt");
        const text = await file.text();
        expect(text).to.equal("你好, 世界");
    });

    // -------------------------------- Blob inheritance --------------------------------
    it("is an instance of Blob (prototype chain wired up)", function () {
        // BJS core (fileTools, Offline/database, abstractEngine,
        // thinNativeEngine) branches on `instanceof Blob`. File must
        // satisfy that check for File inputs to take the Blob path,
        // matching the WHATWG spec where File is a Blob subtype.
        const file = new File(["x"], "x.txt");
        expect(file instanceof Blob).to.equal(true);
        expect(file instanceof File).to.equal(true);
    });
});
