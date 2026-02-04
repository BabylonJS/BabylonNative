#include <Babylon/Plugins/NativeOptimizations.h>
#include <Babylon/JsRuntime.h>
#include <optional>

namespace
{
    void MatrixScaleAdd(const float inputMatrix[16], float scale, float outputMatrix[16])
    {
        for (size_t i = 0; i < 16; ++i)
        {
            outputMatrix[i] += inputMatrix[i] * scale;
        }
    }

    void MatrixTransformNormal(const float matrix[16], float &x, float &y, float &z)
    {
        float rx = x * matrix[0] + y * matrix[4] + z * matrix[8];
        float ry = x * matrix[1] + y * matrix[5] + z * matrix[9];
        float rz = x * matrix[2] + y * matrix[6] + z * matrix[10];

        x = rx;
        y = ry;
        z = rz;
    }

    void MatrixTransformCoordinates(const float matrix[16], float& x, float& y, float& z)
    {
        float rx = x * matrix[0] + y * matrix[4] + z * matrix[8] + matrix[12];
        float ry = x * matrix[1] + y * matrix[5] + z * matrix[9] + matrix[13];
        float rz = x * matrix[2] + y * matrix[6] + z * matrix[10] + matrix[14];
        float rw = 1.0f / (x * matrix[3] + y * matrix[7] + z * matrix[11] + matrix[15]);

        x = rx * rw;
        y = ry * rw;
        z = rz * rw;
    }

    void TransformVector3Coordinates(const Napi::CallbackInfo& info)
    {
        auto coordinates{info[0].As<Napi::Float32Array>()};
        const auto transform{info[1].As<Napi::Object>()};
        const auto m{transform.Get("_m").As<Napi::Float32Array>()};
        const auto offset{info[2].As<Napi::Number>().Uint32Value()};
        const auto length{info[3].As<Napi::Number>().Uint32Value()};

        for (size_t index = offset; index < offset + length; index += 3)
        {
            const auto x{coordinates[index]}, y{coordinates[index + 1]}, z{coordinates[index + 2]};
            const auto rx{x * m[0U] + y * m[4U] + z * m[8U] + m[12U]};
            const auto ry{x * m[1U] + y * m[5U] + z * m[9U] + m[13U]};
            const auto rz{x * m[2U] + y * m[6U] + z * m[10U] + m[14U]};
            const auto rw{1 / (x * m[3U] + y * m[7U] + z * m[11U] + m[15U])};

            coordinates[index + 0] = rx * rw;
            coordinates[index + 1] = ry * rw;
            coordinates[index + 2] = rz * rw;
        }
    }

    void TransformVector3Normals(const Napi::CallbackInfo& info)
    {
        auto normals{info[0].As<Napi::Float32Array>()};
        const auto transform{info[1].As<Napi::Object>()};
        const auto m{transform.Get("_m").As<Napi::Float32Array>()};
        const auto offset{info[2].As<Napi::Number>().Uint32Value()};
        const auto length{info[3].As<Napi::Number>().Uint32Value()};

        for (size_t index = offset; index < offset + length; index += 3)
        {
            const auto x{normals[index]}, y{normals[index + 1]}, z{normals[index + 2]};

            normals[index + 0] = x * m[0U] + y * m[4U] + z * m[8U];
            normals[index + 1] = x * m[1U] + y * m[5U] + z * m[9U];
            normals[index + 2] = x * m[2U] + y * m[6U] + z * m[10U];
        }
    }

    void TransformVector4Normals(const Napi::CallbackInfo& info)
    {
        auto normals{info[0].As<Napi::Float32Array>()};
        const auto transform{info[1].As<Napi::Object>()};
        const auto m{transform.Get("_m").As<Napi::Float32Array>()};
        const auto offset{info[2].As<Napi::Number>().Uint32Value()};
        const auto length{info[3].As<Napi::Number>().Uint32Value()};

        for (size_t index = offset; index < offset + length; index += 4)
        {
            const auto x{normals[index]}, y{normals[index + 1]}, z{normals[index + 2]};

            normals[index + 0] = x * m[0U] + y * m[4U] + z * m[8U];
            normals[index + 1] = x * m[1U] + y * m[5U] + z * m[9U];
            normals[index + 2] = x * m[2U] + y * m[6U] + z * m[10U];
        }
    }

