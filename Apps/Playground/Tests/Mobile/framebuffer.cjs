const { PNG } = require("pngjs");

function decodeFrame(image) {
    if (!image || typeof image.png !== "string" ||
            !Number.isSafeInteger(image.pngBytes) || image.pngBytes < 1 ||
            !Number.isSafeInteger(image.width) || !Number.isSafeInteger(image.height) ||
            image.width < 1 || image.height < 1 || image.width * image.height > 1024 * 1024) {
        throw new Error("Invalid native framebuffer metadata");
    }
    const bytes = Buffer.from(image.png, "base64");
    if (bytes.length !== image.pngBytes || bytes.length < 33 ||
            bytes.readUInt32BE(16) !== image.width || bytes.readUInt32BE(20) !== image.height) {
        throw new Error("Native PNG dimensions or payload length differ from metadata");
    }
    const decoded = PNG.sync.read(bytes, { checkCRC: true });
    if (decoded.width !== image.width || decoded.height !== image.height) {
        throw new Error("Decoded native PNG dimensions differ from metadata");
    }
    return bytes;
}

module.exports = { decodeFrame };
