#include <Babylon/Plugins/NativeMeshopt.h>
#include <Babylon/JsRuntime.h>

#include <napi/napi.h>

#include <meshoptimizer.h>

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace Babylon::Plugins
{
    namespace
    {
        // Native equivalent of MeshoptDecoder.decodeGltfBufferAsync:
        //   decodeMeshopt(source: Uint8Array, count, stride, mode, filter?) -> Uint8Array
        // where mode is "ATTRIBUTES" | "TRIANGLES" | "INDICES" and filter (optional)
        // is "NONE" | "OCTAHEDRAL" | "QUATERNION" | "EXPONENTIAL". Mirrors the
        // reference meshopt_decoder.js decode() helper exactly: decode into a buffer
        // sized for count rounded up to a multiple of 4, then apply the filter
        // in-place over that rounded count, then return the first count*stride bytes.
        Napi::Value DecodeMeshopt(const Napi::CallbackInfo& info)
        {
            const auto env = info.Env();

            if (info.Length() < 4 || !info[0].IsTypedArray())
            {
                throw Napi::TypeError::New(env, "Meshopt: decodeMeshopt(source, count, stride, mode, filter?) requires a source typed array.");
            }

            const auto sourceArray = info[0].As<Napi::TypedArray>();
            const auto* source = static_cast<const unsigned char*>(sourceArray.ArrayBuffer().Data()) + sourceArray.ByteOffset();
            const size_t sourceSize = sourceArray.ByteLength();

            const size_t count = static_cast<size_t>(info[1].As<Napi::Number>().Int64Value());
            const size_t stride = static_cast<size_t>(info[2].As<Napi::Number>().Int64Value());
            const std::string mode = info[3].As<Napi::String>().Utf8Value();

            // Round count up to a multiple of 4 (the reference decoder over-allocates
            // and runs the vertex filter over count4 elements).
            const size_t count4 = (count + 3) & ~static_cast<size_t>(3);

            std::vector<unsigned char> temp(count4 * stride, 0);

            int result;
            if (mode == "ATTRIBUTES")
            {
                result = meshopt_decodeVertexBuffer(temp.data(), count, stride, source, sourceSize);
            }
            else if (mode == "TRIANGLES")
            {
                result = meshopt_decodeIndexBuffer(temp.data(), count, stride, source, sourceSize);
            }
            else if (mode == "INDICES")
            {
                result = meshopt_decodeIndexSequence(temp.data(), count, stride, source, sourceSize);
            }
            else
            {
                throw Napi::Error::New(env, std::string("Meshopt: Unsupported decode mode: ") + mode);
            }

            if (result != 0)
            {
                throw Napi::Error::New(env, "Meshopt: Malformed buffer data: " + std::to_string(result));
            }

            if (info.Length() > 4 && info[4].IsString())
            {
                const std::string filter = info[4].As<Napi::String>().Utf8Value();
                if (filter == "OCTAHEDRAL")
                {
                    meshopt_decodeFilterOct(temp.data(), count4, stride);
                }
                else if (filter == "QUATERNION")
                {
                    meshopt_decodeFilterQuat(temp.data(), count4, stride);
                }
                else if (filter == "EXPONENTIAL")
                {
                    meshopt_decodeFilterExp(temp.data(), count4, stride);
                }
                else if (filter != "NONE")
                {
                    throw Napi::Error::New(env, std::string("Meshopt: Unsupported decode filter: ") + filter);
                }
            }

            const size_t outSize = count * stride;
            auto output = Napi::Uint8Array::New(env, outSize);
            std::memcpy(output.Data(), temp.data(), outSize);
            return output;
        }
    }
}

namespace Babylon::Plugins::NativeMeshopt
{
    void BABYLON_API Initialize(Napi::Env env)
    {
        auto native{JsRuntime::NativeObject::GetFromJavaScript(env)};
        native.Set("decodeMeshopt", Napi::Function::New(env, DecodeMeshopt, "decodeMeshopt"));
    }
}
