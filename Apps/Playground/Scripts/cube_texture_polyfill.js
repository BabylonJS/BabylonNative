// Cube texture fallback for BN's NativeEngine.
//
// BN's NativeEngine.createCubeTexture only natively handles .env single-file
// cubemaps and 6-file face arrays. Snippets that load .dds (or .ktx) cubemaps
// without an explicit forcedExtension or 6-file array throw "Cannot load
// cubemap because 6 files were not defined".
//
// Babylon's standard environment cubemaps are hosted both as <name>.dds and
// <name>.env at the same path on assets.babylonjs.com and
// playground.babylonjs.com. This polyfill detects the failure pre-condition
// and transparently retries with the .env URL. If the .env counterpart 404s,
// it falls through to the original (which throws), preserving existing
// behavior.

(function () {
    "use strict";

    if (typeof BABYLON === "undefined") {
        return;
    }
    if (!BABYLON.NativeEngine || !BABYLON.NativeEngine.prototype) {
        return;
    }

    var proto = BABYLON.NativeEngine.prototype;
    if (proto.__cubeTexturePolyfillInstalled) {
        return;
    }
    proto.__cubeTexturePolyfillInstalled = true;

    var original = proto.createCubeTexture;
    if (typeof original !== "function") {
        return;
    }

    var FALLBACK_EXTS = [".dds", ".ktx", ".ktx2"];

    function getExtension(url, forced) {
        if (forced) {
            return forced.toLowerCase();
        }
        var dot = url.lastIndexOf(".");
        if (dot < 0) {
            return "";
        }
        var ext = url.substring(dot).toLowerCase();
        var q = ext.indexOf("?");
        if (q >= 0) {
            ext = ext.substring(0, q);
        }
        return ext;
    }

    function replaceExt(url, oldExt) {
        return url.substring(0, url.length - oldExt.length) + ".env";
    }

    proto.createCubeTexture = function (rootUrl, scene, files, noMipmap, onLoad, onError, format, forcedExtension, createPolynomials, lodScale, lodOffset, fallback, loaderOptions, useSRGBBuffer, buffer) {
        var ext = getExtension(rootUrl, forcedExtension);
        var hasFiles = files && files.length === 6;
        var canFallback = !buffer && !forcedExtension && !hasFiles && FALLBACK_EXTS.indexOf(ext) >= 0;

        if (!canFallback) {
            return original.apply(this, arguments);
        }

        var self = this;
        var envUrl = replaceExt(rootUrl, ext);
        var texture = fallback || new BABYLON.InternalTexture(self, 7 /* Cube */);
        texture.isCube = true;
        texture.url = rootUrl;

        var settled = false;
        var settle = function (action) {
            if (settled) {
                return;
            }
            settled = true;
            try {
                action();
            } catch (e) {
                if (onError) {
                    onError(e && e.message ? e.message : String(e), e);
                }
            }
        };

        var onEnvLoaded = function (data) {
            settle(function () {
                var buf = (data && data.byteLength !== undefined && !(data instanceof Uint8Array)) ? new Uint8Array(data, 0, data.byteLength) : data;
                original.call(self, envUrl, scene, files, noMipmap, onLoad, onError, format, ".env", createPolynomials, lodScale || 0, lodOffset || 0, texture, loaderOptions, useSRGBBuffer || false, buf);
            });
        };

        var onEnvFailed = function (request, exception) {
            settle(function () {
                original.call(self, rootUrl, scene, files, noMipmap, onLoad, onError, format, forcedExtension, createPolynomials, lodScale, lodOffset, texture, loaderOptions, useSRGBBuffer, buffer);
            });
        };

        try {
            self._loadFile(envUrl, onEnvLoaded, undefined, undefined, true, onEnvFailed);
        } catch (e) {
            onEnvFailed(null, e);
        }

        return texture;
    };

    if (typeof console !== "undefined" && console.log) {
        console.log("[cube_texture_polyfill] NativeEngine.createCubeTexture patched with .env fallback");
    }
})();
