globalThis.window = globalThis;
globalThis.canvas = globalThis;
globalThis.OffscreenCanvas = function (width, height) {
    return {
        width,
        height,
        getContext: function () {
            return {
                fillRect: function () {},
                measureText: function () {
                    return { width: 0 };
                },
            };
        },
    };
};
globalThis.document = {
    createElement: function (type) {
        return type === "canvas" ? new OffscreenCanvas(64, 64) : {};
    },
    removeEventListener: function () {},
};

BABYLON.Tools.DefaultRetryStrategy = function (url, request, retryIndex) {
    if (retryIndex >= 5 || url.indexOf("file:") !== -1) {
        return -1;
    }
    return request.status === 0 || request.status === 429 || request.status >= 500
        ? Math.pow(2, retryIndex) * 500
        : -1;
};

_native.RootUrl = "https://playground.babylonjs.com";
globalThis.__babylonNativePlaywright = true;
