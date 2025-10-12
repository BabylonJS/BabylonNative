// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
#include "test_main.h"

#include <gtest/gtest.h>
#include <gtest/internal/gtest-internal.h>

#include <filesystem>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_set>
#include "child_process.h"
#include "string_utils.h"

namespace fs = std::filesystem;

namespace node_api_tests {

/*static*/ std::filesystem::path TestFixtureBase::node_lite_path_;
/*static*/ std::filesystem::path TestFixtureBase::js_root_dir_;

std::filesystem::path ResolveNodeLitePath(const std::filesystem::path& exePath);

/*static*/ void TestFixtureBase::InitializeGlobals(
    const char* test_exe_path) noexcept {
  fs::path exePath = fs::canonical(test_exe_path);

  fs::path nodeLitePath =
      ResolveNodeLitePath(fs::path(test_exe_path));
  if (!fs::exists(nodeLitePath)) {
    std::cerr << "Error: Cannot find node_lite executable at "
              << nodeLitePath << std::endl;
    exit(1);
  }
  TestFixtureBase::node_lite_path_ = nodeLitePath;

  fs::path testRootPath = exePath.parent_path();
  fs::path rootJsPath = testRootPath / "test";
  if (!fs::exists(rootJsPath)) {
    testRootPath = testRootPath.parent_path();
    rootJsPath = testRootPath / "test";
  }
  if (!fs::exists(rootJsPath)) {
    std::cerr << "Error: Cannot find test directory." << std::endl;
    exit(1);
  }
  TestFixtureBase::js_root_dir_ = rootJsPath;
}

// Forward declaration
int EvaluateJSFile(int argc, char** argv);

struct NodeApiTestFixture : TestFixtureBase {
  explicit NodeApiTestFixture(fs::path testProcess, fs::path jsFilePath)
      : m_testProcess(std::move(testProcess)),
        m_jsFilePath(std::move(jsFilePath)) {}
  static void SetUpTestSuite() {}
  static void TearDownTestSuite() {}
  void SetUp() override {}
  void TearDown() override {}

  void TestBody() override {
    ProcessResult result =
        SpawnSync(m_testProcess.string(), {m_jsFilePath.string()});
    if (result.status == 0) {
      return;
    }
    if (!result.std_error.empty()) {
      std::stringstream errorStream(result.std_error);
      std::vector<std::string> errorLines;
      std::string line;
      while (std::getline(errorStream, line)) {
        if (!line.empty() && line[line.size() - 1] == '\r') {
          line.erase(line.size() - 1, std::string::npos);
        }
        errorLines.push_back(line);
      }
      if (errorLines.size() >= 3) {
        std::string file = errorLines[0].find("file:") == 0
                               ? errorLines[0].substr(std::size("file:") - 1)
                               : "<Unknown>";
        int line = errorLines[1].find("line:") == 0
                       ? std::stoi(errorLines[1].substr(std::size("line:") - 1))
                       : 0;
        std::string message = errorLines[2];
        std::stringstream details;
        for (size_t i = 3; i < errorLines.size(); i++) {
          details << errorLines[i] << std::endl;
        }
        GTEST_MESSAGE_AT_(file.c_str(),
                          line,
                          message.c_str(),
                          ::testing::TestPartResult::kFatalFailure)
            << details.str();
        return;
      }
    }
    ASSERT_EQ(result.status, 0);
  }

  static void RegisterNodeApiTests();

 private:
  fs::path m_testProcess;
  fs::path m_jsFilePath;
};

std::string SanitizeName(const std::string& name) {
  return ReplaceAll(ReplaceAll(name, "-", "_"), ".", "_");
}

#ifdef NODE_API_TESTS_HAVE_NATIVE_MODULES
const std::unordered_set<std::string>& GetEnabledNativeModuleFolders() {
  static const std::unordered_set<std::string> modules = []() {
    std::unordered_set<std::string> result;
#ifdef NODE_API_AVAILABLE_NATIVE_TESTS
    std::stringstream stream(NODE_API_AVAILABLE_NATIVE_TESTS);
    std::string entry;
    while (std::getline(stream, entry, ',')) {
      if (!entry.empty()) {
        result.insert(entry);
      }
    }
#endif
    return result;
  }();
  return modules;
}
#endif

std::filesystem::path ResolveNodeLitePath(const std::filesystem::path& exePath) {
  fs::path nodeLitePath = exePath;
  nodeLitePath.replace_filename("node_lite");
#if defined(_WIN32)
  nodeLitePath += ".exe";
#endif
  return nodeLitePath;
}

}  // namespace node_api_tests

namespace node_api_tests {

void NodeApiTestFixture::RegisterNodeApiTests() {
  for (const fs::directory_entry& dir_entry :
       fs::recursive_directory_iterator(js_root_dir_)) {
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
#ifdef NODE_API_TESTS_HAVE_NATIVE_MODULES
      std::string moduleName = jsFilePath.parent_path().filename().string();
      if (GetEnabledNativeModuleFolders().count(moduleName) == 0) {
        continue;
      }
      includeTest = true;
#else
      includeTest = false;
#endif
    }

    if (!includeTest) {
      continue;
    }

    std::string testSuiteName = SanitizeName(suiteFolder);
    std::string testName =
        SanitizeName(jsFilePath.parent_path().filename().string() + "_" +
                     jsFilePath.filename().string());

    ::testing::RegisterTest(
        testSuiteName.c_str(),
        testName.c_str(),
        nullptr,
        nullptr,
        jsFilePath.string().c_str(),
        1,
        [jsFilePath]() {
          return new NodeApiTestFixture(node_lite_path_, jsFilePath);
        });
  }
}

}  // namespace node_api_tests

int main(int argc, char** argv) {
  node_api_tests::TestFixtureBase::InitializeGlobals(argv[0]);
  ::testing::InitGoogleTest(&argc, argv);
  node_api_tests::NodeApiTestFixture::RegisterNodeApiTests();
  return RUN_ALL_TESTS();
}
