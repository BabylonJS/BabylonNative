import { expect } from "chai";

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
