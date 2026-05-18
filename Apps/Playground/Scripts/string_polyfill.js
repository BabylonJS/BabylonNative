// ES2019 String.prototype.trimStart / trimEnd polyfill for Chakra.
//
// Chakra predates ES2019 and only implements the older trimLeft / trimRight
// names. Babylon.js's ShaderProcessor and NodeMaterial code paths use the
// newer trimEnd name, so on Chakra-based BN backends those paths fail with
// `TypeError: Object doesn't support property or method 'trimEnd'`.
//
// This shim aliases trimStart -> trimLeft and trimEnd -> trimRight when the
// new names are missing. Both old and new names are now standard aliases in
// the spec, so engines that already provide trimEnd are not touched.
(function () {
    var sp = String.prototype;
    if (typeof sp.trimEnd !== "function" && typeof sp.trimRight === "function") {
        Object.defineProperty(sp, "trimEnd", {
            value: sp.trimRight,
            writable: true,
            configurable: true
        });
    }
    if (typeof sp.trimStart !== "function" && typeof sp.trimLeft === "function") {
        Object.defineProperty(sp, "trimStart", {
            value: sp.trimLeft,
            writable: true,
            configurable: true
        });
    }
})();
