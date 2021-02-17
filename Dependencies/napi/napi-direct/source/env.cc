#include <napi/env.h>
#include <sstream>

namespace Napi
{
    Napi::Value Eval(Napi::Env env, const char* source, const char* sourceUrl)
    {
        napi_value result;

        // Append the source URL so V8 can locate the file.
        std::ostringstream sourceWithURL;
        sourceWithURL << source << "\n";
        sourceWithURL << "//# sourceURL=" << sourceUrl << "\n";

        NAPI_THROW_IF_FAILED(env, napi_run_script(env, Napi::String::New(env, sourceWithURL.str().c_str()), sourceUrl, &result));
        return{ env, result };
    }
}
