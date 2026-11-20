const assert = require("node:assert/strict");
const fs = require("node:fs");
const path = require("node:path");
const crypto = require("node:crypto");
const workload = require("./workload.json");
const shared = require("./shared.cjs");

assert.equal(workload.nativeSource, "397d5c09ec01fce62b206ba4b68b2fc189b4f744");
assert.equal(workload.referenceSource, "d09f642c3813c8df178215acadc27f649510675a");
assert.equal(workload.cases.length, 305);
assert.equal(new Set(workload.cases.map(item => item.index)).size, 305);
for (const item of workload.cases) {
    assert.ok(["first", "second"].includes(item.partition));
    assert.deepEqual(shared.config.tests[item.index], item.scene);
    const reference = fs.readFileSync(path.join(shared.sourceRoot, "visualization", "ReferenceImages", item.referenceFile));
    assert.equal(crypto.createHash("sha256").update(reference).digest("hex"), item.referenceSha256);
}
console.log(`Verified 305 exact scene definitions and unchanged references; partitions ${
    workload.cases.filter(item => item.partition === "first").length}/${
    workload.cases.filter(item => item.partition === "second").length}`);
