#include "test_main.h"

#include <gtest/gtest.h>

#include <filesystem>
#include <sstream>
#include <string>
#include <unordered_set>

#include "child_process.h"

#if defined(_WIN32)
#include <windows.h>
#endif

namespace fs = std::filesystem;

namespace {

fs::path GetExecutablePath(const char* argv0) {
#if defined(_WIN32)
  // argv[0] is the command as typed, which omits ".exe" when launched by bare name.
  std::wstring path(MAX_PATH, L'\0');
  for (;;) {
    DWORD length = ::GetModuleFileNameW(nullptr, path.data(),
                                        static_cast<DWORD>(path.size()));
    if (length == 0) {
      return {};
    }
    if (length < path.size()) {
      path.resize(length);
      return path;
    }
    path.resize(path.size() * 2);
  }
#else
  return fs::canonical(argv0);
#endif
}

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
#if defined(_WIN32)
  ::SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
#endif

  fs::path exe_path = GetExecutablePath(argv[0]);
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
