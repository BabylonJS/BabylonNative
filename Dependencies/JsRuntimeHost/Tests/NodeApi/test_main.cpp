// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "test_main.h"

#include <gtest/gtest.h>
#include <gtest/internal/gtest-internal.h>

#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "string_utils.h"

namespace fs = std::filesystem;

namespace node_api_tests {

namespace {

NodeApiTestConfig g_test_config{};
bool g_test_config_initialized = false;

const NodeApiTestConfig& RequireConfig() noexcept {
  if (!g_test_config_initialized) {
    std::cerr << "[NodeApiTests] configuration not initialized." << std::endl;
    std::abort();
  }
  return g_test_config;
}

std::string SanitizeName(const std::string& name) {
  return ReplaceAll(ReplaceAll(name, "-", "_"), ".", "_");
}

}  // namespace

void InitializeNodeApiTests(const NodeApiTestConfig& config) noexcept {
  g_test_config = config;
  g_test_config_initialized = true;
}

const NodeApiTestConfig& GetNodeApiTestConfig() noexcept {
  return RequireConfig();
}

const NodeApiTestConfig& TestFixtureBase::Config() noexcept {
  return RequireConfig();
}

class NodeApiTestFixture : public TestFixtureBase {
 public:
  explicit NodeApiTestFixture(fs::path jsFilePath)
      : m_jsFilePath(std::move(jsFilePath)) {}

