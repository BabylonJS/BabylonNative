import { expect } from "chai";

describe("AbortController", function () {
    it("should not throw while aborting with no callbacks", function () {
        const controller = new AbortController();
        expect(controller.signal.aborted).to.equal(false);

        // Trigger with no callbacks
        controller.abort();

        expect(controller.signal.aborted).to.equal(true);
    });

    it("should not throw while aborting and correctly trigger both callbacks", function (done) {
        const controller = new AbortController();
        expect(controller.signal.aborted).to.equal(false);

        let cb1 = false, cb2 = false;

        // Expect aborted to be true after abort()
        controller.signal.onabort = () => {
            expect(controller.signal.aborted).to.equal(true);
            cb1 = true;

            if (cb1 && cb2) {
                done();
            }
        }

        controller.signal.addEventListener("abort", () => {
            expect(controller.signal.aborted).to.equal(true);
            cb2 = true;

            if (cb1 && cb2) {
                done();
            }
        })

        controller.abort();
    });

    it("should remove listener and not invoke callback function", function () {
        const controller = new AbortController();
        expect(controller.signal.aborted).to.equal(false);

        // If this function is unsuccessfully removed it will assert when called
        const onAbort = () => {
            expect(controller.signal.aborted).to.equal(false);
        };

        controller.signal.addEventListener("abort", onAbort);
        controller.signal.removeEventListener("abort", onAbort);

        controller.abort();

        expect(controller.signal.aborted).to.equal(true);
    });

    it("AbortSignal.abort() returns a signal already aborted with an AbortError reason", function () {
        const signal = (AbortSignal as any).abort();
        expect(signal.aborted).to.equal(true);
        expect(signal.reason).to.be.an.instanceof(Error);
        expect(signal.reason.name).to.equal("AbortError");
    });

    it("throwIfAborted() throws the reason only once aborted", function () {
        const controller = new AbortController();
        // Not aborted yet: must not throw.
        (controller.signal as any).throwIfAborted();

        controller.abort();
        expect(() => (controller.signal as any).throwIfAborted()).to.throw();
    });

    it("abort(reason) records the provided reason", function () {
        const controller = new AbortController();
        const reason = new Error("custom reason");
        controller.abort(reason);
        expect((controller.signal as any).reason).to.equal(reason);
    });

    it("abort() with no reason defaults to an AbortError", function () {
        const controller = new AbortController();
        controller.abort();
        const reason = (controller.signal as any).reason;
        expect(reason).to.be.an.instanceof(Error);
        expect(reason.name).to.equal("AbortError");
    });
});
