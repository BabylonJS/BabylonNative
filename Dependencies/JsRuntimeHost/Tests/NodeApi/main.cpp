#include "test_main.h"

#include <gtest/gtest.h>

#include <filesystem>
#include <sstream>
#include <string>
#include <unordered_set>

#include "child_process.h"

namespace fs = std::filesystem;

namespace {

fs::path ResolveNodeLitePath(const fs::path& exe_path) {
  fs::path nodeLitePath = exe_path;
  nodeLitePath.replace_filename("node_lite");
#if defined(_WIN32)
  nodeLitePath += ".exe";
#endif
  return nodeLitePath;
}

fs::path ResolveTestsRoot(const fs::path& exe_path) {
  fs::path testRootPath = exe_path.parent_path();
  fs::path js_root = testRootPath / "test";
  if (!fs::exists(js_root)) {
    testRootPath = testRootPath.parent_path();
    js_root = testRootPath / "test";
  }
  return js_root;
}

std::unordered_set<std::string> ParseEnabledNativeSuites() {
  std::unordered_set<std::string> suites;
#ifdef NODE_API_AVAILABLE_NATIVE_TESTS
  std::stringstream stream(NODE_API_AVAILABLE_NATIVE_TESTS);
  std::string entry;
  while (std::getline(stream, entry, ',')) {
    if (!entry.empty()) {
      suites.insert(entry);
    }
  }
#endif
  return suites;
}

}  // namespace

int main(int argc, char** argv) {
  fs::path exe_path = fs::canonical(argv[0]);
  fs::path js_root = ResolveTestsRoot(exe_path);
  if (!fs::exists(js_root)) {
    std::cerr << "Error: Cannot find Node-API test directory." << std::endl;
    return EXIT_FAILURE;
  }

  fs::path node_lite_path = ResolveNodeLitePath(exe_path);
  if (!fs::exists(node_lite_path)) {
    std::cerr << "Error: Cannot find node_lite executable at "
              << node_lite_path << std::endl;
    return EXIT_FAILURE;
  }

  node_api_tests::NodeApiTestConfig config{};
  config.js_root = js_root;
  config.run_script =
      [node_lite_path](const fs::path& script_path)
          -> node_api_tests::ProcessResult {
    return node_api_tests::SpawnSync(node_lite_path.string(),
                                     {script_path.string()});
  };
  config.enabled_native_suites = ParseEnabledNativeSuites();

  node_api_tests::InitializeNodeApiTests(config);

  ::testing::InitGoogleTest(&argc, argv);
  node_api_tests::RegisterNodeApiTests();
  return RUN_ALL_TESTS();
}