    template<typename IndexT>
    void FlipIndicesT(Napi::TypedArrayOf<IndexT> indices, uint32_t offset, uint32_t length)
    {
        for (size_t index = offset; index < offset + length; index += 3)
        {
            const auto tmp{indices[index + 1]};
            indices[index + 1] = indices[index + 2];
            indices[index + 2] = tmp;
        }
    }

    void FlipFaces(const Napi::CallbackInfo& info)
    {
        auto indices{info[0].As<Napi::TypedArray>()};
        const auto offset{info[1].As<Napi::Number>().Uint32Value()};
        const auto length{info[2].As<Napi::Number>().Uint32Value()};

        if (indices.TypedArrayType() == napi_typedarray_type::napi_int32_array)
        {
            FlipIndicesT<int32_t>(indices.As<Napi::Int32Array>(), offset, length);
        }
        else if (indices.TypedArrayType() == napi_typedarray_type::napi_uint32_array)
        {
            FlipIndicesT<uint32_t>(indices.As<Napi::Uint32Array>(), offset, length);
        }
        else if (indices.TypedArrayType() == napi_typedarray_type::napi_uint16_array)
        {
            FlipIndicesT<uint16_t>(indices.As<Napi::Uint16Array>(), offset, length);
        }
        else
        {
            throw Napi::Error::New(info.Env(), "Indices TypedArray element type was unexpected.");
        }
    }

    template<typename IndexT>
    void ExtractMinAndMaxIndexedT(const Napi::Float32Array positions, const Napi::TypedArrayOf<IndexT> indices, uint32_t indexStart, uint32_t indexCount, Napi::Object minVector, Napi::Object maxVector)
    {
        auto minX{minVector.Get("_x").As<Napi::Number>().FloatValue()};
        auto minY{minVector.Get("_y").As<Napi::Number>().FloatValue()};
        auto minZ{minVector.Get("_z").As<Napi::Number>().FloatValue()};
        auto maxX{maxVector.Get("_x").As<Napi::Number>().FloatValue()};
        auto maxY{maxVector.Get("_y").As<Napi::Number>().FloatValue()};
        auto maxZ{maxVector.Get("_z").As<Napi::Number>().FloatValue()};

        for (auto index = indexStart; index < indexStart + indexCount; index++)
        {
            const auto offset{static_cast<size_t>(indices[index] * 3)};
            const auto x{positions[offset]};
            const auto y{positions[offset + 1]};
            const auto z{positions[offset + 2]};

            minX = std::min(minX, x);
            minY = std::min(minY, y);
            minZ = std::min(minZ, z);
            maxX = std::max(maxX, x);
            maxY = std::max(maxY, y);
            maxZ = std::max(maxZ, z);
        }

        minVector.Set("_x", minX);
        minVector.Set("_y", minY);
        minVector.Set("_z", minZ);
        maxVector.Set("_x", maxX);
        maxVector.Set("_y", maxY);
        maxVector.Set("_z", maxZ);
    }

