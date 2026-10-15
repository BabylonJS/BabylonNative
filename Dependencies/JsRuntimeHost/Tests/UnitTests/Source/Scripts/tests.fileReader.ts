import { expect } from "chai";

describe("FileReader", function () {
    // -------------------------------- State constants --------------------------------
    it("exposes EMPTY / LOADING / DONE as static constants", function () {
        expect(FileReader.EMPTY).to.equal(0);
        expect(FileReader.LOADING).to.equal(1);
        expect(FileReader.DONE).to.equal(2);
    });

    it("exposes EMPTY / LOADING / DONE on instances", function () {
        const reader = new FileReader();
        expect(reader.EMPTY).to.equal(0);
        expect(reader.LOADING).to.equal(1);
        expect(reader.DONE).to.equal(2);
    });

    it("does not pollute Object.prototype with EMPTY/LOADING/DONE", function () {
        // Regression: in earlier drafts the JSC napi shim's
        // func.Get("prototype") returns Object.prototype, so writing
        // EMPTY/LOADING/DONE through it pollutes every plain object's
        // for..in iteration and breaks consumers like Babylon.js's
        // CameraInputsManager.attachElement.
        const plain: any = {};
        const keys: string[] = [];
        for (const k in plain) keys.push(k);
        expect(keys).to.have.lengthOf(0);

        // And the keys must not be present as inherited enumerable
        // properties on a fresh object either.
        expect("EMPTY" in plain && !Object.prototype.hasOwnProperty.call(plain, "EMPTY"))
            .to.equal(false);
    });

    // -------------------------------- Initial state --------------------------------
    it("initializes with EMPTY readyState and null result/error", function () {
        const reader = new FileReader();
        expect(reader.readyState).to.equal(FileReader.EMPTY);
        expect(reader.result).to.equal(null);
        expect(reader.error).to.equal(null);
    });

    it("provides null on* event handler slots by default", function () {
        const reader = new FileReader();
        expect(reader.onloadstart).to.equal(null);
        expect(reader.onprogress).to.equal(null);
        expect(reader.onload).to.equal(null);
        expect(reader.onabort).to.equal(null);
        expect(reader.onerror).to.equal(null);
        expect(reader.onloadend).to.equal(null);
    });

    // -------------------------------- readAsText --------------------------------
    it("reads a Blob as text via onload", function (done) {
        const reader = new FileReader();
        const blob = new Blob(["Hello"]);
        reader.onload = function () {
            try {
                expect(reader.readyState).to.equal(FileReader.DONE);
                expect(reader.result).to.equal("Hello");
                done();
            } catch (e) {
                done(e);
            }
        };
        reader.readAsText(blob);
    });

    it("reads a File as text via onload", function (done) {
        const reader = new FileReader();
        const file = new File(["World"], "world.txt");
        reader.onload = function () {
            try {
                expect(reader.result).to.equal("World");
                done();
            } catch (e) {
                done(e);
            }
        };
        reader.readAsText(file);
    });

    it("fires onloadend after onload", function (done) {
        const reader = new FileReader();
        const blob = new Blob(["abc"]);
        let loadFired = false;
        reader.onload = function () {
            loadFired = true;
        };
        reader.onloadend = function () {
            try {
                expect(loadFired).to.equal(true);
                expect(reader.readyState).to.equal(FileReader.DONE);
                done();
            } catch (e) {
                done(e);
            }
        };
        reader.readAsText(blob);
    });

    // -------------------------------- readAsArrayBuffer --------------------------------
    it("reads a Blob as an ArrayBuffer via onload", function (done) {
        const reader = new FileReader();
        const blob = new Blob([new Uint8Array([1, 2, 3])]);
        reader.onload = function () {
            try {
                expect(reader.result).to.be.instanceOf(ArrayBuffer);
                expect(reader.result.byteLength).to.equal(3);
                const view = new Uint8Array(reader.result);
                expect(view[0]).to.equal(1);
                expect(view[2]).to.equal(3);
                done();
            } catch (e) {
                done(e);
            }
        };
        reader.readAsArrayBuffer(blob);
    });

    // -------------------------------- readAsDataURL --------------------------------
    it("reads a Blob as a base64 data URL", function (done) {
        const reader = new FileReader();
        // "Hello" -> base64 SGVsbG8=
        const blob = new Blob(["Hello"], { type: "text/plain" });
        reader.onload = function () {
            try {
                expect(reader.result).to.be.a("string");
                expect(reader.result).to.equal("data:text/plain;base64,SGVsbG8=");
                done();
            } catch (e) {
                done(e);
            }
        };
        reader.readAsDataURL(blob);
    });

    it("falls back to application/octet-stream when the source blob has no type", function (done) {
        const reader = new FileReader();
        const blob = new Blob(["Hello"]);
        reader.onload = function () {
            try {
                expect(reader.result).to.equal("data:application/octet-stream;base64,SGVsbG8=");
                done();
            } catch (e) {
                done(e);
            }
        };
        reader.readAsDataURL(blob);
    });

    // -------------------------------- addEventListener --------------------------------
    it("dispatches 'load' events to addEventListener listeners", function (done) {
        const reader = new FileReader();
        const blob = new Blob(["abc"]);
        let countA = 0;
        let countB = 0;
        const handlerA = function () {
            countA++;
        };
        const handlerB = function () {
            countB++;
        };
        reader.addEventListener("load", handlerA);
        reader.addEventListener("load", handlerB);
        // Per WHATWG, adding the same listener twice is a no-op, so handlerA
        // should still fire exactly once.
        reader.addEventListener("load", handlerA);
        reader.onloadend = function () {
            try {
                expect(countA).to.equal(1);
                expect(countB).to.equal(1);
                done();
            } catch (e) {
                done(e);
            }
        };
        reader.readAsText(blob);
    });

    it("does not call a listener after removeEventListener", function (done) {
        const reader = new FileReader();
        const blob = new Blob(["abc"]);
        let called = false;
        const handler = function () {
            called = true;
        };
        reader.addEventListener("load", handler);
        reader.removeEventListener("load", handler);
        reader.onloadend = function () {
            try {
                expect(called).to.equal(false);
                done();
            } catch (e) {
                done(e);
            }
        };
        reader.readAsText(blob);
    });

    // -------------------------------- abort --------------------------------
    it("transitions readyState to DONE after abort()", function (done) {
        const reader = new FileReader();
        const blob = new Blob(["abc"]);
        reader.readAsText(blob);
        // Immediately abort before the queued read completes.
        reader.abort();
        // Wait one microtask turn so any pending dispatch settles before we inspect state.
        Promise.resolve().then(() => {
            try {
                expect(reader.readyState).to.equal(FileReader.DONE);
                done();
            } catch (e) {
                done(e);
            }
        });
    });
});
