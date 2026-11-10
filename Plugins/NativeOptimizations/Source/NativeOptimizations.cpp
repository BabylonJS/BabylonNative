#include <Babylon/Plugins/NativeOptimizations.h>
#include <Babylon/JsRuntime.h>
#include <algorithm>
#include <array>
#include <optional>

namespace
{
    // Babylon.js stores matrices in a plain Array when useHighPrecisionMatrix is set.
    std::array<float, 16> ReadMatrix(const Napi::Object& transform)
    {
        const auto value{transform.Get("_m")};
        std::array<float, 16> m{};
        if (value.IsArray())
        {
            const auto elements{value.As<Napi::Array>()};
            for (uint32_t i = 0; i < m.size(); ++i)
            {
                m[i] = elements.Get(i).As<Napi::Number>().FloatValue();
            }
        }
        else
        {
            const auto elements{value.As<Napi::Float32Array>()};
            for (uint32_t i = 0; i < m.size(); ++i)
            {
                m[i] = elements[i];
            }
        }
        return m;
    }

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
        const auto m{ReadMatrix(transform)};
        // offset and length are optional in Babylon.js, defaulting to the whole array.
        const auto offset{info[2].IsUndefined() ? 0 : info[2].As<Napi::Number>().Uint32Value()};
        const auto length{info[3].IsUndefined() ? static_cast<uint32_t>(coordinates.ElementLength()) : info[3].As<Napi::Number>().Uint32Value()};

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
        const auto m{ReadMatrix(transform)};
        // offset and length are optional in Babylon.js, defaulting to the whole array.
        const auto offset{info[2].IsUndefined() ? 0 : info[2].As<Napi::Number>().Uint32Value()};
        const auto length{info[3].IsUndefined() ? static_cast<uint32_t>(normals.ElementLength()) : info[3].As<Napi::Number>().Uint32Value()};

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
        const auto m{ReadMatrix(transform)};
        // offset and length are optional in Babylon.js, defaulting to the whole array.
        const auto offset{info[2].IsUndefined() ? 0 : info[2].As<Napi::Number>().Uint32Value()};
        const auto length{info[3].IsUndefined() ? static_cast<uint32_t>(normals.ElementLength()) : info[3].As<Napi::Number>().Uint32Value()};

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
        const auto offset{info[1].IsUndefined() ? 0 : info[1].As<Napi::Number>().Uint32Value()};
        const auto length{info[2].IsUndefined() ? static_cast<uint32_t>(indices.ElementLength()) : info[2].As<Napi::Number>().Uint32Value()};

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
        const auto matrixStorage{ modelView.Get("_m") };
        if (!matrixStorage.IsArray() &&
            !(matrixStorage.IsTypedArray() && matrixStorage.As<Napi::TypedArray>().TypedArrayType() == napi_float32_array))
        {
            throw Napi::TypeError::New(info.Env(), "sortSplats requires modelView._m to be a Float32Array or Array.");
        }
        const auto m{ matrixStorage.As<Napi::Object>() };

        auto positions{ info[1].As<Napi::Float32Array>() };

        auto indices{ info[2].As<Napi::Float32Array>() };

        auto rightHand{ info[3].As<Napi::Boolean>() };


        float depthFactor = -1.f;
        if (rightHand) {
            depthFactor = 1.f;
        }

        const auto indexCount = indices.ElementLength();
        // The index buffer may be padded (e.g. to a multiple of 16) past the
        // splat count, so only sort splats that have a position.
        const auto splatCount = std::min(indexCount, positions.ElementLength() / 4);
        // High-precision Babylon matrices use ordinary number arrays.
        const auto m2{m.Get(2u)};
        const auto m6{m.Get(6u)};
        const auto m10{m.Get(10u)};
        if (!m2.IsNumber() || !m6.IsNumber() || !m10.IsNumber())
        {
            throw Napi::TypeError::New(info.Env(), "sortSplats requires modelView._m[2], [6], [10] to be numbers.");
        }
        float vp[3] = {
            m2.As<Napi::Number>().FloatValue(),
            m6.As<Napi::Number>().FloatValue(),
            m10.As<Napi::Number>().FloatValue()
        };
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

