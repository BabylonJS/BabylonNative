import { test } from "node:test";
import assert from "node:assert/strict";
import { runInNewContext } from "node:vm";
import { encodeNativeValue, decodeNativeValue, serializeEvaluation } from "./lite-parity-page.mjs";
import { latestCases, metricsForCases, summarizeRun } from "./lite-parity-records.mjs";

test("a successful focused retry cannot turn an incomplete or failing full suite into a pass", () => {
    const cases = [{ id: "a", status: "failed" }, { id: "b", status: "passed" }];
    assert.deepEqual(summarizeRun(cases, [{ id: "a" }, { id: "b" }], "passed"), {
        status: "failed", pending: 0,
    });
    assert.deepEqual(summarizeRun(cases, [{ id: "a" }, { id: "b" }, { id: "c" }], "passed"), {
        status: "IN PROGRESS / incomplete", pending: 1,
    });
    assert.deepEqual(summarizeRun([], [], "failed"), { status: "failed", pending: 0 });
});

test("native arguments preserve special numbers and undefined properties", () => {
    const value = {
        absent: undefined,
        array: [undefined, NaN, Infinity, -Infinity, -0, null],
        nested: { ready: true, name: "scene" },
    };
    const result = decodeNativeValue(JSON.parse(JSON.stringify(encodeNativeValue(value))));
    assert.deepEqual(result, value);
    assert.ok(Object.hasOwn(result, "absent"));
    assert.ok(Object.is(result.array[4], -0));
});

test("evaluation arguments are decoded in the target context", () => {
    const expression = serializeEvaluation(value => ({ original: value, valid: Number.isNaN(value.x) }), { x: NaN });
    const result = runInNewContext(expression, { __liteParityDecode: decodeNativeValue });
    assert.equal(result.valid, true);
    assert.ok(Number.isNaN(result.original.x));
});

test("string evaluations reject ambiguous arguments", () => {
    assert.equal(serializeEvaluation("1 + 2"), "1 + 2");
    assert.throws(() => serializeEvaluation("1 + 2", { x: 1 }), /cannot receive an argument/);
});

test("cycles and malformed transport values fail explicitly", () => {
    const cycle = {};
    cycle.self = cycle;
    assert.throws(() => encodeNativeValue(cycle), /cycles/);
    assert.throws(() => decodeNativeValue({ kind: "number", value: "not-a-number" }), /Invalid/);
    assert.throws(() => decodeNativeValue({ kind: "object", value: [["x", { kind: "undefined" }], ["x", { kind: "undefined" }]] }), /Invalid/);
});

test("latest case outcomes retain retries as attempts rather than duplicate cases", () => {
    const old = { id: "scene1", status: "failed" };
    const current = { id: "scene1", status: "passed" };
    const other = { id: "scene7", status: "failed" };
    assert.deepEqual(latestCases([old, other, current]), [current, other]);
});

test("repeat-each identities do not inflate scheduled case coverage", () => {
    const location = { file: "scenes\\scene40.spec.ts", line: 39, column: 1 };
    const title = ["", "native", "Scene 40"];
    const original = { id: "original", location, title, status: "failed" };
    const repeated = { ...original, id: "repeat-2", status: "passed" };
    const otherProject = { ...original, id: "browser", title: ["", "browser", "Scene 40"] };
    const cases = latestCases([original, repeated, otherProject]);
    assert.deepEqual(cases, [repeated, otherProject]);
    assert.deepEqual(summarizeRun([repeated], [original], "passed"), {
        status: "passed", pending: 0,
    });
    assert.deepEqual(summarizeRun([repeated], [original, otherProject], "passed"), {
        status: "IN PROGRESS / incomplete", pending: 1,
    });
    assert.deepEqual(metricsForCases([
        { testId: "original" }, { testId: "repeat-2" },
    ], [repeated]), [{ testId: "repeat-2" }]);
});

test("metrics belong to the latest case attempt even when screenshot paths are reused", () => {
    const records = [{
        id: "scene1", startedAt: "2026-10-06T10:00:00Z", finishedAt: "2026-10-06T10:01:00Z",
    }];
    const old = { testId: "scene1", recordedAt: "2026-10-06T09:00:00Z" };
    const current = { testId: "scene1", recordedAt: "2026-10-06T10:00:30Z" };
    const unrelated = { testId: "scene7", recordedAt: "2026-10-06T10:00:30Z" };
    const legacy = { type: "full", result: { mad: 1 } };
    assert.deepEqual(metricsForCases([old, current, unrelated, legacy], records), [current, legacy]);
});
