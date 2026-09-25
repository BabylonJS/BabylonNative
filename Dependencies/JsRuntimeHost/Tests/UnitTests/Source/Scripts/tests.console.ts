import { expect } from "chai";

describe("Console", function () {
    it("should log a simple console log string without error", function () {
        expect(() => console.log("I am a test string")).to.not.throw();
    });
    it("should log sequence of strings", function () {
        expect(() => console.log("I", "am", "a", "test", "string", 2, 2.345, { foo: "bar" })).to.not.throw();
    });
    it("Should log string substitutions", function () {
        expect(() => console.log("String sub: %s, float sub: %f, int sub: %d", "string", 3.1457, 3.1457)).to.not.throw();
    });
    it("Should allow numbers to substitute strings", function () {
        expect(() => console.log("Print these numbers! %s %s", 1.2345, 1)).to.not.throw();
    });
    it("Should allow strings to substitute numbers", function () {
        expect(() => console.log("Print these strings! %f %d", "foo", "bar")).to.not.throw();
    });
    it("Should allow for less substitution arguments than parameters", function () {
        expect(() => console.log("%s", "I am a string", 12345)).to.not.throw();
    });
    it("Should allow for more substitution arguments than parameters", function () {
        expect(() => console.log("%s %s", "I am a string")).to.not.throw();
    });
    it("Should allow a substitution argument with no parameter", function () {
        expect(() => console.log("%s")).to.not.throw();
    });
    it("Should ignore invalid substitution arguments", function () {
        expect(() => console.log("%y %s %k", "I am a string")).to.not.throw();
        expect(() => console.log("%y%s%k", "I am a string")).to.not.throw();
        expect(() => console.log("%", 756)).to.not.throw();
        expect(() => console.log("%%", 756)).to.not.throw();
    });
    it("Should allow logging NaN", function () {
        expect(() => console.log("%d %f %s", NaN, NaN, NaN)).to.not.throw();
        expect(() => console.log("%d %f %s", 0 / 0, 0 / 0, 0 / 0)).to.not.throw();
    });
});
