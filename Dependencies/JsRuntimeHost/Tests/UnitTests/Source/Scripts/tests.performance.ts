import { expect } from "chai";

describe("Performance", function () {
    this.timeout(1000);

    it("should have performance global defined", function () {
        expect(performance).to.not.be.undefined;
    });

    it("should return a number from performance.now()", function () {
        const now = performance.now();
        expect(now).to.be.a("number");
    });

    it("should return a non-negative value from performance.now()", function () {
        const now = performance.now();
        expect(now).to.be.at.least(0);
    });

    it("should return increasing values from performance.now()", function (done) {
        const first = performance.now();
        setTimeout(() => {
            const second = performance.now();
            expect(second).to.be.greaterThan(first);
            done();
        }, 10);
    });

    it("should measure elapsed time accurately", function (done) {
        const start = performance.now();
        const delay = 50;
        setTimeout(() => {
            const elapsed = performance.now() - start;
            // setTimeout guarantees a minimum delay, not a maximum.
            // Only check the lower bound to avoid flakes on busy CI agents.
            expect(elapsed).to.be.at.least(delay - 5);
            done();
        }, delay);
    });

    it("should return sub-millisecond precision", function () {
        // Call performance.now() multiple times rapidly and check if we get fractional values
        const samples: number[] = [];
        for (let i = 0; i < 100; i++) {
            samples.push(performance.now());
        }
        // At least some samples should have fractional parts (sub-millisecond precision)
        const hasFractional = samples.some(s => s % 1 !== 0);
        expect(hasFractional).to.equal(true);
    });
});
