import { expect } from "chai";

describe("TextEncoder", function () {
    it("should expose encoding === 'utf-8'", function () {
        const encoder = new TextEncoder();
        expect(encoder.encoding).to.equal("utf-8");
    });

    it("should encode an ASCII string into UTF-8 bytes", function () {
        const encoder = new TextEncoder();
        const bytes = encoder.encode("Hello");
        expect(Array.from(bytes)).to.eql([72, 101, 108, 108, 111]);
    });

    it("should return an empty Uint8Array when called with no argument", function () {
        const encoder = new TextEncoder();
        const bytes = encoder.encode();
        expect(bytes.length).to.equal(0);
    });

    it("should return an empty Uint8Array for undefined input", function () {
        const encoder = new TextEncoder();
        const bytes = encoder.encode(undefined);
        expect(bytes.length).to.equal(0);
    });

    it("should encode a multi-byte UTF-8 string", function () {
        const encoder = new TextEncoder();
        // "é" is U+00E9 -> 0xC3 0xA9 in UTF-8
        const bytes = encoder.encode("é");
        expect(Array.from(bytes)).to.eql([0xC3, 0xA9]);
    });

    it("should encode a string containing a null byte", function () {
        const encoder = new TextEncoder();
        const bytes = encoder.encode("H\0i");
        expect(Array.from(bytes)).to.eql([72, 0, 105]);
    });
});
