#include <functional>
#include <sstream>
#include <Babylon/JsRuntime.h>
#include "TextDecoder.h"
#include "ReadableStream.h"
#include "ReadableStreamDefaultController.h"
#include "DecompressionStream.h"
#include "Response.h"

namespace Babylon::Polyfills::DecompressionStream
{
    void BABYLON_API Initialize(Napi::Env env)
    {
        Internal::TextDecoder::Initialize(env);
        Internal::DecompressionStream::Initialize(env);
        Internal::ReadableStream::Initialize(env);
        Internal::ReadableStreamDefaultController::Initialize(env);
        Internal::Response::Initialize(env);
    }
}
