#include <functional>
#include <sstream>
#include <Babylon/JsRuntime.h>

#ifdef _MSC_VER
    #pragma warning(push)
    #pragma warning(disable: 4505) // error C4505: '' : unreferenced local function has been removed
#endif
#ifdef __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunused-function"
#endif
#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wunused-function"
#endif

#include "miniz.h"

#ifdef _MSC_VER
    #pragma warning(pop)
#endif
#ifdef __GNUC__
    #pragma GCC diagnostic pop
#endif
#ifdef __clang__
    #pragma clang diagnostic pop
#endif

namespace Babylon::Plugins::Internal::NativeZip
{
    Napi::Value UnzipSync(const Napi::CallbackInfo& info)
    {
        const Napi::Env env{info.Env()};

        if (info.Length() < 1 || !info[0].IsTypedArray()) {
            throw Napi::Error::New(env, "Expected Uint8Array argument");
        }

        // zip content
        Napi::Uint8Array input = info[0].As<Napi::Uint8Array>();
        const uint8_t* zipData = input.Data();
        const size_t zipSize = input.ByteLength();

        mz_zip_archive zip_archive{};

        if (!mz_zip_reader_init_mem(&zip_archive, zipData, zipSize, 0)) {
            throw Napi::Error::New(env, "Failed to initialize zip archive");
        }

        const auto fileCount = mz_zip_reader_get_num_files(&zip_archive);
        Napi::Object result = Napi::Object::New(env);
        for (mz_uint i = 0; i < fileCount; i++)
        {
            mz_zip_archive_file_stat file_stat;
            if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat))
            {
                continue;
            }

            if (mz_zip_reader_is_file_a_directory(&zip_archive, i))
            {
                continue; // skip directories
            }

            std::string filename(file_stat.m_filename);

            size_t uncompressed_size = (size_t)file_stat.m_uncomp_size;
            Napi::ArrayBuffer jsBuffer = Napi::ArrayBuffer::New(env, uncompressed_size);

            if (!mz_zip_reader_extract_to_mem(&zip_archive, i, jsBuffer.Data(), uncompressed_size, 0))
            {
                throw Napi::Error::New(env, "Failed to extract file");
                continue;
            }

            Napi::Uint8Array jsArray = Napi::Uint8Array::New(env, uncompressed_size, jsBuffer, 0);

            // result[filename] = Uint8Array
            result.Set(Napi::String::New(env, filename), jsArray);
        }
        mz_zip_reader_end(&zip_archive);
        
        return result;
    }

    void Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        Napi::Object fflate = Napi::Object::New(env);

        fflate.Set("UnzipSync", Napi::Function::New(env, UnzipSync, "UnzipSync"));

        // Attach to global scope
        env.Global().Set("fflate", fflate);
    }
}

namespace Babylon::Plugins::NativeZip
{
    void BABYLON_API Initialize(Napi::Env env)
    {
        Internal::NativeZip::Initialize(env);
    }
}
