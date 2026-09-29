// Browser globals needed before loading Babylon UMD bundles.

// Let UMD bundles merge into the global object without enabling DOM paths through window.
if (typeof globalThis.global === "undefined") {
    globalThis.global = globalThis;
}

// Provide the browser's default window.name for snippets.
if (typeof globalThis.name === "undefined") {
    globalThis.name = "";
}
