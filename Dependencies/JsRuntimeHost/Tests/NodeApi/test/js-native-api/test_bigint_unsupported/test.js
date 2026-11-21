'use strict';
const common = require('../../common');
const assert = require('assert');
const {
  CreateBigIntExpectThrow,
} = require(`./build/${common.buildType}/test_bigint_unsupported`);

// On engines without BigInt (jsc-android ~2020, Win10 Chakra) the BigInt create API throws a
// JS-catchable ENOTSUP. The standard test_bigint can't run on these engines at all -- its `0n`
// literals raise a SyntaxError at parse time -- so this is the feature-detection fallback.
let threw = false;
try {
  CreateBigIntExpectThrow();
} catch (err) {
  threw = true;
  assert.strictEqual(err.code, 'ENOTSUP');
}
assert.strictEqual(threw, true, 'expected napi_create_bigint_int64 to throw ENOTSUP');
