import { expect } from "chai";

describe("TextDecoder", function () {
    it("should decode a Uint8Array to a string", function () {
        const decoder = new TextDecoder();
        const encoded = new Uint8Array([72, 101, 108, 108, 111]); // "Hello"
        const result = decoder.decode(encoded);
        expect(result).to.equal("Hello");
    });

    it("should decode an empty Uint8Array to an empty string", function () {
        const decoder = new TextDecoder();
        const result = decoder.decode(new Uint8Array([]));
        expect(result).to.equal("");
    });

    it("should decode an ArrayBuffer to a string", function () {
        const decoder = new TextDecoder();
        const buffer = new Uint8Array([87, 111, 114, 108, 100]).buffer; // "World"
        const result = decoder.decode(buffer);
        expect(result).to.equal("World");
    });

    it("should decode a TypedArray subarray with non-zero byteOffset", function () {
        const decoder = new TextDecoder();
        const full = new Uint8Array([88, 72, 105]); // "XHi"
        const sub = full.subarray(1); // [72, 105] -> "Hi"
        const result = decoder.decode(sub);
        expect(result).to.equal("Hi");
    });

    it("should decode a Uint8Array containing a null byte", function () {
        const decoder = new TextDecoder();
        const encoded = new Uint8Array([72, 0, 105]); // "H\0i"
        const result = decoder.decode(encoded);
        expect(result).to.equal("H\0i");
        expect(result.length).to.equal(3);
    });

    it("throwing from the constructor repeatedly does not corrupt native state", function () {
        // Regression for a Chakra N-API ObjectWrap bug: when a wrapped
        // constructor throws, the native instance is destroyed during stack
        // unwinding but the wrap finalizer stayed attached to `this`, so a
        // later GC ran the finalizer on freed memory (heap corruption). Throw
        // many times to create many dangling wraps, then allocate/decode to
        // exercise the heap and surface any corruption within this test run.
        for (let i = 0; i < 100; ++i) {
            expect(() => new TextDecoder("iso-8859-2")).to.throw();
        }
        const decoder = new TextDecoder("utf-8");
        expect(decoder.decode(new Uint8Array([79, 75]))).to.equal("OK");
    });

    it("should accept the WHATWG 'utf8' label (no hyphen)", function () {
        const decoder = new TextDecoder("utf8");
        const result = decoder.decode(new Uint8Array([72, 105])); // "Hi"
        expect(result).to.equal("Hi");
    });

    it("should accept utf-8 labels case-insensitively and with surrounding whitespace", function () {
        for (const label of ["UTF-8", "UTF8", "  utf-8  ", "\tUtf8\n"]) {
            const decoder = new TextDecoder(label);
            expect(decoder.decode(new Uint8Array([79, 75]))).to.equal("OK");
        }
    });

    it("should accept the other WHATWG utf-8 aliases", function () {
        for (const label of ["unicode-1-1-utf-8", "unicode11utf8", "unicode20utf8", "x-unicode20utf8"]) {
            const decoder = new TextDecoder(label);
            expect(decoder.decode(new Uint8Array([79, 75]))).to.equal("OK");
        }
    });

    it("should still throw for a genuinely unsupported encoding", function () {
        expect(() => new TextDecoder("iso-8859-2")).to.throw();
    });

    it("should decode utf-16le", function () {
        const decoder = new TextDecoder("utf-16le");
        // "Hi" as UTF-16LE code units.
        expect(decoder.decode(new Uint8Array([0x48, 0x00, 0x69, 0x00]))).to.equal("Hi");
    });

    it("should decode utf-16be", function () {
        const decoder = new TextDecoder("utf-16be");
        expect(decoder.decode(new Uint8Array([0x00, 0x48, 0x00, 0x69]))).to.equal("Hi");
    });

    it("should accept the other WHATWG utf-16 aliases as little endian", function () {
        for (const label of ["utf-16", "ucs-2", "unicode", "unicodeFEFF", "csunicode", "iso-10646-ucs-2"]) {
            const decoder = new TextDecoder(label);
            expect(decoder.decode(new Uint8Array([0x4F, 0x00, 0x4B, 0x00]))).to.equal("OK");
        }
        expect(new TextDecoder("unicodeFFFE").decode(new Uint8Array([0x00, 0x4F, 0x00, 0x4B]))).to.equal("OK");
    });

    it("should strip a leading byte order mark", function () {
        expect(new TextDecoder("utf-16le").decode(new Uint8Array([0xFF, 0xFE, 0x48, 0x00]))).to.equal("H");
        expect(new TextDecoder("utf-16be").decode(new Uint8Array([0xFE, 0xFF, 0x00, 0x48]))).to.equal("H");
    });

    it("should decode utf-16 outside the BMP and preserve null code units", function () {
        // U+1F600 as a surrogate pair, then U+0000, then "A".
        const decoder = new TextDecoder("utf-16le");
        const result = decoder.decode(new Uint8Array([0x3D, 0xD8, 0x00, 0xDE, 0x00, 0x00, 0x41, 0x00]));
        expect(result).to.equal("\u{1F600}\0A");
        expect(result.length).to.equal(4);
    });

    it("should replace a trailing odd UTF-16 byte with U+FFFD", function () {
        expect(new TextDecoder("utf-16le").decode(new Uint8Array([0x48, 0x00, 0x00]))).to.equal("H\uFFFD");
        expect(new TextDecoder("utf-16be").decode(new Uint8Array([0x00, 0x48, 0x00]))).to.equal("H\uFFFD");
        expect(new TextDecoder("utf-16le").decode(new Uint8Array([0x00]))).to.equal("\uFFFD");
        expect(new TextDecoder("utf-16be").decode(new Uint8Array([0x00]))).to.equal("\uFFFD");
    });

    it("should replace unpaired UTF-16 surrogates with U+FFFD", function () {
        // Lone lead U+D800.
        expect(new TextDecoder("utf-16le").decode(new Uint8Array([0x00, 0xD8]))).to.equal("\uFFFD");
        expect(new TextDecoder("utf-16be").decode(new Uint8Array([0xD8, 0x00]))).to.equal("\uFFFD");
        // Lone trail U+DC00.
        expect(new TextDecoder("utf-16le").decode(new Uint8Array([0x00, 0xDC]))).to.equal("\uFFFD");
        expect(new TextDecoder("utf-16be").decode(new Uint8Array([0xDC, 0x00]))).to.equal("\uFFFD");
        // Lead followed by BMP 'A': replacement, then reprocess 'A'.
        expect(new TextDecoder("utf-16le").decode(new Uint8Array([0x00, 0xD8, 0x41, 0x00]))).to.equal("\uFFFDA");
        expect(new TextDecoder("utf-16be").decode(new Uint8Array([0xD8, 0x00, 0x00, 0x41]))).to.equal("\uFFFDA");
        // Unpaired lead plus leftover odd byte is a single end-of-queue replacement.
        expect(new TextDecoder("utf-16le").decode(new Uint8Array([0x00, 0xD8, 0x00]))).to.equal("\uFFFD");
        expect(new TextDecoder("utf-16be").decode(new Uint8Array([0xD8, 0x00, 0x00]))).to.equal("\uFFFD");
    });
});
