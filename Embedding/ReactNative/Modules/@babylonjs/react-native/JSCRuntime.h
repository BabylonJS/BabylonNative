// NOTE: This file was copied from the JSCRuntime.h included in React Native 0.63.1 with modifications in order to:
//   1. Access the underlying JSC JSGlobalContextRef.
//   2. Create a custom JSCRuntime with bug fixes.

/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <jsi/jsi.h>
#include <memory.h>
#include <JavaScriptCore/JavaScript.h>

namespace facebook {
namespace jsc2 {

std::unique_ptr<jsi::Runtime> makeJSCRuntime(JSGlobalContextRef ctx);

JSGlobalContextRef getJSGlobalContextRefFromJSCRuntime(jsi::Runtime& runtime);

} // namespace jsc
} // namespace facebook
