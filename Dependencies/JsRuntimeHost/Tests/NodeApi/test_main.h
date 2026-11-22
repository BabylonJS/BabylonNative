// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#ifndef NODE_API_TEST_TEST_MAIN_H
#define NODE_API_TEST_TEST_MAIN_H

#include <gtest/gtest.h>
#include <filesystem>
#include <functional>
#include <unordered_set>

#include "child_process.h"

namespace node_api_tests {

struct NodeApiTestConfig {
  std::filesystem::path js_root;
  std::function<ProcessResult(const std::filesystem::path&)> run_script;
  std::unordered_set<std::string> enabled_native_suites;
};

void InitializeNodeApiTests(const NodeApiTestConfig& config) noexcept;
const NodeApiTestConfig& GetNodeApiTestConfig() noexcept;
void RegisterNodeApiTests();

class TestFixtureBase : public ::testing::Test {
 protected:
  static const NodeApiTestConfig& Config() noexcept;
};

}  // namespace node_api_tests

#endif  // !NODE_API_TEST_TEST_MAIN_H
