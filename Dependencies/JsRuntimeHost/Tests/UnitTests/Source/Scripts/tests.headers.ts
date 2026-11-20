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
