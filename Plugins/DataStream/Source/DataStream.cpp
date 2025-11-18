#include "DataStream.h"

#include <functional>
#include <sstream>
#include <Babylon/JsRuntime.h>
#include <bx/bx.h>
#include "TextDecoder.h"
#include "ReadableStream.h"
#include "DecompressionStream.h"
#include "Response.h"

BX_PRAGMA_DIAGNOSTIC_PUSH();
BX_PRAGMA_DIAGNOSTIC_IGNORED_CLANG_GCC("-Wunused-function");
BX_PRAGMA_DIAGNOSTIC_IGNORED_MSVC(4505) // error C4505: '' : unreferenced local function has been removed

#include "miniz.h"

BX_PRAGMA_DIAGNOSTIC_POP();

namespace Babylon::Plugins::Internal::DataStream
{
    void Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        Napi::Object fflate = Napi::Object::New(env);

        fflate.Set("unzipSync", Napi::Function::New(env, DataStream::UnzipSync, "unzipSync"));

        // Attach to global scope
        env.Global().Set("fflate", fflate);
    }

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
}

namespace Babylon::Plugins::DataStream
{
    void BABYLON_API Initialize(Napi::Env env)
    {
        Internal::DataStream::Initialize(env);
        Internal::TextDecoder::Initialize(env);
        Internal::DecompressionStream::Initialize(env);
        Internal::ReadableStream::Initialize(env);
        Internal::Response::Initialize(env);
    }
}
