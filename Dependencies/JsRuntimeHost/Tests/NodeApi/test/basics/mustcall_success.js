const common = require('../common');

var fn = common.mustCall();
fn();

Promise.resolve().then(() => {
  setImmediate(common.mustCall());
});
