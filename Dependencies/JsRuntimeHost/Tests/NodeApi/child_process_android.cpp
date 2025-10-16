#include "child_process.h"

namespace node_api_tests {

ProcessResult SpawnSync(std::string_view /*command*/, std::vector<std::string> /*args*/)
{
    ProcessResult result{};
    result.status = -1;
    result.std_error = "child_process.spawnSync is not supported on this platform.";
    result.std_output.clear();
    return result;
}

} // namespace node_api_tests
