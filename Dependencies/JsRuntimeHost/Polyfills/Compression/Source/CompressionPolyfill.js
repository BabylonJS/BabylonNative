(function() {
    "use strict";

    return function createCompressionPolyfill(NativeCompressionCodec) {
        var compressionStates = new WeakMap();
        var decompressionStates = new WeakMap();

        function normalizeFormat(value) {
            // String concatenation follows Web IDL's DOMString conversion,
            // including throwing for Symbol values and propagating user
            // conversion exceptions.
            var format = value + "";
            if (format !== "deflate" && format !== "deflate-raw" && format !== "gzip") {
                throw new TypeError("Unsupported compression format: '" + format + "'");
            }
            return format;
        }

        function toUint8Array(chunk) {
            if (chunk instanceof ArrayBuffer) {
                return new Uint8Array(chunk);
            }
            if (ArrayBuffer.isView(chunk) && chunk.buffer instanceof ArrayBuffer) {
                return new Uint8Array(chunk.buffer, chunk.byteOffset, chunk.byteLength);
            }
            throw new TypeError("Compression stream chunks must be BufferSource values");
        }

        function initialize(instance, states, format, compressing) {
            var codec = new NativeCompressionCodec(format, compressing);
            var enqueue;
            var transform = new TransformStream({
                start: function(controller) {
                    enqueue = controller.enqueue.bind(controller);
                },
                transform: function(chunk) {
                    var bytes = toUint8Array(chunk);
                    if (bytes.byteLength === 0) {
                        return;
                    }

                    try {
                        codec.transform(bytes, enqueue);
                    } catch (error) {
                        codec.close();
                        codec = null;
                        enqueue = null;
                        throw error;
                    }
                },
                flush: function() {
                    try {
                        codec.finish(enqueue);
                    } finally {
                        codec.close();
                        codec = null;
                        enqueue = null;
                    }
                }
            });

            states.set(instance, {
                readable: transform.readable,
                writable: transform.writable
            });
        }

        function getState(states, value) {
            var state = states.get(value);
            if (!state) {
                throw new TypeError("Illegal invocation");
            }
            return state;
        }

        function CompressionStream(format) {
            if (!(this instanceof CompressionStream)) {
                throw new TypeError("CompressionStream must be constructed with new");
            }
            if (arguments.length === 0) {
                throw new TypeError("CompressionStream requires a format");
            }
            initialize(this, compressionStates, normalizeFormat(format), true);
        }

        function DecompressionStream(format) {
            if (!(this instanceof DecompressionStream)) {
                throw new TypeError("DecompressionStream must be constructed with new");
            }
            if (arguments.length === 0) {
                throw new TypeError("DecompressionStream requires a format");
            }
            initialize(this, decompressionStates, normalizeFormat(format), false);
        }

        Object.defineProperties(CompressionStream.prototype, {
            readable: {
                configurable: true,
                enumerable: true,
                get: function() { return getState(compressionStates, this).readable; }
            },
            writable: {
                configurable: true,
                enumerable: true,
                get: function() { return getState(compressionStates, this).writable; }
            }
        });
        Object.defineProperties(DecompressionStream.prototype, {
            readable: {
                configurable: true,
                enumerable: true,
                get: function() { return getState(decompressionStates, this).readable; }
            },
            writable: {
                configurable: true,
                enumerable: true,
                get: function() { return getState(decompressionStates, this).writable; }
            }
        });

        if (typeof Symbol === "function" && Symbol.toStringTag) {
            Object.defineProperty(CompressionStream.prototype, Symbol.toStringTag, {
                configurable: true,
                value: "CompressionStream"
            });
            Object.defineProperty(DecompressionStream.prototype, Symbol.toStringTag, {
                configurable: true,
                value: "DecompressionStream"
            });
        }

        return {
            CompressionStream: CompressionStream,
            DecompressionStream: DecompressionStream
        };
    };
})()
