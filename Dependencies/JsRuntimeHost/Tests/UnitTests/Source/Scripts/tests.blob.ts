import { expect } from "chai";

describe("Blob", function () {
    this.timeout(10000);

    let emptyBlobs: Blob[], helloBlobs: Blob[], stringBlob: Blob, typedArrayBlob: Blob, arrayBufferBlob: Blob, blobBlob: Blob;

    async function readStream(stream: ReadableStream<Uint8Array>, mode?: "byob"): Promise<number[]> {
        const reader: any = stream.getReader(mode === undefined ? undefined : { mode });
        const bytes: number[] = [];
        while (true) {
            const result = mode === "byob"
                ? await reader.read(new Uint8Array(64))
                : await reader.read();
            if (result.done) {
                return bytes;
            }
            bytes.push(...Array.from(result.value as Uint8Array));
        }
    }

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

    // Focused ports from WPT FileAPI/blob/Blob-constructor.any.js.
    it("accepts iterable parts and preserves their order", async function () {
        const parts = {
            *[Symbol.iterator]() {
                yield "foo";
                yield new Uint8Array([98, 97, 114]);
                yield new Blob(["baz"]);
            }
        };
        const blob = new Blob(parts as any);
        expect(blob.size).to.equal(9);
        expect(await blob.text()).to.equal("foobarbaz");
    });

    it("uses an Array's overridden iterator", async function () {
        const parts = ["ignored"];
        parts[Symbol.iterator] = function* () {
            yield "custom";
        };

        expect(await new Blob(parts).text()).to.equal("custom");
    });

    it("closes an iterator when BlobPart conversion throws", function () {
        let closed = false;
        const badPart = {
            toString() {
                throw new Error("part conversion failed");
            }
        };
        const parts = (function* () {
            try {
                yield badPart;
            } finally {
                closed = true;
            }
        })();

        // QuickJS currently wraps an exception rethrown through a native
        // constructor as its generic JS error type, so assert the observable
        // iterator-close behavior separately from the adapter's error text.
        expect(() => new Blob(parts as any)).to.throw();
        expect(closed).to.equal(true);
    });

    it("observes BlobPart array mutations during iteration", async function () {
        const parts: any[] = [
            {
                toString() {
                    parts.pop();
                    return "PASS";
                }
            },
            {
                toString() {
                    throw new Error("removed part was converted");
                }
            }
        ];

        expect(await new Blob(parts).text()).to.equal("PASS");
    });

    it("converts parts before reading options in WebIDL order", function () {
        const accesses: string[] = [];
        const part = {
            toString() {
                accesses.push("part");
                return "data";
            }
        };
        new Blob([part], {
            get type() {
                accesses.push("type");
                return "TEXT/PLAIN";
            },
            get endings() {
                accesses.push("endings");
                return "transparent" as EndingType;
            }
        });

        expect(accesses).to.deep.equal(["part", "endings", "type"]);
    });

    it("validates the endings enum and options dictionary", function () {
        expect(() => new Blob([], { endings: "NATIVE" as EndingType })).to.throw();
        expect(() => new Blob([], { endings: "invalid" as EndingType })).to.throw();
        for (const value of [123, true, "abc"]) {
            expect(() => new Blob([], value as any)).to.throw();
        }
        expect(() => new Blob([], null as any)).not.to.throw();
        expect(() => new Blob([], undefined)).not.to.throw();
    });

    it("exposes browser-compatible class tags", function () {
        expect(String(new Blob())).to.equal("[object Blob]");
        expect(String(new File([], "empty.txt"))).to.equal("[object File]");
    });

    it("rejects primitive parts containers", function () {
        for (const value of [null, true, 7, "not a sequence"]) {
            // Some Node-API adapters currently wrap a Napi::TypeError thrown
            // by a constructor as their generic JS error type.
            expect(() => new Blob(value as any)).to.throw();
        }
    });

    it("normalizes valid MIME types and clears invalid types", function () {
        expect(new Blob([], { type: "TEXT/PLAIN" }).type).to.equal("text/plain");
        expect(new Blob([], { type: "te\x09xt/plain" }).type).to.equal("");
        expect(new Blob([], { type: "text/\x7fplain" }).type).to.equal("");
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

    it("replaces invalid UTF-8 bytes", async function () {
        const invalid = new Uint8Array([192, 193, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255]);
        expect(await new Blob([invalid]).text()).to.equal("\ufffd".repeat(invalid.length));
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

    // Focused ports from WPT FileAPI/blob/Blob-slice.any.js.
    it("slices across part boundaries and applies clamp rounding", async function () {
        const blob = new Blob(["foo", new Blob(["bar"]), "baz"]);
        expect(await blob.slice(2, 7).text()).to.equal("obarb");
        expect(await new Blob(["abcd"]).slice(1.5).text()).to.equal("cd");
        expect(await new Blob(["abcd"]).slice(2.5).text()).to.equal("cd");
        expect(await blob.slice(-3, undefined, "TEXT/PLAIN").text()).to.equal("baz");
        expect(blob.slice(-3, undefined, "TEXT/PLAIN").type).to.equal("text/plain");
    });

    // Focused ports from WPT FileAPI/blob/Blob-stream.any.js.
    it("streams binary data through default and BYOB readers", async function () {
        const input = [8, 241, 48, 123, 151];
        const blob = new Blob([new Uint8Array(input)]);
        expect(await readStream(blob.stream())).to.deep.equal(input);
        expect(await readStream(blob.stream(), "byob")).to.deep.equal(input);
        expect(await readStream(new Blob().stream())).to.deep.equal([]);
    });

    // Adapted from WPT streams/readable-byte-streams/general.any.js BYOB
    // coverage. Small, offset views exercise the caller-owned output path.
    it("fills bounded BYOB views across Blob segment boundaries", async function () {
        const input = new Uint8Array(97);
        for (let index = 0; index < input.length; ++index) {
            input[index] = (index * 31) & 0xff;
        }

        const blob = new Blob([input.subarray(0, 11), input.subarray(11, 53), input.subarray(53)]);
        const reader = blob.stream().getReader({ mode: "byob" });
        const output: number[] = [];
        const requestSizes = [1, 3, 7, 16];
        let requestIndex = 0;
        while (true) {
            const requestSize = requestSizes[requestIndex++ % requestSizes.length];
            const request = new Uint8Array(new ArrayBuffer(requestSize + 4), 2, requestSize);
            const result = await reader.read(request);
            if (result.done) {
                break;
            }
            expect(result.value!.byteLength).to.be.at.most(requestSize);
            output.push(...Array.from(result.value!));
        }

        expect(output).to.deep.equal(Array.from(input));
    });

    it("keeps independent stream cursors after the Blob reference is dropped", async function () {
        let blob: Blob | null = new Blob(["PASS"]);
        const first = blob.stream();
        const second = blob.stream();
        blob = null;
        expect(await readStream(first)).to.deep.equal([80, 65, 83, 83]);
        expect(await readStream(second)).to.deep.equal([80, 65, 83, 83]);
    });

    // Adapted from WebKit's fast/files/blob-stream-chunks.html.
    it("chunks a large Blob and supports cancellation without retaining work", async function () {
        const blob = new Blob([new Uint8Array(5 * 1024 * 1024)]);
        const reader = blob.stream().getReader();
        const first = await reader.read();
        expect(first.done).to.equal(false);
        expect(first.value!.byteLength).to.be.at.most(64 * 1024);
        await reader.cancel();
        await reader.closed;
    });

    // Adapted from WebKit's blob-stream crash regression and exercises
    // teardown of the C++ pull-state closures under repeated construction.
    it("constructs and cancels many empty streams without crashing", async function () {
        for (let index = 0; index < 1000; ++index) {
            await new Blob().stream().cancel();
        }
    });

    // Scaled port of Firefox's dom/streams/test/xpcshell/large-pipeto.js.
    it("pipes nested shared Blob parts without corrupting chunk boundaries", async function () {
        // 40k-chunk / nested-part workloads run through the Streams polyfill's per-chunk promise
        // machinery; on the iOS simulator and Hermes that straddles mocha's default budget.
        this.timeout(60000);
        const pattern = new Uint8Array(256 * 1024);
        for (let index = 0; index < pattern.length; ++index) {
            pattern[index] = index % 256;
        }
        const pair = new Blob([pattern, pattern]);
        const nested = new Blob([pair, pair, pair, pair, pair, pair]);
        let position = 0;

        await nested.stream().pipeTo(new WritableStream({
            write(chunk: Uint8Array) {
                for (const value of chunk) {
                    const expected = position % pattern.length % 256;
                    if (value !== expected) {
                        throw new Error(`Blob stream byte ${position}: expected ${expected}, received ${value}`);
                    }
                    ++position;
                }
            }
        }));
        expect(position).to.equal(pattern.length * 12);
    });

    it("uses a File's Blob bytes when composing parts", async function () {
        const file = new File(["a", "b"], "letters.txt");
        expect(await new Blob(["<", file, ">"]).text()).to.equal("<ab>");
    });
});