    void ExtractMinAndMaxIndexed(const Napi::CallbackInfo& info)
    {
        const auto positions{info[0].As<Napi::Float32Array>()};
        const auto indices{info[1].As<Napi::TypedArray>()};
        const auto indexStart{info[2].As<Napi::Number>().Uint32Value()};
        const auto indexCount{info[3].As<Napi::Number>().Uint32Value()};
        auto minVector{info[4].As<Napi::Object>()};
        auto maxVector{info[5].As<Napi::Object>()};

        if (indices.TypedArrayType() == napi_typedarray_type::napi_int32_array)
        {
            ExtractMinAndMaxIndexedT<int32_t>(positions, indices.As<Napi::Int32Array>(), indexStart, indexCount, minVector, maxVector);
        }
        else if (indices.TypedArrayType() == napi_typedarray_type::napi_uint32_array)
        {
            ExtractMinAndMaxIndexedT<uint32_t>(positions, indices.As<Napi::Uint32Array>(), indexStart, indexCount, minVector, maxVector);
        }
        else if (indices.TypedArrayType() == napi_typedarray_type::napi_uint16_array)
        {
            ExtractMinAndMaxIndexedT<uint16_t>(positions, indices.As<Napi::Uint16Array>(), indexStart, indexCount, minVector, maxVector);
        }
        else
        {
            throw std::runtime_error{"Indices TypedArray element type was unexpected."};
        }
    }

    void ExtractMinAndMax(const Napi::CallbackInfo& info)
    {
        const auto positions{info[0].As<Napi::Float32Array>()};
        const auto start{info[1].As<Napi::Number>().Uint32Value()};
        const auto count{info[2].As<Napi::Number>().Uint32Value()};
        const auto stride{info[3].As<Napi::Number>().Uint32Value()};
        auto minVector{info[4].As<Napi::Object>()};
        auto maxVector{info[5].As<Napi::Object>()};

        auto minX{minVector.Get("_x").As<Napi::Number>().FloatValue()};
        auto minY{minVector.Get("_y").As<Napi::Number>().FloatValue()};
        auto minZ{minVector.Get("_z").As<Napi::Number>().FloatValue()};
        auto maxX{maxVector.Get("_x").As<Napi::Number>().FloatValue()};
        auto maxY{maxVector.Get("_y").As<Napi::Number>().FloatValue()};
        auto maxZ{maxVector.Get("_z").As<Napi::Number>().FloatValue()};

        for (auto index = start, offset = start * stride; index < start + count; index++, offset += stride)
        {
            const auto x{positions[offset]};
            const auto y{positions[offset + 1]};
            const auto z{positions[offset + 2]};

            minX = std::min(minX, x);
            minY = std::min(minY, y);
            minZ = std::min(minZ, z);
            maxX = std::max(maxX, x);
            maxY = std::max(maxY, y);
            maxZ = std::max(maxZ, z);
        }

        minVector.Set("_x", minX);
        minVector.Set("_y", minY);
        minVector.Set("_z", minZ);
        maxVector.Set("_x", maxX);
        maxVector.Set("_y", maxY);
        maxVector.Set("_z", maxZ);
    }

    // Ported from `applySkeleton` function in abstractMesh.ts
    void ApplySkeleton(const Napi::CallbackInfo& info)
    {
        auto data = info[0].As<Napi::Float32Array>();
        const auto kind = info[1].As<Napi::String>().Utf8Value();
        const auto skeletonMatrices = info[2].As<Napi::Float32Array>();
        const auto matricesIndicesData = info[3].As<Napi::Float32Array>();
        const auto matricesWeightsData = info[4].As<Napi::Float32Array>();

        std::optional<Napi::Float32Array> matricesIndicesExtraData;
        if (!info[5].IsNull())
        {
            matricesIndicesExtraData = info[5].As<Napi::Float32Array>();
        };

        std::optional<Napi::Float32Array> matricesWeightsExtraData;
        if (!info[6].IsNull())
        {
            matricesWeightsExtraData = info[6].As<Napi::Float32Array>();
        }

        auto matrixTransform = kind == "normal" ? MatrixTransformNormal : MatrixTransformCoordinates;

        for (size_t index = 0, matWeightIdx = 0; index < data.ElementLength(); index += 3, matWeightIdx += 4)
        {
            float finalMatrix[16]{};

            for (size_t inf = 0; inf < 4; ++inf) {
                float weight = matricesWeightsData[matWeightIdx + inf];
                if (weight > 0.0f) {
                    MatrixScaleAdd(&skeletonMatrices[static_cast<size_t>(matricesIndicesData[matWeightIdx + inf] * 16.0f)], weight, finalMatrix);
                }
            }
            if (matricesIndicesExtraData.has_value() && matricesWeightsExtraData.has_value()) {
                const auto& indices = matricesIndicesExtraData.value();
                const auto& weights = matricesWeightsExtraData.value();
                for (size_t inf = 0; inf < 4; ++inf) {
                    float weight = weights[matWeightIdx + inf];
                    if (weight > 0.0f) {
                        MatrixScaleAdd(&skeletonMatrices[static_cast<size_t>(indices[matWeightIdx + inf] * 16.0f)], weight, finalMatrix);
                    }
                }
            }

            matrixTransform(finalMatrix, data[index], data[index + 1], data[index + 2]);
        }
    }

