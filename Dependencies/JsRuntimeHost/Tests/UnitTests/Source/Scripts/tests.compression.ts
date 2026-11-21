import { expect } from "chai";

describe("Compression streams", function () {
    this.timeout(20000);

    type SupportedCompressionFormat = "deflate" | "deflate-raw" | "gzip";
    type CompressionTransform = {
        readable: ReadableStream<Uint8Array>;
        writable: WritableStream<BufferSource>;
    };

    const formats: SupportedCompressionFormat[] = ["deflate", "deflate-raw", "gzip"];
    const expectedOutput = new TextEncoder().encode("expected output");
    const compressedFixtures: Array<[SupportedCompressionFormat, Uint8Array]> = [
        ["deflate", new Uint8Array([120, 156, 75, 173, 40, 72, 77, 46, 73, 77, 81, 200, 47, 45, 41, 40, 45, 1, 0, 48, 173, 6, 36])],
        ["gzip", new Uint8Array([31, 139, 8, 0, 0, 0, 0, 0, 0, 3, 75, 173, 40, 72, 77, 46, 73, 77, 81, 200, 47, 45, 41, 40, 45, 1, 0, 176, 1, 57, 179, 15, 0, 0, 0])],
        ["deflate-raw", new Uint8Array([75, 173, 40, 72, 77, 46, 73, 77, 81, 200, 47, 45, 41, 40, 45, 1, 0])],
    ];

    async function concatenateStream(readable: ReadableStream<Uint8Array>): Promise<Uint8Array> {
        const reader = readable.getReader();
        const chunks: Uint8Array[] = [];
        let byteLength = 0;
        while (true) {
            const result = await reader.read();
            if (result.done) {
                break;
            }
            expect(result.value).to.be.instanceOf(Uint8Array);
            chunks.push(result.value);
            byteLength += result.value.byteLength;
        }

        const output = new Uint8Array(byteLength);
        let offset = 0;
        for (const chunk of chunks) {
            output.set(chunk, offset);
            offset += chunk.byteLength;
        }
        return output;
    }

    async function transformChunks(transform: CompressionTransform, chunks: BufferSource[]): Promise<Uint8Array> {
        const output = concatenateStream(transform.readable);
        const writer = transform.writable.getWriter();
        for (const chunk of chunks) {
            await writer.write(chunk);
        }
        await writer.close();
        return output;
    }

    async function roundTrip(input: Uint8Array, format: SupportedCompressionFormat): Promise<Uint8Array> {
        const compressed = await transformChunks(new CompressionStream(format), [input]);
        return transformChunks(new DecompressionStream(format), [compressed]);
    }

    it("exposes browser-shaped constructors, properties, and brand checks", function () {
        const compression = new CompressionStream("gzip");
        const decompression = new DecompressionStream("gzip");
        expect(compression.readable).to.be.instanceOf(ReadableStream);
        expect(compression.writable).to.be.instanceOf(WritableStream);
        expect(decompression.readable).to.be.instanceOf(ReadableStream);
        expect(decompression.writable).to.be.instanceOf(WritableStream);
        expect(String(compression)).to.equal("[object CompressionStream]");
        expect(String(decompression)).to.equal("[object DecompressionStream]");

        const compressionReadable = Object.getOwnPropertyDescriptor(CompressionStream.prototype, "readable")!.get!;
        const decompressionWritable = Object.getOwnPropertyDescriptor(DecompressionStream.prototype, "writable")!.get!;
        expect(() => compressionReadable.call({})).to.throw(TypeError);
        expect(() => decompressionWritable.call({})).to.throw(TypeError);
    });

    // Focused ports from WPT compression/*-constructor-error.any.js.
    it("validates and converts constructor formats", function () {
        expect(() => new CompressionStream()).to.throw(TypeError);
        expect(() => new DecompressionStream()).to.throw(TypeError);
        expect(() => new CompressionStream("invalid" as any)).to.throw(TypeError);
        expect(() => new DecompressionStream("GZIP" as any)).to.throw(TypeError);
        expect(() => new CompressionStream(Symbol("gzip") as any)).to.throw(TypeError);

        const failure = new Error("format conversion failed");
        let thrown: unknown;
        try {
            new DecompressionStream({ toString() { throw failure; } } as any);
        } catch (error) {
            thrown = error;
        }
        expect(thrown).to.equal(failure);
    });

    // Focused ports from WPT compression-stream.any.js,
    // compression-multiple-chunks.any.js, and including-empty-chunk.any.js.
    it("round-trips all supported formats across multiple and empty chunks", async function () {
        const middleBacking = new Uint8Array(new TextEncoder().encode("!from browser-shaped streams!"));
        const inputChunks = [
            new TextEncoder().encode("Hello "),
            new Uint8Array(),
            middleBacking.subarray(1, middleBacking.byteLength - 1),
        ];
        const expected = new TextEncoder().encode("Hello from browser-shaped streams");

        for (const format of formats) {
            const compressed = await transformChunks(new CompressionStream(format), inputChunks);
            const output = await transformChunks(new DecompressionStream(format), [compressed]);
            expect(Array.from(output)).to.deep.equal(Array.from(expected));
        }
    });

    // Ported from WPT decompression-buffersource.any.js.
    it("accepts ArrayBuffer, typed-array, and DataView input", async function () {
        const fixtures: Array<[SupportedCompressionFormat, number[], string]> = [
            ["deflate", [120, 156, 75, 52, 48, 52, 50, 54, 49, 53, 3, 0, 8, 136, 1, 199], "a0123456"],
            ["gzip", [31, 139, 8, 0, 0, 0, 0, 0, 0, 3, 75, 52, 48, 52, 2, 0, 216, 252, 63, 136, 4, 0, 0, 0], "a012"],
            ["deflate-raw", [0, 6, 0, 249, 255, 65, 66, 67, 68, 69, 70, 1, 0, 0, 255, 255], "ABCDEF"],
        ];

        for (const [format, values, expected] of fixtures) {
            const expectedBytes = Array.from(new TextEncoder().encode(expected));
            const makeBuffer = () => new Uint8Array(values).buffer as ArrayBuffer;
            const padded = new Uint8Array(values.length + 2);
            padded.set(values, 1);
            const inputs: BufferSource[] = [
                makeBuffer(),
                new Int8Array(makeBuffer()),
                new Uint16Array(makeBuffer()),
                new DataView(makeBuffer()),
                new DataView(padded.buffer, 1, values.length),
            ];
            for (const input of inputs) {
                const output = await transformChunks(new DecompressionStream(format), [input]);
                expect(Array.from(output)).to.deep.equal(expectedBytes);
            }
        }
    });

    // Focused ports from WPT decompression-split-chunk.any.js and
    // decompression-uint8array-output.any.js.
    it("decompresses input split at every small chunk boundary", async function () {
        for (const [format, fixture] of compressedFixtures) {
            for (let chunkSize = 1; chunkSize < 16; ++chunkSize) {
                const chunks: Uint8Array[] = [];
                for (let offset = 0; offset < fixture.byteLength; offset += chunkSize) {
                    chunks.push(fixture.slice(offset, offset + chunkSize));
                }
                const output = await transformChunks(new DecompressionStream(format), chunks);
                expect(Array.from(output)).to.deep.equal(Array.from(expectedOutput));
            }
        }
    });

    // Ported from WPT compression-large-flush-output.any.js.
    it("does not truncate output produced while closing", async function () {
        const encoded = new TextEncoder().encode(JSON.stringify(Array.from({ length: 10000 }, (_, index) => index)));
        const input = encoded.subarray(0, 35579);
        for (const format of formats) {
            expect(Array.from(await roundTrip(input, format))).to.deep.equal(Array.from(input));
        }
    });

    // WPT decompression-extra-input.any.js requires already-produced output
    // to be observable before the stream reports the trailing-data error.
    it("emits valid output before rejecting extra compressed input", async function () {
        for (const [format, fixture] of compressedFixtures) {
            const stream = new DecompressionStream(format);
            const reader = stream.readable.getReader();
            const writer = stream.writable.getWriter();
            const firstRead = reader.read();
            const write = writer.write(new Uint8Array([...fixture, 0])).catch(error => error);
            const first = await firstRead;
            expect(first.done).to.equal(false);
            expect(Array.from(first.value!)).to.deep.equal(Array.from(expectedOutput));

            let readFailure: unknown;
            try { await reader.read(); } catch (error) { readFailure = error; }
            expect(readFailure).to.be.instanceOf(TypeError);
            expect(await write).to.be.instanceOf(TypeError);
        }
    });

    // Focused ports from WPT compression-bad-chunks.any.js and
    // decompression-bad-chunks.any.js.
    it("errors both sides of the transform for non-BufferSource chunks", async function () {
        for (const create of [
            () => new CompressionStream("gzip"),
            () => new DecompressionStream("gzip"),
        ]) {
            const stream = create();
            const reader = stream.readable.getReader();
            const writer = stream.writable.getWriter();
            const read = reader.read().catch(error => error);
            const write = writer.write({} as any).catch(error => error);
            expect(await write).to.be.instanceOf(TypeError);
            expect(await read).to.be.instanceOf(TypeError);
        }
    });

    // Focused ports from WPT decompression-corrupt-input.any.js and the
    // truncated-input check in Chromium's InflateTransformer.
    it("rejects corrupt and truncated compressed input", async function () {
        for (const [format, fixture] of compressedFixtures) {
            const inputs = [
                fixture.subarray(0, fixture.byteLength - 1),
                new Uint8Array(fixture.map((value, index) => index === 0 ? value ^ 0xff : value)),
            ];
            for (const input of inputs) {
                const stream = new DecompressionStream(format);
                const output = concatenateStream(stream.readable).catch(error => error);
                const writer = stream.writable.getWriter();
                await writer.write(input).catch(() => undefined);
                const close = writer.close().catch(error => error);
                expect(await close).to.be.instanceOf(TypeError);
                expect(await output).to.be.instanceOf(TypeError);
            }
        }
    });

    // Chromium buffers output before enqueue because enqueue may execute
    // JavaScript that mutates or detaches the input still being consumed.
    // This regression test forces that reentrancy without needing postMessage.
    it("finishes consuming input before an enqueue callback can mutate it", async function () {
        const input = new Uint8Array(256 * 1024);
        let state = 0x12345678;
        for (let index = 0; index < input.length; ++index) {
            state ^= state << 13;
            state ^= state >>> 17;
            state ^= state << 5;
            input[index] = state & 0xff;
        }
        const expected = input.slice();

        const prototype = TransformStreamDefaultController.prototype as any;
        const originalEnqueue = prototype.enqueue;
        let mutated = false;
        prototype.enqueue = function(chunk: unknown) {
            if (!mutated) {
                mutated = true;
                input.fill(0);
            }
            return originalEnqueue.call(this, chunk);
        };

        let compressed: Uint8Array;
        try {
            compressed = await transformChunks(new CompressionStream("deflate"), [input]);
        } finally {
            prototype.enqueue = originalEnqueue;
        }

        expect(mutated).to.equal(true);
        const output = await transformChunks(new DecompressionStream("deflate"), [compressed!]);
        expect(Array.from(output)).to.deep.equal(Array.from(expected));
    });

    // Empty chunks are common in WebKit and WPT stream regressions. They must
    // not allocate codec output or accumulate retained per-write state.
    it("handles a long sequence of empty writes without retained output", async function () {
        const chunks = Array.from({ length: 1024 }, () => new Uint8Array());
        const compressed = await transformChunks(new CompressionStream("gzip"), chunks);
        const output = await transformChunks(new DecompressionStream("gzip"), [compressed]);
        expect(output.byteLength).to.equal(0);
    });

    // Repeated short-lived streams model asset-heavy native applications. A
    // completed stream must release zlib and scratch storage before JS GC.
    it("releases completed codec state across repeated streams", async function () {
        const fixture = compressedFixtures.find(([format]) => format === "gzip")![1];
        for (let iteration = 0; iteration < 128; ++iteration) {
            const output = await transformChunks(new DecompressionStream("gzip"), [fixture]);
            expect(Array.from(output)).to.deep.equal(Array.from(expectedOutput));
        }
    });
});
