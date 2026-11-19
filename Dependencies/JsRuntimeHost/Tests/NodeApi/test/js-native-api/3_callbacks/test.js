'use strict';
const common = require('../../common');
const assert = require('assert');
const addon = require(`./build/${common.buildType}/3_callbacks`);

addon.RunCallback(function(msg) {
  assert.strictEqual(msg, 'hello world');
});

function testRecv(desiredRecv) {
  addon.RunCallbackWithRecv(function() {
    assert.strictEqual(this, desiredRecv);
  }, desiredRecv);
}

testRecv(undefined);
testRecv(null);
testRecv(5);
testRecv(true);
testRecv('Hello');
testRecv([]);
testRecv({});

const originalCall = Function.prototype.call;
const unexpectedCall = () => {
  assert.fail('napi_call_function must not use a replaceable call property');
};
const callback = function() {
  assert.strictEqual(this, undefined);
};
callback.call = unexpectedCall;
Function.prototype.call = unexpectedCall;
try {
  addon.RunCallbackWithRecv(callback, undefined);
} finally {
  Function.prototype.call = originalCall;
}