        // Point padding entries at the one-past-the-end sentinel, like the Babylon.js sort worker.
        for (size_t i = splatCount; i < indexCount; i++)
        {
            indices[i] = float(splatCount);
        }
    }

    // Coefficients of the sort key for splats transformed by one world matrix: key = dot([x y z 1], coefficients).
    // Babylon matrices use row vectors, so clip = [x y z 1] * world * viewProjection and the key is one clip column.
    std::array<float, 4> SplatKeyCoefficients(const std::array<float, 16>& world, const std::array<float, 16>& viewProjection, uint32_t column)
    {
        std::array<float, 4> coefficients{};
        for (uint32_t row = 0; row < 4; ++row)
        {
            for (uint32_t k = 0; k < 4; ++k)
            {
                coefficients[row] += world[row * 4 + k] * viewProjection[k * 4 + column];
            }
        }
        return coefficients;
    }

    // sortSplatIndices(viewProjection, worldMatrices, positions, indices, vertexCount, intervals, partIndices): boolean
    // Mirrors the Babylon.js sort worker. Writes the active source splats (every splat below vertexCount, or the
    // [start, count] pairs of intervals) to indices sorted back to front, and points the remaining entries at the
    // empty slot at vertexCount. worldMatrices holds the mesh world matrix, or one world matrix per part when
    // partIndices is set. Returns false and leaves indices untouched when a part index has no world matrix.
    Napi::Value SortSplatIndices(const Napi::CallbackInfo& info)
    {
        const auto env{info.Env()};
        const auto isTypedArray = [](const Napi::Value& value, napi_typedarray_type type) {
            return value.IsTypedArray() && value.As<Napi::TypedArray>().TypedArrayType() == type;
        };

        if (!info[0].IsObject() || !info[1].IsArray() || !isTypedArray(info[2], napi_float32_array) ||
            !isTypedArray(info[3], napi_float32_array) || !info[4].IsNumber())
        {
            throw Napi::TypeError::New(env, "sortSplatIndices requires (Matrix, Matrix[], Float32Array, Float32Array, number, Uint32Array | null, Uint8Array | null).");
        }
        const bool hasIntervals = isTypedArray(info[5], napi_uint32_array);
        const bool hasPartIndices = isTypedArray(info[6], napi_uint8_array);
        if ((!hasIntervals && !info[5].IsNull() && !info[5].IsUndefined()) || (!hasPartIndices && !info[6].IsNull() && !info[6].IsUndefined()))
        {
            throw Napi::TypeError::New(env, "sortSplatIndices requires intervals to be a Uint32Array or null and partIndices to be a Uint8Array or null.");
        }

        const auto viewProjection{ReadMatrix(info[0].As<Napi::Object>())};
        const auto worldMatrices{info[1].As<Napi::Array>()};
        const auto positions{info[2].As<Napi::Float32Array>()};
        auto indices{info[3].As<Napi::Float32Array>()};
        const uint32_t vertexCount{info[4].As<Napi::Number>().Uint32Value()};

        const uint32_t partCount{worldMatrices.Length()};
        if (partCount == 0)
        {
            throw Napi::RangeError::New(env, "sortSplatIndices requires at least one world matrix.");
        }
        if (positions.ElementLength() / 4 < vertexCount)
        {
            throw Napi::RangeError::New(env, "sortSplatIndices requires 4 position floats per splat.");
        }

        // Sort by distance from the camera: clip w with a perspective projection (for either handedness and for
        // reversed depth), and clip z with an orthographic one, whose clip w is constant.
        const bool perspective = viewProjection[3] != 0.f || viewProjection[7] != 0.f || viewProjection[11] != 0.f;
        const uint32_t keyColumn = perspective ? 3 : 2;
        std::vector<std::array<float, 4>> coefficients(partCount);
        for (uint32_t part = 0; part < partCount; ++part)
        {
            const auto world{worldMatrices.Get(part)};
            if (!world.IsObject())
            {
                throw Napi::TypeError::New(env, "sortSplatIndices requires worldMatrices to contain matrices.");
            }
            coefficients[part] = SplatKeyCoefficients(ReadMatrix(world.As<Napi::Object>()), viewProjection, keyColumn);
        }

        std::optional<Napi::Uint8Array> partIndices;
        if (hasPartIndices && info[6].As<Napi::Uint8Array>().ElementLength() > 0)
        {
            partIndices = info[6].As<Napi::Uint8Array>();
        }

        const uint32_t defaultInterval[2]{0, vertexCount};
        const uint32_t* intervalData{defaultInterval};
        size_t intervalWords{2};
        if (hasIntervals)
        {
            const auto intervals{info[5].As<Napi::Uint32Array>()};
            intervalData = intervals.Data();
            intervalWords = intervals.ElementLength() & ~size_t{1};
        }

        size_t activeCount{0};
        for (size_t word = 0; word < intervalWords; word += 2)
        {
            if (intervalData[word] > vertexCount || intervalData[word + 1] > vertexCount - intervalData[word])
            {
                throw Napi::RangeError::New(env, "sortSplatIndices requires intervals within vertexCount.");
            }
            activeCount += intervalData[word + 1];
        }
        if (activeCount > indices.ElementLength())
        {
            throw Napi::RangeError::New(env, "sortSplatIndices requires indices to hold every active splat.");
        }

        struct SplatKey
        {
            float Key;
            uint32_t Index;
        };
        // This function is not threadsafe because of static
        static std::vector<SplatKey> keys;
        keys.clear();
        keys.reserve(activeCount);

        const float* position{positions.Data()};
        for (size_t word = 0; word < intervalWords; word += 2)
        {
            const uint32_t end{intervalData[word] + intervalData[word + 1]};
            for (uint32_t index = intervalData[word]; index < end; ++index)
            {
                uint32_t part{0};
                if (partIndices.has_value())
                {
                    const auto& parts{partIndices.value()};
                    part = parts[std::min<size_t>(index, parts.ElementLength() - 1)];
                    if (part >= partCount)
                    {
                        // Part indices and matrices disagree mid-rebuild: keep the last good order.
                        return Napi::Boolean::New(env, false);
                    }
                }
                const auto& c{coefficients[part]};
                const float* p{position + 4 * size_t{index}};
                // Negate the distance so the ascending sort puts the farthest splat first.
                keys.push_back({-(c[0] * p[0] + c[1] * p[1] + c[2] * p[2] + c[3]), index});
            }
        }

        std::stable_sort(keys.begin(), keys.end(), [](const SplatKey& a, const SplatKey& b) { return a.Key < b.Key; });

        float* indexData{indices.Data()};
        for (size_t i = 0; i < activeCount; ++i)
        {
            indexData[i] = float(keys[i].Index);
        }
        std::fill(indexData + activeCount, indexData + indices.ElementLength(), float(vertexCount));

        return Napi::Boolean::New(env, true);
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
        nativeObject.Set("sortSplatIndices", Napi::Function::New(env, SortSplatIndices, "sortSplatIndices"));
    }
}
