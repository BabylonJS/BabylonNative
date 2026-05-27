#include <Babylon/Polyfills/CubeTexture.h>

#include <napi/env.h>

namespace Babylon::Polyfills::CubeTexture
{
    namespace
    {
        // Embedded polyfill source. Kept verbatim from the original Playground
        // cube_texture_polyfill.js (PR #1710 v0) so behaviour stays identical:
        //   - Patches BABYLON.NativeEngine.prototype.createCubeTexture.
        //   - Triggers only when the URL ends in .dds / .ktx / .ktx2 and no
        //     forcedExtension, 6-file array, or raw buffer was supplied.
        //   - Retries via NativeEngine._loadFile to fetch the .env counterpart;
        //     on 404 falls back to the original (which throws), preserving the
        //     existing failure mode.
        //   - Idempotent via the __cubeTexturePolyfillInstalled marker on the
        //     prototype.
        constexpr const char* JS_SOURCE = R"javascript(
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

        var onEnvFailed = function (/* request, exception */) {
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
        console.log("[CubeTexture polyfill] NativeEngine.createCubeTexture patched with .env fallback");
    }
})();
)javascript";

        constexpr const char* JS_SOURCE_URL = "babylon-native://polyfills/CubeTexture.js";
    }

    void Initialize(Napi::Env env)
    {
        // The free function Napi::Eval(env, source, url) is declared by every
        // engine-specific <napi/env.h> across both N-API trees (Chakra, V8,
        // JavaScriptCore in Core/Node-API/Include/Engine/<X>/, and JSI in
        // Core/Node-API-JSI/Include/napi/). Using it uniformly avoids the
        // Shared-only env.RunScript() which is missing from the JSI tree.
        Napi::HandleScope scope{env};
        Napi::Eval(env, JS_SOURCE, JS_SOURCE_URL);
    }
}
