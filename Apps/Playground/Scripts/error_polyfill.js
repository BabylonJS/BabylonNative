// ES2022 Error(message, options) polyfill for Chakra runtimes.
//
// Chakra's built-in Error constructor predates ES2022 and treats its second
// argument as part of the message: `new Error("hello", { cause: 42 })`
// produces an Error whose `message` is `"[object Object]"` and whose `cause`
// is undefined. That swallows every Babylon.js shader-compile error message
// that uses the modern `throw new Error("SHADER ERROR\n" + msg, { cause: e })`
// pattern, leaving only `Error: [object Object]` in stdout, hiding every
// glslang / SPIRV-Cross / DXC diagnostic.
//
// This file replaces the global Error constructor (and its standard subclasses)
// with a thin wrapper that forwards `message` correctly and attaches `cause`
// from the options bag. The patch is a no-op on engines that already implement
// the ES2022 signature (V8, modern JavaScriptCore), so it is safe to load on
// every backend.
(function () {
    var globalObj =
        (typeof globalThis !== "undefined") ? globalThis :
        (typeof self !== "undefined")       ? self :
        (typeof global !== "undefined")     ? global :
        (new Function("return this;"))();

    // If the host already implements ES2022 Error options, do nothing.
    try {
        var probe = new Error("probe", { cause: 42 });
        if (probe.message === "probe" && probe.cause === 42) {
            return;
        }
    } catch (_) {
        // fall through to patching
    }

    function patchCtor(name) {
        var Orig = globalObj[name];
        if (typeof Orig !== "function") {
            return;
        }

        var hasReflectConstruct =
            (typeof Reflect !== "undefined") &&
            (typeof Reflect.construct === "function");

        function Patched(message, options) {
            var m;
            if (typeof message === "string") {
                m = message;
            } else if (typeof message === "undefined") {
                m = undefined;
            } else {
                m = String(message);
            }

            var args = (m === undefined) ? [] : [m];
            // new.target preservation: when invoked via `super(...)` from a
            // subclass we want the new instance's prototype chain to point at
            // the subclass, not at Orig. Reflect.construct does that for us.
            var newTarget = (typeof new.target !== "undefined") ? new.target : Patched;
            var err = hasReflectConstruct
                ? Reflect.construct(Orig, args, newTarget)
                : (m === undefined ? new Orig() : new Orig(m));

            if (options !== null && typeof options === "object" && "cause" in options) {
                Object.defineProperty(err, "cause", {
                    value: options.cause,
                    writable: true,
                    configurable: true,
                    enumerable: false
                });
            }
            return err;
        }

        Patched.prototype = Orig.prototype;
        if (typeof Orig.captureStackTrace === "function") {
            Patched.captureStackTrace = Orig.captureStackTrace;
        }
        // Preserve the original name so stack traces still show "Error: ...".
        try {
            Object.defineProperty(Patched, "name", { value: name, configurable: true });
        } catch (_) { /* non-writable on some engines, harmless */ }

        globalObj[name] = Patched;
    }

    var ctorNames = [
        "Error",
        "TypeError",
        "RangeError",
        "SyntaxError",
        "ReferenceError",
        "URIError",
        "EvalError"
    ];
    for (var i = 0; i < ctorNames.length; i++) {
        patchCtor(ctorNames[i]);
    }
})();
