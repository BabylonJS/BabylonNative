import { expect } from "chai";

describe("Web Streams", function () {
    // Focused ports from the WHATWG Streams WPT suites at c05b4473:
    // readable-streams/general.any.js and tee.any.js,
    // writable-streams/write.any.js, and transform-streams/general.any.js.
    it("installs the standard stream constructors", function () {
        expect(ReadableStream).to.be.a("function");
        expect(WritableStream).to.be.a("function");
        expect(TransformStream).to.be.a("function");
        expect(ByteLengthQueuingStrategy).to.be.a("function");
        expect(CountQueuingStrategy).to.be.a("function");
    });

    it("delivers queued chunks in order and closes the reader", async function () {
        const stream = new ReadableStream({
            start(controller) {
                controller.enqueue("a");
                controller.enqueue("b");
                controller.close();
            }
        });
        const reader = stream.getReader();

        expect(await reader.read()).to.deep.equal({ value: "a", done: false });
        expect(await reader.read()).to.deep.equal({ value: "b", done: false });
        expect(await reader.read()).to.deep.equal({ value: undefined, done: true });
        await reader.closed;
    });

    it("propagates a rejected pull to read and closed", async function () {
        const failure = new Error("pull failed");
        const reader = new ReadableStream({
            pull() {
                return Promise.reject(failure);
            }
        }).getReader();

        let readFailure: unknown;
        let closedFailure: unknown;
        try { await reader.read(); } catch (error) { readFailure = error; }
        try { await reader.closed; } catch (error) { closedFailure = error; }
        expect(readFailure).to.equal(failure);
        expect(closedFailure).to.equal(failure);
    });

    it("tees without one branch consuming the other", async function () {
        const [first, second] = new ReadableStream({
            start(controller) {
                controller.enqueue("a");
                controller.enqueue("b");
                controller.close();
            }
        }).tee();
        const firstReader = first.getReader();
        const secondReader = second.getReader();

        expect(await firstReader.read()).to.deep.equal({ value: "a", done: false });
        expect(await firstReader.read()).to.deep.equal({ value: "b", done: false });
        expect(await firstReader.read()).to.deep.equal({ value: undefined, done: true });
        expect(await secondReader.read()).to.deep.equal({ value: "a", done: false });
    });

    it("waits for asynchronous writes before closing", async function () {
        const stored: number[] = [];
        const writable = new WritableStream({
            write(chunk) {
                return Promise.resolve().then(() => stored.push(chunk));
            }
        });
        const writer = writable.getWriter();

        writer.write(1);
        writer.write(2);
        await writer.close();
        expect(stored).to.deep.equal([1, 2]);
    });

    it("applies transform output and backpressure", async function () {
        const transform = new TransformStream({
            transform(chunk, controller) {
                controller.enqueue(chunk.toUpperCase());
            }
        });
        const writer = transform.writable.getWriter();
        const reader = transform.readable.getReader();
        const write = writer.write("native");

        expect(await reader.read()).to.deep.equal({ value: "NATIVE", done: false });
        await write;
        await writer.close();
        expect(await reader.read()).to.deep.equal({ value: undefined, done: true });
    });

    it("supports BYOB reads from byte streams", async function () {
        let sent = false;
        const stream = new ReadableStream({
            type: "bytes",
            pull(controller) {
                if (!sent) {
                    sent = true;
                    controller.enqueue(new Uint8Array([8, 241, 48, 123, 151]));
                    controller.close();
                }
            }
        } as any);
        const reader = stream.getReader({ mode: "byob" });
        const result = await reader.read(new Uint8Array(8));

        expect(result.done).to.equal(false);
        expect(Array.from(result.value!)).to.deep.equal([8, 241, 48, 123, 151]);
        expect((await reader.read(new Uint8Array(8))).done).to.equal(true);
    });

    // Ported from Firefox's dom/streams/test/xpcshell/subclassing.js.
    it("supports subclassed streams, readers, and queuing strategies", async function () {
        class SubclassedStream extends ReadableStream {}
        class SubclassedStrategy extends CountQueuingStrategy {}
        const stream = new SubclassedStream({
            start(controller) {
                controller.enqueue("first");
                controller.close();
            }
        });
        const Reader = stream.getReader().constructor as typeof ReadableStreamDefaultReader;
        class SubclassedReader extends Reader {}

        expect(stream).to.be.instanceOf(ReadableStream);
        expect(new SubclassedStrategy({ highWaterMark: 4 }).highWaterMark).to.equal(4);

        const secondStream = new ReadableStream({
            start(controller) {
                controller.enqueue("second");
                controller.close();
            }
        });
        const reader = new SubclassedReader(secondStream);
        expect(await reader.read()).to.deep.equal({ value: "second", done: false });
    });

    // Ported from Chromium's http/tests/streams/chromium/transform-stream-enqueue.html.
    it("rejects enqueues after a transform is terminated or errored", function () {
        expect(() => new TransformStream({
            start(controller) {
                controller.terminate();
                controller.enqueue("late");
            }
        })).to.throw(TypeError);

        expect(() => new TransformStream({
            start(controller) {
                controller.error(new Error("failed"));
                controller.enqueue("late");
            }
        })).to.throw(TypeError);
    });
});
