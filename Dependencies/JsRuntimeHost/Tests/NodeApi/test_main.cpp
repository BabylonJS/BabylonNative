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

    ASSERT_EQ(result.status, 0);
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
