import { expect } from "chai";

// URLSearchParams
describe("URLSearchParams", function () {

    // -------------------------------- URLSearchParams Get --------------------------------

    it("should retrieve null from empty searchParams", function () {
        // Get Empty
        const params = new URLSearchParams("");

        expect(params.get("foo")).to.equal(null);
    });

    it("should retrieve value from searchParams", function () {
        // Get Value
        const params = new URLSearchParams("?foo=1");

        expect(params.get("foo")).to.equal("1");
    });

    // -------------------------------- URLSearchParams Set --------------------------------

    const paramsSet = new URLSearchParams("");

    it("should throw exception when trying to set with less than 2 parameters", function () {
        // `set` expects parameters, none given.
        // @ts-expect-error
        expect(() => paramsSet.set()).to.throw();
    });

    it("should preserve the type and message of an error thrown from native code", function () {
        // A native throw must reach JS unchanged. When napi_throw reported failure, the
        // error was replaced by an InternalError reading "Uncaught C++ exception: ...",
        // built by stringifying an error whose handle scope had already closed.
        let caught: any;
        try {
            // @ts-expect-error
            paramsSet.set();
        } catch (e) {
            caught = e;
        }
        expect(caught).to.be.an.instanceOf(Error);
        expect(caught.name).to.equal("Error");
        // Not an equality check: the JSI backend prefixes "Exception in HostFunction: ".
        expect(caught.message).to.contain(
            "Failed to execute 'set' on 'URLSearchParams': 2 arguments required, but only 0 present"
        );
        expect(caught.message).to.not.contain("Uncaught C++ exception");
    });

    it("should add a number and retrieve it as a string from searchParams", function () {
        // Set Number
        paramsSet.set("foo", 400 as any);
        expect(paramsSet.get("foo")).to.equal("400");
    });

    it("should add a string and retrieve it as a string from searchParams", function () {
        // Set String
        paramsSet.set("bar", "50");
        expect(paramsSet.get("bar")).to.equal("50");
    });

    it("should add a boolean and retrieve it as a string from searchParams", function () {
        // Set Boolean
        paramsSet.set("baz", true as any);
        expect(paramsSet.get("baz")).to.equal("true");
    });

    it("should set an existing number and retrieve it as a string from searchParams", function () {
        // Set Existing Value
        paramsSet.set("foo", 9999 as any);
        expect(paramsSet.get("foo")).to.equal("9999");
    });

    // -------------------------------- URLSearchParams Has --------------------------------

    it("should check value is in searchParams (True)", function () {
        // Check existing value
        const paramsHas = new URLSearchParams("?foo=1");
        expect(paramsHas.has("foo")).to.equal(true);
    });

    it("should check value is in searchParams (False)", function () {
        // Check non-existing value
        const paramsHas = new URLSearchParams("?foo=1");
        expect(paramsHas.has("Microsoft")).to.equal(false);
    });

    it("should check empty searchParams for value (False)", function () {
        // Check empty params
        const paramsEmpty = new URLSearchParams("");
        expect(paramsEmpty.has("foo")).to.equal(false);
    });

    // -------------------------------- URLSearchParams Construction --------------------------------

    it("should retrieve search params set at construction", function () {
        // Retrieve params via url.search, passed into ctor
        const url = new URL("https://example.com?foo=1&bar=2");
        const params1 = new URLSearchParams(url.search);
        expect(params1.get("foo")).to.equal("1");
        expect(params1.get("bar")).to.equal("2");
    });

    it("should retrieve search params from url.searchParams object", function () {
        // Get the URLSearchParams object directly from an URL object
        const url = new URL("https://example.com?foo=1&bar=2");
        const params1a = url.searchParams;
        expect(params1a.get("foo")).to.equal("1");
        expect(params1a.get("bar")).to.equal("2");
    });

    it("should retrieve search params string constructed URLSearchParams", function () {
        // Pass in a string literal
        const params2 = new URLSearchParams("foo=1&bar=2");
        expect(params2.get("foo")).to.equal("1");
        expect(params2.get("bar")).to.equal("2");
    });
});
