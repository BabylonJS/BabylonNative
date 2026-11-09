(function () {
    "use strict";
    var endpoint = globalThis.__mobilePlaywrightUrl;
    if (!/^http:\/\/(?:bs-local\.com|127\.0\.0\.1):[0-9]+\/device\/[a-f0-9-]{36}$/.test(endpoint)) {
        throw new Error("Invalid mobile test controller endpoint");
    }
    var stopped = false;
    var consoleMessages = [];
    var originalConsole = console.error;
    var crcTable = new Uint32Array(256);
    for (var i = 0; i < 256; ++i) {
        var crc = i;
        for (var bit = 0; bit < 8; ++bit) {
            crc = (crc >>> 1) ^ ((crc & 1) ? 0xedb88320 : 0);
        }
        crcTable[i] = crc;
    }
    function base64(bytes) {
        var alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        var parts = [];
        for (var index = 0; index < bytes.length; index += 3) {
            var value = (bytes[index] << 16) | ((bytes[index + 1] || 0) << 8) | (bytes[index + 2] || 0);
            parts.push(alphabet[(value >>> 18) & 63] + alphabet[(value >>> 12) & 63] +
                (index + 1 < bytes.length ? alphabet[(value >>> 6) & 63] : "=") +
                (index + 2 < bytes.length ? alphabet[value & 63] : "="));
        }
        return parts.join("");
    }
    function encodePNG(data, width, height) {
        if (!Number.isFinite(width) || !Number.isFinite(height)) {
            throw new Error("Invalid mobile RGBA framebuffer dimensions or length");
        }
        // Match native writePNG's integer conversion for hardware-scaled scenes.
        width = Math.floor(width);
        height = Math.floor(height);
        if (width < 1 || height < 1 || width * height > 1024 * 1024 ||
                data.length !== width * height * 4) {
            throw new Error("Invalid mobile RGBA framebuffer dimensions or length");
        }
        if (typeof fflate === "undefined") {
            throw new Error("Mobile PNG compression was not installed by the controller");
        }
        var stride = width * 4;
        var rows = new Uint8Array((stride + 1) * height);
        for (var row = 0; row < height; ++row) {
            rows.set(data.subarray(row * stride, (row + 1) * stride), row * (stride + 1) + 1);
        }
        var compressed = fflate.zlibSync(rows, { level: 1 });
        var png = new Uint8Array(57 + compressed.length);
        png.set([137, 80, 78, 71, 13, 10, 26, 10]);
        var view = new DataView(png.buffer);
        var offset = 8;
        function chunk(type, bytes) {
            view.setUint32(offset, bytes.length);
            for (var n = 0; n < 4; ++n) {
                png[offset + 4 + n] = type.charCodeAt(n);
            }
            png.set(bytes, offset + 8);
            var checksum = 0xffffffff;
            for (var n = offset + 4; n < offset + 8 + bytes.length; ++n) {
                checksum = crcTable[(checksum ^ png[n]) & 255] ^ (checksum >>> 8);
            }
            view.setUint32(offset + 8 + bytes.length, (checksum ^ 0xffffffff) >>> 0);
            offset += bytes.length + 12;
        }
        var header = new Uint8Array(13);
        var headerView = new DataView(header.buffer);
        headerView.setUint32(0, width);
        headerView.setUint32(4, height);
        header[8] = 8;
        header[9] = 6;
        chunk("IHDR", header);
        chunk("IDAT", compressed);
        chunk("IEND", new Uint8Array(0));
        return { width: width, height: height, png: base64(png), pngBytes: png.length };
    }
    async function post(message) {
        var response = await fetch(endpoint + "/message", {
            method: "POST", headers: [["Content-Type", "application/json"]],
            body: JSON.stringify(message)
        });
        if (!response.ok) {
            throw new Error("Mobile controller POST failed: " + response.status);
        }
    }
    ["log", "warn", "error"].forEach(function (level) {
        var original = console[level];
        console[level] = function () {
            original.apply(console, arguments);
            consoleMessages.push({
                level: level, text: Array.prototype.map.call(arguments, String).join(" ")
            });
            if (consoleMessages.length > 50) {
                consoleMessages.shift();
            }
        };
    });
    globalThis.__mobilePlaywright = {
        encodePNG: encodePNG,
        viewport: function (requested) {
            var probe = new _native.Engine();
            try {
                var width = probe.getRenderWidth();
                var height = probe.getRenderHeight();
                if (width !== requested.width || height !== requested.height) {
                    throw new Error("Mobile framebuffer is " + width + "x" + height +
                        ", expected " + requested.width + "x" + requested.height);
                }
            } finally {
                probe.dispose();
            }
        },
        screenshot: function () {
            var capture = globalThis.__babylonNativeScreenshot;
            if (!capture) {
                throw new Error("The shared Playwright render step did not capture a native framebuffer");
            }
            var image = encodePNG(capture.data, capture.width, capture.height);
            delete globalThis.__babylonNativeScreenshot;
            return image;
        }
    };
    (async function () {
        try {
            await post({ type: "ready", protocol: 2, babylonVersion: BABYLON.Engine.Version,
                graphicsApi: TestUtils.getGraphicsApiName() });
            while (!stopped) {
                var response = await fetch(endpoint + "/next");
                if (response.status === 204) {
                    continue;
                }
                if (!response.ok) {
                    throw new Error("Mobile controller poll failed: " + response.status);
                }
                var command = JSON.parse(await response.text());
                if (!Number.isSafeInteger(command.id) || typeof command.source !== "string") {
                    throw new Error("Malformed mobile controller command");
                }
                consoleMessages.length = 0;
                var started = performance.now();
                try {
                    var result = await (0, eval)(command.source);
                    await post({ type: "result", id: command.id, value: result,
                        console: consoleMessages.slice(),
                        deviceMs: performance.now() - started });
                } catch (error) {
                    await post({ type: "result", id: command.id, error: String(error),
                        console: consoleMessages.slice(),
                        stack: error.stack || "", deviceMs: performance.now() - started });
                }
            }
        } catch (error) {
            stopped = true;
            originalConsole("Mobile test transport stopped: " + String(error) + "\n" + (error.stack || ""));
        }
    })();
})();
