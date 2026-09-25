import { expect } from "chai";

describe("setTimeout", function () {
    this.timeout(5000);

    it("should return an id greater than zero", function () {
        const id = setTimeout(() => { }, 0);
        expect(id).to.be.greaterThan(0);
    });

    it("should return an id greater than zero when given an undefined function", function () {
        const id = setTimeout(undefined as any, 0);
        expect(id).to.be.greaterThan(0);
    });

    it("should call the given function after the given delay", function (done) {
        const startTime = new Date().getTime();
        setTimeout(() => {
            try {
                expect(new Date().getTime() - startTime).to.be.at.least(10);
                done();
            }
            catch (e) {
                done(e);
            }
        }, 10);
    });

    it("should call the given nested function after the given delay", function (done) {
        const startTime = new Date().getTime();
        setTimeout(() => {
            setTimeout(() => {
                try {
                    expect(new Date().getTime() - startTime).to.be.at.least(20);
                    done();
                }
                catch (e) {
                    done(e);
                }
            }, 10);
        }, 10);
    });

    it("should call the given function after the given delay when the delay is a string representing a valid number", function (done) {
        const startTime = new Date().getTime();
        setTimeout(() => {
            try {
                expect(new Date().getTime() - startTime).to.be.at.least(10);
                done();
            }
            catch (e) {
                done(e);
            }
        }, "10" as any);
    });

    it("should call the given function after zero milliseconds when the delay is a string representing an invalid number", function (done) {
        setTimeout(() => {
            done();
        }, "a" as any);
    });

    it("should call the given function after other tasks execute when the given delay is zero", function (done) {
        let trailingCodeExecuted = false;
        setTimeout(() => {
            try {
                expect(trailingCodeExecuted).to.be.true;
                done();
            }
            catch (e) {
                done(e);
            }
        }, 0);
        trailingCodeExecuted = true;
    });

    it("should call the given function after other tasks execute when the given delay is undefined", function (done) {
        let trailingCodeExecuted = false;
        setTimeout(() => {
            try {
                expect(trailingCodeExecuted).to.be.true;
                done();
            }
            catch (e) {
                done(e);
            }
        }, undefined);
        trailingCodeExecuted = true;
    });

    // See https://github.com/BabylonJS/JsRuntimeHost/issues/9
    // it("should call the given functions in the correct order", function (done) {
    //     const called = [];
    //     for (let i = 9; i >= 0; i--) {
    //         setTimeout(() => {
    //             called.push(i);
    //             if (called.length === 10) {
    //                 try {
    //                     expect(called).to.deep.equal([0, 1, 2, 3, 4, 5, 6, 7, 8, 9]);
    //                     done();
    //                 }
    //                 catch (e) {
    //                     done(e);
    //                 }
    //             }
    //         }, i * 10);
    //     }
    // });
});

describe("clearTimeout", function () {
    this.timeout(5000);

    it("should stop the timeout matching the given timeout id", function (done) {
        const id = setTimeout(() => {
            done(new Error("Timeout was not cleared"));
        }, 0);
        clearTimeout(id);
        setTimeout(done, 100);
    });

    it("should do nothing if the given timeout id is undefined", function (done) {
        setTimeout(() => { done(); }, 0);
        clearTimeout(undefined);
    });

    it("should be interchangeable with clearInterval", function (done) {
        const id = setTimeout(() => {
            done(new Error("Interval was not cleared"));
        }, 0);
        clearInterval(id);
        setTimeout(done, 100);
    });
});

describe("setInterval", function () {
    this.timeout(5000);

    it("should return an id greater than zero", function () {
        const id = setInterval(() => { }, 0);
        clearInterval(id);
        expect(id).to.be.greaterThan(0);
    });

    it("should call the given function at the given interval", function (done) {
        let startTime = new Date().getTime();
        let tickCount = 0;
        const id = setInterval(() => {
            try {
                tickCount++;
                expect(new Date().getTime() - startTime).to.be.at.least(tickCount * 10);
                if (tickCount > 2) {
                    clearInterval(id);
                    done();
                }
            }
            catch (e) {
                console.log(`finished with error: ${e}`);
                clearInterval(id);
                done(e);
            }
        }, 10);
    });

    it("should not starve other queued work when the interval has no delay", function (done) {
        // Regression test: a repeating timeout used to be re-armed on the timer
        // thread immediately, before its callback had run on the JS thread. With a
        // zero delay that produced an unbounded backlog of queued callbacks which
        // starved every other item on the JS dispatch queue, so this setTimeout
        // would never fire.
        let finished = false;
        const intervalId = setInterval(() => { });

        const timeoutId = setTimeout(() => {
            finished = true;
            clearInterval(intervalId);
            done();
        }, 100);

        setTimeout(() => {
            if (!finished) {
                clearInterval(intervalId);
                clearTimeout(timeoutId);
                done(new Error("setTimeout was starved by a zero delay setInterval"));
            }
        }, 2000);
    });

    it("should stop when cleared from within its own callback", function (done) {
        // Exercises the re-arm path: a repeating timeout is now re-armed only
        // after its callback returns, so a clear from inside the callback must
        // win and no further ticks may occur.
        let ticks = 0;
        let id = 0;
        id = setInterval(() => {
            ticks++;
            clearInterval(id);
        }, 10);

        setTimeout(() => {
            try {
                expect(ticks).to.equal(1);
                done();
            }
            catch (e) {
                done(e);
            }
        }, 200);
    });
});

describe("clearInterval", function () {
    this.timeout(5000);

    it("should stop the interval matching the given interval id", function (done) {
        const id = setInterval(() => {
            done(new Error("Interval was not cleared"));
        }, 0);
        clearInterval(id);
        setTimeout(done, 100);
    });

    it("should do nothing if the given interval id is undefined", function (done) {
        setTimeout(() => { done(); }, 0);
        clearInterval(undefined);
    });

    it("should be interchangeable with clearTimeout", function (done) {
        const id = setInterval(() => {
            done(new Error("Interval was not cleared"));
        }, 0);
        clearTimeout(id);
        setTimeout(done, 100);
    });
});
