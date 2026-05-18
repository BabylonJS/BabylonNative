// dom_polyfill.js
//
// Minimal browser/DOM globals for the Babylon Native Playground JS host.
// Provides TextEncoder and PointerEvent on runtimes (older Chakra) that do
// not ship them natively. AbortController is provided by a native polyfill.
// Each shim is self-detecting and no-ops if the symbol already exists.
//
// `document` is shimmed by validation_native.js once it has loaded the
// config, since the runner needs to inject test-specific behaviour into
// createElement.

(function () {
    'use strict';

    // Chakra has no `globalThis`; use the Function-constructor trick.
    var g = (new Function('return this'))();

    if (typeof g.TextEncoder === 'undefined') {
        function TextEncoder() {}
        Object.defineProperty(TextEncoder.prototype, 'encoding', {
            get: function () { return 'utf-8'; },
            configurable: true,
        });
        TextEncoder.prototype.encode = function (input) {
            var str = input === undefined ? '' : String(input);
            var bytes = [];
            for (var i = 0; i < str.length; i++) {
                var code = str.charCodeAt(i);
                if (code >= 0xD800 && code <= 0xDBFF && i + 1 < str.length) {
                    var c2 = str.charCodeAt(i + 1);
                    if (c2 >= 0xDC00 && c2 <= 0xDFFF) {
                        code = 0x10000 + (((code & 0x3FF) << 10) | (c2 & 0x3FF));
                        i++;
                    }
                }
                if (code < 0x80) {
                    bytes.push(code);
                } else if (code < 0x800) {
                    bytes.push(0xC0 | (code >> 6));
                    bytes.push(0x80 | (code & 0x3F));
                } else if (code < 0x10000) {
                    bytes.push(0xE0 | (code >> 12));
                    bytes.push(0x80 | ((code >> 6) & 0x3F));
                    bytes.push(0x80 | (code & 0x3F));
                } else {
                    bytes.push(0xF0 | (code >> 18));
                    bytes.push(0x80 | ((code >> 12) & 0x3F));
                    bytes.push(0x80 | ((code >> 6) & 0x3F));
                    bytes.push(0x80 | (code & 0x3F));
                }
            }
            return new Uint8Array(bytes);
        };
        TextEncoder.prototype.encodeInto = function (input, dest) {
            var arr = this.encode(input);
            var n = Math.min(arr.length, dest.length);
            for (var i = 0; i < n; i++) dest[i] = arr[i];
            return { read: String(input).length, written: n };
        };
        g.TextEncoder = TextEncoder;
    }

    if (typeof g.PointerEvent === 'undefined') {
        function PointerEvent(type, init) {
            init = init || {};
            this.type = String(type || '');
            this.bubbles = !!init.bubbles;
            this.cancelable = init.cancelable !== false;
            this.composed = !!init.composed;
            this.defaultPrevented = false;
            this.target = init.target || null;
            this.currentTarget = null;
            this.timeStamp = Date.now();
            this.pointerId = init.pointerId !== undefined ? init.pointerId : 0;
            this.width = init.width !== undefined ? init.width : 1;
            this.height = init.height !== undefined ? init.height : 1;
            this.pressure = init.pressure !== undefined ? init.pressure : 0.5;
            this.tangentialPressure = init.tangentialPressure || 0;
            this.tiltX = init.tiltX || 0;
            this.tiltY = init.tiltY || 0;
            this.twist = init.twist || 0;
            this.altitudeAngle = init.altitudeAngle || 0;
            this.azimuthAngle = init.azimuthAngle || 0;
            this.pointerType = init.pointerType || 'mouse';
            this.isPrimary = init.isPrimary !== false;
            this.clientX = init.clientX || 0;
            this.clientY = init.clientY || 0;
            this.offsetX = init.offsetX || 0;
            this.offsetY = init.offsetY || 0;
            this.pageX = init.pageX || 0;
            this.pageY = init.pageY || 0;
            this.screenX = init.screenX || 0;
            this.screenY = init.screenY || 0;
            this.movementX = init.movementX || 0;
            this.movementY = init.movementY || 0;
            this.button = init.button || 0;
            this.buttons = init.buttons || 0;
            this.relatedTarget = init.relatedTarget || null;
            this.ctrlKey = !!init.ctrlKey;
            this.shiftKey = !!init.shiftKey;
            this.altKey = !!init.altKey;
            this.metaKey = !!init.metaKey;
            this.detail = init.detail || 0;
            this.view = init.view || null;
        }
        PointerEvent.prototype.preventDefault = function () { this.defaultPrevented = true; };
        PointerEvent.prototype.stopPropagation = function () {};
        PointerEvent.prototype.stopImmediatePropagation = function () {};
        PointerEvent.prototype.getModifierState = function () { return false; };
        PointerEvent.prototype.getCoalescedEvents = function () { return []; };
        PointerEvent.prototype.getPredictedEvents = function () { return []; };
        g.PointerEvent = PointerEvent;
    }
})();
