import { expect } from "chai";

describe("Headers", function () {
    // Focused ports from WPT fetch/api/headers.
    it("normalizes names and values and combines repeated fields", function () {
        const headers = new Headers([
            ["X-Test", "  first\t"],
            ["x-test", "second"],
            ["X-Other", "\r\n value \n"]
        ]);
        expect(headers.get("X-TEST")).to.equal("first, second");
        expect(headers.get("x-other")).to.equal("value");
        expect(Array.from(headers.keys())).to.deep.equal(["x-other", "x-test"]);
    });

    it("validates sequence shape and HTTP ByteStrings", function () {
        expect(() => new Headers(null as any)).to.throw();
        expect(() => new Headers([["missing-value"]] as any)).to.throw();
        expect(() => new Headers([["too", "many", "values"]] as any)).to.throw();
        expect(() => new Headers([["invalid name", "value"]])).to.throw();
        expect(() => new Headers([["valid", "a\0b"]])).to.throw();
        expect(() => new Headers([["invalidĀ", "value"]])).to.throw();
    });

    it("preserves Set-Cookie fields while combining other duplicates", function () {
        const headers = new Headers([
            ["set-cookie", "a=1"],
            ["x-value", "first"],
            ["Set-Cookie", "b=2"],
            ["X-Value", "second"]
        ]);
        expect(headers.getSetCookie()).to.deep.equal(["a=1", "b=2"]);
        expect(Array.from(headers)).to.deep.equal([
            ["set-cookie", "a=1"],
            ["set-cookie", "b=2"],
            ["x-value", "first, second"]
        ]);
    });

    it("keeps iteration live when headers are changed", function () {
        const headers = new Headers({ bar: "0", baz: "1", foo: "2" });
        const seen: string[] = [];
        for (const [name] of headers) {
            seen.push(name);
            headers.delete("foo");
        }
        expect(seen).to.deep.equal(["bar", "baz"]);
    });

    it("copies an initializer and honors a custom iterator", function () {
        const source = new Headers({ ignored: "value" });
        source[Symbol.iterator] = function* () {
            yield ["custom", "value"];
        };
        const copy = new Headers(source);
        source.set("custom", "changed");
        expect(Array.from(copy)).to.deep.equal([["custom", "value"]]);
    });
});