  void TestBody() override {
    const auto& config = Config();
    ASSERT_TRUE(static_cast<bool>(config.run_script))
        << "Node-API test runner is not configured.";

    struct Exclusion {
      const char* script;
      bool applies;
      const char* reason;
    };
#if defined(JSR_NAPI_ENGINE_V8)
    constexpr bool kV8 = true;
#else
    constexpr bool kV8 = false;
#endif
#if defined(JSR_NAPI_ENGINE_HERMES)
    constexpr bool kHermes = true;
#else
    constexpr bool kHermes = false;
#endif
#if defined(JSR_NAPI_ENGINE_CHAKRA)
    constexpr bool kChakra = true;
#else
    constexpr bool kChakra = false;
#endif
#if defined(JSR_NAPI_ENGINE_QUICKJS)
    constexpr bool kQuickJS = true;
#else
    constexpr bool kQuickJS = false;
#endif
#if defined(JSR_NAPI_ENGINE_JAVASCRIPTCORE)
    constexpr bool kJavaScriptCore = true;
#else
    constexpr bool kJavaScriptCore = false;
#endif
#if defined(__ANDROID__)
    constexpr bool kAndroid = true;
#else
    constexpr bool kAndroid = false;
#endif
    static constexpr const char* kNoSpawnSync =
        "the in-process Android runner cannot spawn the child process this "
        "test runs";
    static constexpr const char* kV8WrapAbort =
        "V8 napi_wrap aborts on objects not created by napi_define_class";
    static constexpr const char* kDefineClassNullProperties =
        "napi_define_class dereferences a NULL properties array (crash)";
    static constexpr const char* kInt64Overflow =
        "napi_get_value_int64 casts out-of-range doubles instead of saturating";
    static constexpr const char* kHermesWrapFinalizer =
        "Hermes runs a napi_wrap finalizer only at env shutdown, not on "
        "garbage collection, when a result reference is requested";
    static constexpr Exclusion kExclusions[] = {
        {"test_general/test.js", true,
         "asserts napi_get_version() == 8; JsRuntimeHost targets Node-API 5"},
        {"test_function/test.js", kV8, kV8WrapAbort},
        {"test_general/testEnvCleanup.js", kV8, kV8WrapAbort},
        {"test_general/testFinalizer.js", kV8, kV8WrapAbort},
        {"test_reference_double_free/test_wrap.js", kV8, kV8WrapAbort},
        {"test_general/testNapiRun.js", kV8,
         "V8 napi_run_script aborts on a non-string script instead of returning "
         "napi_string_expected"},
        {"test_general/testNapiRun.js", kHermes,
         "Hermes exports the 3-argument napi_run_script, but the shared header "
         "declares the 4-argument variant"},
        {"7_factory_wrap/test.js", kHermes, kHermesWrapFinalizer},
        {"8_passing_wrapped/test.js", kHermes, kHermesWrapFinalizer},
        {"test_constructor/test.js", kChakra || kQuickJS,
         "napi_define_class dereferences a NULL properties array (crash); "
         "read-only assignment errors also use non-V8 message text"},
        {"test_constructor/test_null.js", kChakra || kQuickJS || kJavaScriptCore,
         kDefineClassNullProperties},
        {"test_number/test.js", kChakra || kQuickJS, kInt64Overflow},
        {"test_conversions/test.js", kChakra,
         "Chakra does not clear the last error on success, and string getters "
         "check the length pointer before the value type"},
        {"test_dataview/test.js", kChakra,
         "Chakra napi_get_dataview_info only accepts DataViews created by "
         "napi_create_dataview"},
        {"test_function/test.js", kChakra,
         "Chakra napi_create_function accepts a NULL callback, and returning "
         "from a callback with an exception pending crashes"},
        {"test_general/testFinalizer.js", kChakra,
         "Chakra napi_add_finalizer is not implemented"},
        {"test_general/testInstanceOf.js", kChakra,
         "Chakra napi_instanceof ignores Symbol.hasInstance"},
        {"test_general/testNapiRun.js", kChakra,
         "Chakra napi_run_script returns napi_invalid_arg instead of "
         "napi_string_expected for a non-string script"},
        {"test_conversions/test.js", kQuickJS,
         "QuickJS number getters coerce non-numbers instead of returning "
         "napi_number_expected, and string getters accept NULL buf and result"},
        {"test_new_target/test.js", kQuickJS,
         "QuickJS constructs from the called function's prototype instead of "
         "new.target's, so subclass instances lose their methods"},
        {"test_constructor/test.js", kJavaScriptCore,
         "napi_define_class dereferences a NULL properties array (crash); "
         "napi_define_properties takes enumerable from napi_configurable, and "
         "read-only assignment errors use non-V8 message text"},
        {"test_constructor/test2.js", kJavaScriptCore,
         "JavaScriptCore napi_define_class does not set the constructor's name"},
        {"test_conversions/test.js", kJavaScriptCore,
         "JavaScriptCore value getters coerce instead of returning "
         "napi_*_expected, and string getters write through a NULL result "
         "when buf is NULL (crash)"},
        {"test_exception/testFinalizerException.js", kJavaScriptCore,
         "JavaScriptCore runs the external's finalizer only at context "
         "teardown, where napi_throw_error allocates during garbage "
         "collection and aborts"},
        {"test_function/test.js", kJavaScriptCore,
         "JavaScriptCore napi_create_function ignores the name length and "
         "accepts a NULL callback"},
        {"test_general/testFinalizer.js", kJavaScriptCore,
         "JavaScriptCore does not call napi_add_finalizer callbacks before "
         "exit (0 of 2 expected calls)"},
        {"test_general/testNapiRun.js", kJavaScriptCore,
         "JavaScriptCore napi_run_script converts a non-string script to a "
         "string instead of returning napi_string_expected"},
        {"test_general/testNapiStatus.js", kJavaScriptCore,
         "JavaScriptCore napi_get_value_double coerces a string instead of "
         "returning napi_number_expected"},
        {"test_general/testV8Instanceof.js", kJavaScriptCore,
         "JavaScriptCore napi_instanceof passes a non-object constructor to "
         "JSValueIsInstanceOfConstructor (crash)"},
        {"test_number/test.js", kJavaScriptCore,
         "JavaScriptCore integer getters cast out-of-range doubles instead of "
         "wrapping (int32/uint32) or saturating (int64)"},
        {"test_reference_double_free/test_wrap.js", kJavaScriptCore,
         "JavaScriptCore napi_remove_wrap rejects an object wrapped with a "
         "NULL native pointer"},
        {"test_exception/testFinalizerException.js", kAndroid, kNoSpawnSync},
        {"test_general/testEnvCleanup.js", kAndroid, kNoSpawnSync},
        {"test_new_target/test.js", kAndroid && kJavaScriptCore,
         "jsc-android (WebKit r250231) predates WebKit's 2020 fix that gives "
         "JSObjectMakeConstructor instances new.target's prototype, so "
         "subclass instances lose their methods"},
    };
    const std::string path = m_jsFilePath.generic_string();
    for (const Exclusion& exclusion : kExclusions) {
      if (exclusion.applies && path.find(exclusion.script) != std::string::npos) {
        GTEST_SKIP() << exclusion.reason;
      }
    }

    ProcessResult result = config.run_script(m_jsFilePath);
    if (result.status == 0) {
      return;
    }

    if (!result.std_error.empty()) {
      std::stringstream errorStream(result.std_error);
      std::vector<std::string> errorLines;
      std::string line;
      while (std::getline(errorStream, line)) {
        if (!line.empty() && line.back() == '\r') {
          line.pop_back();
        }
        errorLines.push_back(line);
      }
      if (errorLines.size() >= 3) {
        std::string file = errorLines[0].rfind("file:", 0) == 0
                               ? errorLines[0].substr(5)
                               : "<Unknown>";
        int lineNumber = errorLines[1].rfind("line:", 0) == 0
                             ? std::stoi(errorLines[1].substr(5))
                             : 0;
        std::string message = errorLines[2];
        std::stringstream details;
        for (size_t i = 3; i < errorLines.size(); ++i) {
          details << errorLines[i] << std::endl;
        }
        GTEST_MESSAGE_AT_(file.c_str(),
                          lineNumber,
                          message.c_str(),
                          ::testing::TestPartResult::kFatalFailure)
            << details.str();
        return;
      }
    }

    ADD_FAILURE() << "node_lite exited with status " << result.status
                  << "\nstdout:\n" << result.std_output
                  << "\nstderr:\n" << result.std_error;
  }

