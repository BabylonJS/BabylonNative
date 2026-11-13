const { test } = require("node:test");
const assert = require("node:assert/strict");
const { summarizePhase } = require("./run.cjs");

function report(status = "passed") {
    return {
        stats: { duration: 20 },
        suites: [{ specs: [{
            title: "Scene",
            tests: [{ results: [{ status, duration: 12, errors: status === "passed" ? [] : [{ message: "Mismatch" }],
                steps: [{ title: "Compare screenshot", duration: 3, steps: [{ title: "Nested assertion", duration: 2 }] }] }] }],
        }] }],
    };
}

test("reports one completed test and counts only top-level steps", () => {
    const summary = summarizePhase(report(), ["Scene"]);
    assert.equal(summary.executed, 1);
    assert.equal(summary.passed, 1);
    assert.equal(summary.failed, 0);
    assert.equal(summary.testsMs, 12);
    assert.equal(summary.capturedFrames, 1);
    assert.deepEqual(summary.stagesMs, { "Compare screenshot": 3 });
});

test("retains failed comparisons and error details", () => {
    const summary = summarizePhase(report("failed"), ["Scene"]);
    assert.equal(summary.passed, 0);
    assert.equal(summary.failed, 1);
    assert.equal(summary.failures[0].errors[0].message, "Mismatch");
});

test("rejects absent, substituted and duplicated scenes", () => {
    assert.throws(() => summarizePhase(report(), ["Missing"]), /scene names/);
    assert.throws(() => summarizePhase(report(), ["Scene", "Missing"]), /scene names/);
    assert.throws(() => summarizePhase(report(), ["Scene", "Scene"]), /scene names/);
    const duplicate = report();
    duplicate.suites[0].specs.push(duplicate.suites[0].specs[0]);
    assert.throws(() => summarizePhase(duplicate, ["Scene", "Other"]), /scene names/);
});

test("rejects retries and skipped or interrupted tests", () => {
    const retried = report();
    retried.suites[0].specs[0].tests[0].results.push(retried.suites[0].specs[0].tests[0].results[0]);
    assert.throws(() => summarizePhase(retried, ["Scene"]), /one attempt/);
    for (const status of ["skipped", "interrupted"]) {
        assert.throws(() => summarizePhase(report(status), ["Scene"]), /did not complete/);
    }
});