    static int sortSplatQSort(const void* p1, const void* p2) {
        const float* a = (const float*)p1;
        const float* b = (const float*)p2;
        if (a[1] < b[1]) {
            return -1;
        }
        if (a[1] > b[1]) {
            return 1;
        }
        return 0;
    }


    // This function is not threadsafe because of static
    void sortSplats(const Napi::CallbackInfo& info)
    {
        const auto modelView{ info[0].As<Napi::Object>() };
        const auto m{ modelView.Get("_m").As<Napi::Float32Array>() };

        auto positions{ info[1].As<Napi::Float32Array>() };

        auto indices{ info[2].As<Napi::Float32Array>() };

        auto rightHand{ info[3].As<Napi::Boolean>() };


        float depthFactor = -1.f;
        if (rightHand) {
            depthFactor = 1.f;
        }

        const auto splatCount = indices.ElementLength();
        float vp[3] = { m[2u], m[6u], m[10u] };
        static std::vector<float> depthMix;

        depthMix.resize(splatCount * 2);

        for (size_t i = 0; i < splatCount; i++)
        {
            depthMix[i * 2 + 0] = float(i);
            depthMix[i * 2 + 1] = 10000.f + (vp[0] * positions[4 * i + 0] + vp[1] * positions[4 * i + 1] + vp[2] * positions[4 * i + 2]) * depthFactor;
        }

        qsort(depthMix.data(), splatCount, 2 * sizeof(float), sortSplatQSort);

        for (size_t i = 0; i < splatCount; i++)
        {
            indices[i] = depthMix[i * 2 + 0];
        }
    }
}

namespace Babylon::Plugins::NativeOptimizations
{
    void BABYLON_API Initialize(Napi::Env env)
    {
        auto nativeObject{JsRuntime::NativeObject::GetFromJavaScript(env)};
        nativeObject.Set("_ApplySkeleton", Napi::Function::New(env, ApplySkeleton, "_ApplySkeleton"));
        nativeObject.Set("_TransformVector3Coordinates", Napi::Function::New(env, TransformVector3Coordinates, "_TransformVector3Coordinates"));
        nativeObject.Set("_TransformVector3Normals", Napi::Function::New(env, TransformVector3Normals, "_TransformVector3Normals"));
        nativeObject.Set("_TransformVector4Normals", Napi::Function::New(env, TransformVector4Normals, "_TransformVector4Normals"));
        nativeObject.Set("_FlipFaces", Napi::Function::New(env, FlipFaces, "_FlipFaces"));
        nativeObject.Set("extractMinAndMaxIndexed", Napi::Function::New(env, ExtractMinAndMaxIndexed, "extractMinAndMaxIndexed"));
        nativeObject.Set("extractMinAndMax", Napi::Function::New(env, ExtractMinAndMax, "extractMinAndMax"));
        nativeObject.Set("sortSplats", Napi::Function::New(env, sortSplats, "sortSplats"));
    }
}