describe("Response", function () {
    // Focused ports from WPT fetch/api/response.
    it("exposes browser defaults and validates response metadata", function () {
        const response = new Response();
        expect(String(response)).to.equal("[object Response]");
        expect(response.status).to.equal(200);
        expect(response.statusText).to.equal("");
        expect(response.ok).to.equal(true);
        expect(response.type).to.equal("default");
        expect(response.url).to.equal("");
        expect(response.body).to.equal(null);
        expect(response.headers).to.equal(response.headers);

        expect(() => new Response("", { status: 199 })).to.throw(RangeError);
        expect(() => new Response("", { status: 600 })).to.throw(RangeError);
        expect(() => new Response("", { statusText: "bad\ntext" })).to.throw(TypeError);
        expect(() => new Response("body", { status: 204 })).to.throw(TypeError);
    });

    it("streams strings and consumes a body only once", async function () {
        const response = new Response("streamed text");
        expect(response.body).to.be.instanceOf(ReadableStream);
        expect(response.bodyUsed).to.equal(false);
        expect(await response.text()).to.equal("streamed text");
        expect(response.bodyUsed).to.equal(true);

        let rejected = false;
        try {
            await response.arrayBuffer();
        } catch (error) {
            rejected = error instanceof TypeError;
        }
        expect(rejected).to.equal(true);
    });

    // Ported from WPT fetch/api/response/response-consume-empty.any.js.
    it("consumes a null body as empty without disturbing it", async function () {
        const textResponse = new Response();
        expect(await textResponse.text()).to.equal("");
        expect(textResponse.bodyUsed).to.equal(false);

        const bufferResponse = new Response();
        expect((await bufferResponse.arrayBuffer()).byteLength).to.equal(0);
        expect(bufferResponse.bodyUsed).to.equal(false);

        const bytesResponse = new Response();
        expect((await bytesResponse.bytes()).byteLength).to.equal(0);
        expect(bytesResponse.bodyUsed).to.equal(false);

        const blobResponse = new Response();
        expect((await blobResponse.blob()).size).to.equal(0);
        expect(blobResponse.bodyUsed).to.equal(false);

        const jsonResponse = new Response();
        let jsonRejected = false;
        try {
            await jsonResponse.json();
        } catch {
            jsonRejected = true;
        }
        expect(jsonRejected).to.equal(true);
        expect(jsonResponse.bodyUsed).to.equal(false);
    });

    // Byte streams reject zero-length enqueues. An empty BufferSource still
    // represents a non-null body, but its stream must close without a chunk.
    it("consumes an empty BufferSource without enqueueing an empty byte chunk", async function () {
        const response = new Response(new Uint8Array(0));
        expect(response.body).to.be.instanceOf(ReadableStream);
        expect(response.bodyUsed).to.equal(false);
        expect(new Uint8Array(await response.arrayBuffer())).to.eql(new Uint8Array(0));
        expect(response.bodyUsed).to.equal(true);
    });

    it("snapshots mutable BufferSource input once", async function () {
        const input = new Uint8Array([80, 65, 83, 83]);
        const response = new Response(input);
        input.fill(0);
        expect(await response.text()).to.equal("PASS");
    });

    // Adapted from WebKit LayoutTests/fetch/body-init.html.
    it("stringifies integer and object bodies", async function () {
        expect(await new Response(1 as any).text()).to.equal("1");
        expect(await new Response({} as any).text()).to.equal("[object Object]");
    });

    it("sets inferred content types without replacing explicit headers", async function () {
        const text = new Response("text");
        expect(text.headers.get("content-type")).to.equal("text/plain;charset=UTF-8");

        const blob = new Response(new Blob(["blob"], { type: "application/example" }));
        expect(blob.headers.get("content-type")).to.equal("application/example");

        const explicit = new Response("text", { headers: { "content-type": "text/custom" } });
        expect(explicit.headers.get("content-type")).to.equal("text/custom");
    });

    it("clones stream branches for independent consumption", async function () {
        const response = new Response("clone body", {
            headers: { "x-test": "value" },
            status: 201,
            statusText: "Created"
        });
        const clone = response.clone();
        expect(clone.status).to.equal(201);
        expect(clone.headers.get("x-test")).to.equal("value");
        expect(await response.text()).to.equal("clone body");
        expect(await clone.text()).to.equal("clone body");
    });

    it("tracks direct stream reads and rejects invalid body chunks", async function () {
        const direct = new Response("body");
        const reader = direct.body!.getReader();
        expect(direct.bodyUsed).to.equal(false);
        await reader.read();
        expect(direct.bodyUsed).to.equal(true);

        const invalid = new Response(new ReadableStream({
            start(controller) {
                controller.enqueue("not bytes");
                controller.close();
            }
        }) as any);
        let rejected = false;
        try {
            await invalid.bytes();
        } catch (error) {
            rejected = error instanceof TypeError;
        }
        expect(rejected).to.equal(true);
    });

    // Adapted from Firefox dom/fetch/tests/crashtests/1939295.html. The
    // unresolved read must remain safe through runtime teardown.
    it("does not crash while consuming an open empty stream", function () {
        const pending = new Response(new ReadableStream()).text();
        expect(pending).to.be.instanceOf(Promise);
    });

    // Adapted from WebKit's imported many-empty-chunks-crash.html.
    it("consumes many empty chunks without retaining growing byte buffers", async function () {
        const response = new Response(new ReadableStream({
            start(controller) {
                for (let index = 0; index < 40000; ++index) {
                    controller.enqueue(new Uint8Array());
                }
                controller.close();
            }
        }));
        expect((await response.arrayBuffer()).byteLength).to.equal(0);
    });

    // Bounded adaptation of Chromium's call-extra-crash-is-disturbed.html.
    // QuickJS terminates before a JS-defined getter can run after a real
    // native stack overflow, so retain the deep-call regression portably.
    it("reads bodyUsed from a deep call stack without recursion", function () {
        const response = new Response(new ReadableStream());
        function readAtDepth(depth: number): boolean {
            return depth === 0 ? response.bodyUsed : readAtDepth(depth - 1);
        }
        expect(readAtDepth(128)).to.equal(false);
    });

    it("provides error, redirect, and JSON factories", async function () {
        const error = Response.error();
        expect(error.type).to.equal("error");
        expect(error.status).to.equal(0);

        const redirect = Response.redirect("https://example.com/path", 307);
        expect(redirect.status).to.equal(307);
        expect(redirect.headers.get("location")).to.equal("https://example.com/path");

        const json = Response.json({ value: 42 });
        expect(json.headers.get("content-type")).to.equal("application/json");
        expect(await json.json()).to.deep.equal({ value: 42 });
    });

    it("allows mutation on constructed response headers", function () {
        const headers = new Response().headers;
        headers.set("x-test", "mutable");
        expect(headers.get("x-test")).to.equal("mutable");
    });

    it("keeps error and redirect headers immutable after cloning", function () {
        for (const response of [Response.error(), Response.redirect("https://example.com/")]) {
            for (const headers of [response.headers, response.clone().headers]) {
                expect(() => headers.append("x-test", "x")).to.throw(TypeError);
                expect(() => headers.set("x-test", "x")).to.throw(TypeError);
                expect(() => headers.delete("x-test")).to.throw(TypeError);
            }
        }
    });

    it("filters forbidden response Set-Cookie fields at construction", function () {
        const response = new Response(null, {
            headers: {
                "set-cookie": "secret=value",
                "set-cookie2": "legacy=value"
            }
        });
        response.headers.append("Set-Cookie", "other=value");
        expect(response.headers.getSetCookie()).to.deep.equal([]);
        expect(response.headers.has("set-cookie2")).to.equal(false);
    });
});
