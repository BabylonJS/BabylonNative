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

describe("napi_get_property_names", function () {
    // Chakra does not define globalThis; a non-strict function returns the global object.
    const globalObject = Function("return this")();

    function forIn(object: any): string[] {
        const keys: string[] = [];
        for (const key in object) {
            keys.push(key);
        }
        return keys;
    }

    // Some engines' own `for...in` does not implement the shadowing rule that
    // the tests below rely on -- Hermes reports an inherited property that a
    // non-enumerable own property is supposed to hide. Probe for that rather
    // than name engines, and only use `for...in` as an oracle where it holds.
    // https://github.com/BabylonJS/JsRuntimeHost/issues/219 tracks the gap.
    const shadowingProbe = Object.create({ probe: 1 });
    Object.defineProperty(shadowingProbe, "probe", { value: 2, enumerable: false });
    const forInHonoursShadowing = forIn(shadowingProbe).length === 0;

    it("returns own enumerable string keys", function () {
        expect(napiGetPropertyNames({ a: 1, b: 2 })).to.deep.equal(["a", "b"]);
    });

    it("includes enumerable properties inherited from the prototype chain", function () {
        const object = Object.create({ inherited: 1 });
        object.own = 2;
        expect(napiGetPropertyNames(object)).to.deep.equal(["own", "inherited"]);
    });

    it("excludes non-enumerable own properties", function () {
        const object = { visible: 1 };
        Object.defineProperty(object, "hidden", { value: 2, enumerable: false });
        expect(napiGetPropertyNames(object)).to.deep.equal(["visible"]);
    });

    it("excludes symbol keys", function () {
        const object: any = { a: 1 };
        object[Symbol("s")] = 2;
        expect(napiGetPropertyNames(object)).to.deep.equal(["a"]);
    });

    it("reports a shadowed inherited property only once", function () {
        const object = Object.create({ shared: 1 });
        object.shared = 2;
        expect(napiGetPropertyNames(object)).to.deep.equal(["shared"]);
    });

    it("distinguishes property names containing different lone surrogates", function () {
        const object = Object.create({ ["\udc00"]: 1 });
        object["\ud800"] = 2;
        expect(napiGetPropertyNames(object)).to.deep.equal(["\ud800", "\udc00"]);
    });

    it("takes one ownKeys snapshot per prototype level", function () {
        let ownKeysCalls = 0;
        const object = new Proxy({ own: 1 }, {
            ownKeys(target) {
                ++ownKeysCalls;
                return Reflect.ownKeys(target);
            },
        });
        expect(napiGetPropertyNames(object)).to.deep.equal(["own"]);
        expect(ownKeysCalls).to.equal(1);
    });

    it("does not depend on mutable Object globals", function () {
        const objectConstructor = Object;
        const ownNames = Object.getOwnPropertyNames;
        const ownDescriptor = Object.getOwnPropertyDescriptor;
        const prototype = Object.getPrototypeOf;
        const object = Object.create({ inherited: 1 });
        object.own = 2;
        let names: string[] | undefined;

        try {
            Object.getOwnPropertyNames = () => ["forged"];
            Object.getOwnPropertyDescriptor = () => ({ enumerable: false });
            Object.getPrototypeOf = () => null;
            Reflect.set(globalObject, "Object", {});
            names = napiGetPropertyNames(object);
        } finally {
            Reflect.set(globalObject, "Object", objectConstructor);
            Object.getOwnPropertyNames = ownNames;
            Object.getOwnPropertyDescriptor = ownDescriptor;
            Object.getPrototypeOf = prototype;
        }

        expect(names).to.deep.equal(["own", "inherited"]);
    });

    it("deduplicates large sets of enumerable and non-enumerable names", function () {
        const object = Object.create({ inherited: 1 });
        const expected: string[] = [];
        for (let index = 0; index < 1024; ++index) {
            const name = `key${index}`;
            const enumerable = index % 2 === 0;
            Object.defineProperty(object, name, { value: index, enumerable });
            if (enumerable) {
                expected.push(name);
            }
        }
        expected.push("inherited");
        expect(napiGetPropertyNames(object)).to.deep.equal(expected);
    });

    it("omits an inherited property shadowed by a non-enumerable own property", function () {
        const object = Object.create({ shared: 1 });
        Object.defineProperty(object, "shared", { value: 2, enumerable: false });
        expect(napiGetPropertyNames(object)).to.deep.equal([]);
    });

    it("excludes class methods, which are non-enumerable", function () {
        class Point {
            x: number;
            y: number;
            constructor() {
                this.x = 1;
                this.y = 2;
            }
            length(): number {
                return 0;
            }
        }
        expect(napiGetPropertyNames(new Point())).to.deep.equal(["x", "y"]);
    });

    it("reports array indices as strings and omits the non-enumerable length", function () {
        expect(napiGetPropertyNames(["a", "b"])).to.deep.equal(["0", "1"]);
    });

    it("matches for...in over a multi-level prototype chain", function () {
        const grandparent = { deep: 0 };
        const parent: any = Object.create(grandparent);
        parent.middle = 1;
        Object.defineProperty(parent, "hiddenMiddle", { value: 2, enumerable: false });

        const object: any = Object.create(parent);
        object.own = 3;
        object[Symbol("s")] = 4;
        Object.defineProperty(object, "deep", { value: 5, enumerable: false });

        expect(napiGetPropertyNames(object)).to.deep.equal(["own", "middle"]);
        if (forInHonoursShadowing) {
            expect(napiGetPropertyNames(object)).to.deep.equal(forIn(object));
        }
    });

    // `Napi::Object::GetPropertyNames` requires an object, so the coercion the
    // C entry point performs on its argument is only reachable through
    // `napiGetPropertyNamesRaw`. That global is undefined on the JSI backend,
    // which implements the `Napi::` C++ surface directly on JSI and has no C
    // Node-API to call.
    const describeCoercion = typeof napiGetPropertyNamesRaw === "function" ? describe : describe.skip;

    describeCoercion("argument coercion", function () {
        // Hermes' Node-API is not implemented in this repository -- it comes from
        // the Hermes dependency itself -- and it rejects primitives outright
        // rather than applying ToObject. Hermes is an experimental engine here,
        // so assert the specified behaviour everywhere it is ours to control and
        // skip the wrapping cases on Hermes rather than weakening them. The
        // upstream gap is tracked by
        // https://github.com/BabylonJS/JsRuntimeHost/issues/219.
        const describePrimitives = hostEngine === "Hermes" ? describe.skip : describe;

        if (hostEngine === "Hermes") {
            it("rejects primitives instead of applying ToObject (upstream gap)", function () {
                expect(() => napiGetPropertyNamesRaw("ab")).to.throw();
            });
        }

        describePrimitives("of a primitive", function () {
            it("wraps a string primitive and reports its indices", function () {
                expect(napiGetPropertyNamesRaw("ab")).to.deep.equal(["0", "1"]);
            });

            it("wraps a primitive after the global Object binding is replaced", function () {
                const objectConstructor = Object;
                let names: string[] | undefined;
                try {
                    Reflect.set(globalObject, "Object", {});
                    names = napiGetPropertyNamesRaw("ab");
                } finally {
                    Reflect.set(globalObject, "Object", objectConstructor);
                }
                expect(names).to.deep.equal(["0", "1"]);
            });

            it("wraps a number primitive, which has no enumerable properties", function () {
                expect(napiGetPropertyNamesRaw(42)).to.deep.equal([]);
            });

            it("wraps a boolean primitive, which has no enumerable properties", function () {
                expect(napiGetPropertyNamesRaw(true)).to.deep.equal([]);
            });
        });

        it("still reports the prototype chain of a real object", function () {
            const object = Object.create({ inherited: 1 });
            object.own = 2;
            expect(napiGetPropertyNamesRaw(object)).to.deep.equal(["own", "inherited"]);
        });

        it("fails for null", function () {
            expect(() => napiGetPropertyNamesRaw(null)).to.throw();
        });

        it("fails for undefined", function () {
            expect(() => napiGetPropertyNamesRaw(undefined)).to.throw();
        });
    });

    // A `getPrototypeOf` Proxy trap may put an object back onto its own
    // prototype chain -- no specification invariant forbids it -- which makes
    // the chain cyclic. V8 collects keys recursively and so terminates with a
    // `RangeError`; the shared walk is iterative and used to spin forever,
    // burning CPU in native code with no way for script or the test timeout to
    // interrupt it.
    //
    // These only apply to the backends that use the shared walk. V8 and Hermes
    // bring their own key collection, and the JSI adapter forwards to
    // `jsi::Object::getPropertyNames`, so their behaviour here is not ours to
    // specify.
    const usesSharedWalk = hostEngine === "Chakra" || hostEngine === "QuickJS" || hostEngine === "JavaScriptCore";
    const describeCycles = usesSharedWalk ? describe : describe.skip;

    describeCycles("cyclic prototype chains", function () {
        this.timeout(5000);

        it("throws for a proxy that is its own prototype", function () {
            let object: any;
            object = new Proxy({ own: 1 }, { getPrototypeOf() { return object; } });
            expect(() => napiGetPropertyNames(object)).to.throw(RangeError);
        });

        it("throws for a two-object prototype cycle", function () {
            let first: any;
            let second: any;
            first = new Proxy({ a: 1 }, { getPrototypeOf() { return second; } });
            second = new Proxy({ b: 2 }, { getPrototypeOf() { return first; } });
            expect(() => napiGetPropertyNames(first)).to.throw(RangeError);
        });

        it("still reports a long acyclic chain in full", function () {
            const base = { deep: 1 };
            const middle = Object.create(base);
            middle.middle = 2;
            const leaf = Object.create(middle);
            leaf.own = 3;
            expect(napiGetPropertyNames(leaf)).to.deep.equal(["own", "middle", "deep"]);
        });

        it("preserves an exception from a throwing getPrototypeOf trap", function () {
            const object = new Proxy({ own: 1 }, {
                getPrototypeOf() { throw new Error("trap"); },
            });
            expect(() => napiGetPropertyNames(object)).to.throw("trap");
        });
    });
});
