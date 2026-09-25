import { expect } from "chai";

describe("napi class prototype isolation (#172)", function () {
    // Regression coverage for #172: napi-defined class constructors must each
    // get a fresh per-class object as their `.prototype` property. Previously
    // on JSC every napi class shared the global Object.prototype, so writes
    // to one class's prototype polluted every object and every plain `{}`
    // erroneously satisfied `instanceof` for every napi class.

    it("Blob.prototype is not the global Object.prototype", function () {
        expect(Blob.prototype).to.not.equal(Object.prototype);
    });

    it("Blob.prototype chains to Object.prototype", function () {
        expect(Object.getPrototypeOf(Blob.prototype)).to.equal(Object.prototype);
    });

    it("Blob.prototype.constructor points back to Blob", function () {
        expect(Blob.prototype.constructor).to.equal(Blob);
    });

    it("instances inherit from Blob.prototype", function () {
        const blob = new Blob([]);
        expect(Object.getPrototypeOf(blob)).to.equal(Blob.prototype);
    });

    it("plain objects are not instanceof Blob", function () {
        expect({} instanceof Blob).to.equal(false);
    });

    it("writes to Blob.prototype do not pollute Object.prototype", function () {
        const KEY = "__jrh172_blob_prototype_marker__";
        const proto = Blob.prototype as any;
        try {
            proto[KEY] = 1;
            expect(KEY in {}).to.equal(false);
        } finally {
            delete proto[KEY];
        }
    });
});
