#include <Babylon/Plugins/NativeMeshopt.h>
#include <Babylon/JsRuntime.h>

#include <napi/napi.h>

#include <meshoptimizer.h>

// bgfx vendors its own copy of meshoptimizer under bgfx/3rdparty/meshoptimizer. It is reached
// as <meshoptimizer/src/meshoptimizer.h> rather than <meshoptimizer.h>, so it does not collide
// today, but the guard in Dependencies/CMakeLists.txt skips our FetchContent when a target named
// meshoptimizer already exists. If that ever resolves the other way we would decode with a
// different codec version than the one this file was written against, which fails as silent
// data corruption rather than a build break. Pin it.
static_assert(MESHOPTIMIZER_VERSION == 220, "NativeMeshopt expects meshoptimizer 0.22; check the include path and revalidate the decode paths before bumping.");

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace Babylon::Plugins
{
    namespace
    {
        // MESHOPTIMIZER_VERSION is an integer like 220 meaning 0.22. Render it the way the
        // project versions its own releases so the JavaScript side can compare it directly
        // against the version a stream was produced with.
        std::string MeshoptVersionString()
        {
            return std::to_string(MESHOPTIMIZER_VERSION / 1000) + "." + std::to_string((MESHOPTIMIZER_VERSION / 10) % 100);
        }
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

            const int64_t countIn = info[1].As<Napi::Number>().Int64Value();
            const int64_t strideIn = info[2].As<Napi::Number>().Int64Value();
            const std::string mode = info[3].As<Napi::String>().Utf8Value();

            // meshoptimizer validates these with assert(), which compiles out in release builds,
            // so out-of-range values would be undefined behavior rather than a thrown error.
            if (countIn < 0)
            {
                throw Napi::RangeError::New(env, "Meshopt: count must not be negative, got " + std::to_string(countIn));
            }
            if (strideIn <= 0 || strideIn > 256)
            {
                throw Napi::RangeError::New(env, "Meshopt: stride must be in [1, 256], got " + std::to_string(strideIn));
            }
            if (mode == "ATTRIBUTES")
            {
                if (strideIn % 4 != 0)
                {
                    throw Napi::RangeError::New(env, "Meshopt: ATTRIBUTES stride must be a multiple of 4, got " + std::to_string(strideIn));
                }
            }
            else if (mode == "TRIANGLES" || mode == "INDICES")
            {
                if (strideIn != 2 && strideIn != 4)
                {
                    throw Napi::RangeError::New(env, "Meshopt: " + mode + " stride must be 2 or 4, got " + std::to_string(strideIn));
                }
                if (mode == "TRIANGLES" && countIn % 3 != 0)
                {
                    throw Napi::RangeError::New(env, "Meshopt: TRIANGLES count must be a multiple of 3, got " + std::to_string(countIn));
                }
            }
            else
            {
                throw Napi::Error::New(env, "Meshopt: Unsupported decode mode: " + mode);
            }

            const size_t count = static_cast<size_t>(countIn);
            const size_t stride = static_cast<size_t>(strideIn);

            // Round count up to a multiple of 4 (the reference decoder over-allocates
            // and runs the vertex filter over count4 elements).
            const size_t count4 = (count + 3) & ~static_cast<size_t>(3);

            // Guard the allocation size so a huge count cannot wrap size_t.
            constexpr int64_t maxDecodedBytes = 1LL << 31;
            if (static_cast<int64_t>(count4) * strideIn > maxDecodedBytes)
            {
                throw Napi::RangeError::New(env, "Meshopt: decoded size (" + std::to_string(count4) + " x " +
                    std::to_string(strideIn) + " bytes) exceeds the 2 GB limit.");
            }

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
            else
            {
                result = meshopt_decodeIndexSequence(temp.data(), count, stride, source, sourceSize);
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

        // Grouped for the same reasons as DracoCodec. Version matters more here: meshoptimizer
        // stores its codec version in the first header byte and a decoder rejects streams newer
        // than it understands, returning an error rather than degraded output. Publishing the
        // version lets the JavaScript side fall back before it tries.
        auto codec = Napi::Object::New(env);
        codec.Set("Decode", Napi::Function::New(env, DecodeMeshopt, "Decode"));
        codec.Set("Version", Napi::String::New(env, MeshoptVersionString()));
        native.Set("MeshoptCodec", codec);
    }
}