  static void Register() {
    const auto& config = Config();
    const fs::path& js_root = config.js_root;
    if (js_root.empty()) {
      std::cerr << "[NodeApiTests] JS root directory not configured." << std::endl;
      std::abort();
    }

    for (const fs::directory_entry& dir_entry :
         fs::recursive_directory_iterator(js_root)) {
      if (!dir_entry.is_regular_file() ||
          dir_entry.path().extension() != ".js") {
        continue;
      }

      fs::path jsFilePath = dir_entry.path();
      fs::path suitePath = jsFilePath.parent_path().parent_path();
      std::string suiteFolder = suitePath.filename().string();

      bool includeTest = false;
      if (suiteFolder == "basics") {
        includeTest = true;
      } else if (suiteFolder == "js-native-api") {
        if (config.enabled_native_suites.empty()) {
          continue;
        }
        std::string moduleName = jsFilePath.parent_path().filename().string();
        includeTest =
            config.enabled_native_suites.find(moduleName) !=
            config.enabled_native_suites.end();
      } else {
        continue;
      }

      if (!includeTest) {
        continue;
      }

      std::string testSuiteName = SanitizeName(suiteFolder);
      std::string testName = SanitizeName(
          jsFilePath.parent_path().filename().string() + "_" +
          jsFilePath.filename().string());

      ::testing::RegisterTest(
          testSuiteName.c_str(),
          testName.c_str(),
          nullptr,
          nullptr,
          jsFilePath.string().c_str(),
          1,
          [jsFilePath]() { return new NodeApiTestFixture(jsFilePath); });
    }
  }

 private:
  fs::path m_jsFilePath;
};

void RegisterNodeApiTests() {
  NodeApiTestFixture::Register();
}

}  // namespace node_api_tests
