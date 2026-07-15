#include <Babylon/Plugins/NativeDraco.h>
#include <Babylon/JsRuntime.h>

#include <napi/napi.h>

#include <draco/compression/decode.h>
#include <draco/compression/encode.h>
#include <draco/core/decoder_buffer.h>
#include <draco/core/encoder_buffer.h>
#include <draco/mesh/mesh.h>
#include <draco/point_cloud/point_cloud.h>
#include <draco/attributes/geometry_attribute.h>
#include <draco/attributes/point_attribute.h>

#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

namespace Babylon::Plugins
{
    namespace
    {
        // De-interleaves and tightly packs one Draco attribute's per-point values into a
        // freshly allocated JS typed array of type T. This mirrors emscripten's
        // GetAttributeDataArrayForAllPoints, which Babylon's WASM decoder relies on.
        template<typename T>
        Napi::Value CopyAttributeData(Napi::Env env, const draco::PointCloud& pointCloud, const draco::PointAttribute& attribute)
        {
            const int8_t numComponents = attribute.num_components();
            const uint32_t numPoints = pointCloud.num_points();
            const size_t numValues = static_cast<size_t>(numPoints) * numComponents;

            auto array = Napi::TypedArrayOf<T>::New(env, numValues);
            T* out = array.Data();

            for (draco::PointIndex i(0); i < numPoints; ++i)
            {
                const draco::AttributeValueIndex valueIndex = attribute.mapped_index(i);
                attribute.ConvertValue<T>(valueIndex, numComponents, out + static_cast<size_t>(i.value()) * numComponents);
            }

            return array;
        }

        // Builds the { kind, data, size, byteOffset, byteStride, normalized } record that
        // Babylon's DracoDecoder consumes for each decoded vertex attribute.
        Napi::Value DecodeAttribute(Napi::Env env, const draco::PointCloud& pointCloud, const draco::PointAttribute& attribute, const std::string& kind)
        {
            const int8_t numComponents = attribute.num_components();

            Napi::Value data;
            uint32_t bytesPerComponent = 0;
            switch (attribute.data_type())
            {
                case draco::DT_FLOAT32: data = CopyAttributeData<float>(env, pointCloud, attribute); bytesPerComponent = 4; break;
                case draco::DT_INT8: data = CopyAttributeData<int8_t>(env, pointCloud, attribute); bytesPerComponent = 1; break;
                case draco::DT_UINT8: data = CopyAttributeData<uint8_t>(env, pointCloud, attribute); bytesPerComponent = 1; break;
                case draco::DT_INT16: data = CopyAttributeData<int16_t>(env, pointCloud, attribute); bytesPerComponent = 2; break;
                case draco::DT_UINT16: data = CopyAttributeData<uint16_t>(env, pointCloud, attribute); bytesPerComponent = 2; break;
                case draco::DT_INT32: data = CopyAttributeData<int32_t>(env, pointCloud, attribute); bytesPerComponent = 4; break;
                case draco::DT_UINT32: data = CopyAttributeData<uint32_t>(env, pointCloud, attribute); bytesPerComponent = 4; break;
                default:
                    throw Napi::Error::New(env, "Draco: Cannot decode invalid attribute data type " + std::to_string(attribute.data_type()));
            }

            auto result = Napi::Object::New(env);
            result.Set("kind", Napi::String::New(env, kind));
            result.Set("data", data);
            result.Set("size", Napi::Number::New(env, numComponents));
            // GetAttributeDataArrayForAllPoints returns a tightly packed array, so the
            // consumable buffer has offset 0 and a stride of one full vertex.
            result.Set("byteOffset", Napi::Number::New(env, 0));
            result.Set("byteStride", Napi::Number::New(env, static_cast<double>(numComponents) * bytesPerComponent));
            result.Set("normalized", Napi::Boolean::New(env, attribute.normalized()));
            return result;
        }

        Napi::Value DecodeDracoMesh(const Napi::CallbackInfo& info)
        {
            auto env = info.Env();

            if (info.Length() < 1 || !info[0].IsTypedArray())
            {
                throw Napi::TypeError::New(env, "decodeDracoMesh: expected a typed array of compressed Draco data");
            }

            const auto typedArray = info[0].As<Napi::TypedArray>();
            const auto* data = static_cast<const char*>(typedArray.ArrayBuffer().Data()) + typedArray.ByteOffset();
            const size_t size = typedArray.ByteLength();

            draco::DecoderBuffer buffer;
            buffer.Init(data, size);

            draco::Decoder decoder;
            const auto geometryTypeStatus = draco::Decoder::GetEncodedGeometryType(&buffer);
            if (!geometryTypeStatus.ok())
            {
                throw Napi::Error::New(env, geometryTypeStatus.status().error_msg());
            }

            std::unique_ptr<draco::PointCloud> geometry;
            Napi::Value indices = env.Null();

            switch (geometryTypeStatus.value())
            {
                case draco::TRIANGULAR_MESH:
                {
                    auto meshStatus = decoder.DecodeMeshFromBuffer(&buffer);
                    if (!meshStatus.ok())
                    {
                        throw Napi::Error::New(env, meshStatus.status().error_msg());
                    }

                    std::unique_ptr<draco::Mesh> mesh = std::move(meshStatus).value();

                    const uint32_t numFaces = mesh->num_faces();
                    auto indicesArray = Napi::Uint32Array::New(env, static_cast<size_t>(numFaces) * 3);
                    uint32_t* out = indicesArray.Data();
                    for (uint32_t f = 0; f < numFaces; ++f)
                    {
                        const draco::Mesh::Face& face = mesh->face(draco::FaceIndex(f));
                        out[f * 3 + 0] = face[0].value();
                        out[f * 3 + 1] = face[1].value();
                        out[f * 3 + 2] = face[2].value();
                    }
                    indices = indicesArray;

                    geometry = std::move(mesh);
                    break;
                }
                case draco::POINT_CLOUD:
                {
                    auto pointCloudStatus = decoder.DecodePointCloudFromBuffer(&buffer);
                    if (!pointCloudStatus.ok())
                    {
                        throw Napi::Error::New(env, pointCloudStatus.status().error_msg());
                    }
                    geometry = std::move(pointCloudStatus).value();
                    break;
                }
                default:
                    throw Napi::Error::New(env, "Draco: Cannot decode invalid geometry type");
            }

            auto attributes = Napi::Array::New(env);
            uint32_t attributeCount = 0;

            if (info.Length() > 1 && info[1].IsObject())
            {
                // glTF path: caller provides a map of Babylon vertex-buffer kind -> Draco unique id.
                const auto attributeIds = info[1].As<Napi::Object>();
                const auto keys = attributeIds.GetPropertyNames();
                for (uint32_t i = 0; i < keys.Length(); ++i)
                {
                    const auto kind = keys.Get(i).As<Napi::String>().Utf8Value();
                    const uint32_t id = attributeIds.Get(kind).As<Napi::Number>().Uint32Value();
                    const draco::PointAttribute* attribute = geometry->GetAttributeByUniqueId(id);
                    if (attribute != nullptr)
                    {
                        attributes.Set(attributeCount++, DecodeAttribute(env, *geometry, *attribute, kind));
                    }
                }
            }
            else
            {
                // Standalone path: probe the standard named attributes.
                const struct
                {
                    const char* kind;
                    draco::GeometryAttribute::Type type;
                } namedAttributes[] = {
                    {"position", draco::GeometryAttribute::POSITION},
                    {"normal", draco::GeometryAttribute::NORMAL},
                    {"color", draco::GeometryAttribute::COLOR},
                    {"uv", draco::GeometryAttribute::TEX_COORD},
                };

                for (const auto& named : namedAttributes)
                {
                    if (geometry->GetNamedAttributeId(named.type) != -1)
                    {
                        attributes.Set(attributeCount++, DecodeAttribute(env, *geometry, *geometry->GetNamedAttribute(named.type), named.kind));
                    }
                }
            }

            auto result = Napi::Object::New(env);
            result.Set("indices", indices);
            result.Set("attributes", attributes);
            result.Set("totalVertices", Napi::Number::New(env, static_cast<double>(geometry->num_points())));
            return result;
        }

        // ----------------------------------------------------------------------------
        // Encoder
        // ----------------------------------------------------------------------------

        draco::GeometryAttribute::Type DracoAttributeTypeFromName(const std::string& name)
        {
            if (name == "POSITION") return draco::GeometryAttribute::POSITION;
            if (name == "NORMAL") return draco::GeometryAttribute::NORMAL;
            if (name == "COLOR") return draco::GeometryAttribute::COLOR;
            if (name == "TEX_COORD") return draco::GeometryAttribute::TEX_COORD;
            return draco::GeometryAttribute::GENERIC;
        }

        // Returns a typed pointer to the start of the typed array's data, honoring its byte offset.
        template<typename T>
        const T* TypedArrayData(const Napi::TypedArray& array)
        {
            const auto* base = static_cast<const uint8_t*>(array.ArrayBuffer().Data()) + array.ByteOffset();
            return reinterpret_cast<const T*>(base);
        }

        // Replicates draco's emscripten PointCloudBuilder::AddAttribute<T>: creates a de-interleaved
        // per-point attribute and returns its attribute id (which equals its unique id, see
        // PointCloud::SetAttribute -> set_unique_id).
        template<typename T>
        int AddAttributeToMesh(draco::Mesh& mesh, draco::GeometryAttribute::Type type, draco::DataType dataType, long numVertices, int8_t numComponents, const T* values)
        {
            std::unique_ptr<draco::PointAttribute> att(new draco::PointAttribute());
            att->Init(type, numComponents, dataType, /* normalized */ false, numVertices);
            const int attId = mesh.AddAttribute(std::move(att));
            draco::PointAttribute* attPtr = mesh.attribute(attId);
            for (draco::PointIndex i(0); i < numVertices; ++i)
            {
                attPtr->SetAttributeValue(attPtr->mapped_index(i), &values[static_cast<size_t>(i.value()) * numComponents]);
            }
            if (mesh.num_points() == 0)
            {
                mesh.set_num_points(numVertices);
            }
            else if (mesh.num_points() != static_cast<uint32_t>(numVertices))
            {
                return -1;
            }
            return attId;
        }

        // Dispatches AddAttributeToMesh on the typed array's element type, mirroring the WASM
        // encoder's addAttributeMap.
        int AddTypedAttributeToMesh(Napi::Env env, draco::Mesh& mesh, draco::GeometryAttribute::Type type, const Napi::TypedArray& data, int8_t numComponents)
        {
            const long numVertices = static_cast<long>(data.ElementLength()) / numComponents;
            switch (data.TypedArrayType())
            {
                case napi_float32_array: return AddAttributeToMesh<float>(mesh, type, draco::DT_FLOAT32, numVertices, numComponents, TypedArrayData<float>(data));
                case napi_uint32_array: return AddAttributeToMesh<uint32_t>(mesh, type, draco::DT_UINT32, numVertices, numComponents, TypedArrayData<uint32_t>(data));
                case napi_uint16_array: return AddAttributeToMesh<uint16_t>(mesh, type, draco::DT_UINT16, numVertices, numComponents, TypedArrayData<uint16_t>(data));
                case napi_uint8_array:
                case napi_uint8_clamped_array: return AddAttributeToMesh<uint8_t>(mesh, type, draco::DT_UINT8, numVertices, numComponents, TypedArrayData<uint8_t>(data));
                case napi_int32_array: return AddAttributeToMesh<int32_t>(mesh, type, draco::DT_INT32, numVertices, numComponents, TypedArrayData<int32_t>(data));
                case napi_int16_array: return AddAttributeToMesh<int16_t>(mesh, type, draco::DT_INT16, numVertices, numComponents, TypedArrayData<int16_t>(data));
                case napi_int8_array: return AddAttributeToMesh<int8_t>(mesh, type, draco::DT_INT8, numVertices, numComponents, TypedArrayData<int8_t>(data));
                default:
                    throw Napi::Error::New(env, "Draco: Unsupported attribute typed array for encoding");
            }
        }

        // Reads an index typed array (Uint16Array or Uint32Array) into a flat int vector.
        std::vector<int> ReadIndices(const Napi::TypedArray& data)
        {
            const size_t count = data.ElementLength();
            std::vector<int> out(count);
            if (data.TypedArrayType() == napi_uint32_array)
            {
                const uint32_t* src = TypedArrayData<uint32_t>(data);
                for (size_t i = 0; i < count; ++i) { out[i] = static_cast<int>(src[i]); }
            }
            else
            {
                const uint16_t* src = TypedArrayData<uint16_t>(data);
                for (size_t i = 0; i < count; ++i) { out[i] = static_cast<int>(src[i]); }
            }
            return out;
        }

        Napi::Value EncodeDracoMesh(const Napi::CallbackInfo& info)
        {
            auto env = info.Env();

            if (info.Length() < 1 || !info[0].IsArray())
            {
                throw Napi::TypeError::New(env, "encodeDracoMesh: expected an array of attributes");
            }

            const auto attributesIn = info[0].As<Napi::Array>();
            const auto options = (info.Length() > 2 && info[2].IsObject()) ? info[2].As<Napi::Object>() : Napi::Object::New(env);

            // Locate the mandatory position attribute and its vertex count.
            long positionVerticesCount = 0;
            bool hasPosition = false;
            for (uint32_t i = 0; i < attributesIn.Length(); ++i)
            {
                const auto attr = attributesIn.Get(i).As<Napi::Object>();
                if (attr.Get("dracoName").As<Napi::String>().Utf8Value() == "POSITION")
                {
                    const auto data = attr.Get("data").As<Napi::TypedArray>();
                    const int8_t size = static_cast<int8_t>(attr.Get("size").As<Napi::Number>().Int32Value());
                    positionVerticesCount = static_cast<long>(data.ElementLength()) / size;
                    hasPosition = true;
                    break;
                }
            }
            if (!hasPosition)
            {
                throw Napi::Error::New(env, "Draco: Missing position attribute for encoding.");
            }

            // Indices: use the provided buffer, or synthesize an identity list for unindexed meshes.
            std::vector<int> indices;
            if (info.Length() > 1 && info[1].IsTypedArray())
            {
                indices = ReadIndices(info[1].As<Napi::TypedArray>());
            }
            else
            {
                indices.resize(positionVerticesCount);
                for (long i = 0; i < positionVerticesCount; ++i) { indices[i] = static_cast<int>(i); }
            }

            draco::Mesh mesh;
            const long numFaces = static_cast<long>(indices.size()) / 3;
            mesh.SetNumFaces(numFaces);
            for (draco::FaceIndex f(0); f < numFaces; ++f)
            {
                draco::Mesh::Face face;
                face[0] = draco::PointIndex(indices[f.value() * 3 + 0]);
                face[1] = draco::PointIndex(indices[f.value() * 3 + 1]);
                face[2] = draco::PointIndex(indices[f.value() * 3 + 2]);
                mesh.SetFace(f, face);
            }

            draco::Encoder encoder;

            const bool hasQuantization = options.Has("quantizationBits") && options.Get("quantizationBits").IsObject();
            const auto quantizationBits = hasQuantization ? options.Get("quantizationBits").As<Napi::Object>() : Napi::Object::New(env);

            auto attributeIds = Napi::Object::New(env);
            for (uint32_t i = 0; i < attributesIn.Length(); ++i)
            {
                const auto attr = attributesIn.Get(i).As<Napi::Object>();
                const std::string kind = attr.Get("kind").As<Napi::String>().Utf8Value();
                const std::string dracoName = attr.Get("dracoName").As<Napi::String>().Utf8Value();
                const int8_t size = static_cast<int8_t>(attr.Get("size").As<Napi::Number>().Int32Value());
                const auto data = attr.Get("data").As<Napi::TypedArray>();
                const draco::GeometryAttribute::Type type = DracoAttributeTypeFromName(dracoName);

                const int attId = AddTypedAttributeToMesh(env, mesh, type, data, size);
                if (attId < 0)
                {
                    throw Napi::Error::New(env, "Draco: Failed to add attribute '" + kind + "' (vertex count mismatch).");
                }
                attributeIds.Set(kind, Napi::Number::New(env, attId));

                if (hasQuantization && quantizationBits.Has(dracoName))
                {
                    const int32_t bits = quantizationBits.Get(dracoName).As<Napi::Number>().Int32Value();
                    if (bits) // matches WASM path: only set for truthy (non-zero) values
                    {
                        encoder.SetAttributeQuantization(type, bits);
                    }
                }
            }

            if (options.Has("method") && options.Get("method").IsString())
            {
                const std::string method = options.Get("method").As<Napi::String>().Utf8Value();
                encoder.SetEncodingMethod(method == "MESH_SEQUENTIAL_ENCODING" ? draco::MESH_SEQUENTIAL_ENCODING : draco::MESH_EDGEBREAKER_ENCODING);
            }

            if (options.Has("encodeSpeed") && options.Get("encodeSpeed").IsNumber() && options.Has("decodeSpeed") && options.Get("decodeSpeed").IsNumber())
            {
                encoder.SetSpeedOptions(options.Get("encodeSpeed").As<Napi::Number>().Int32Value(), options.Get("decodeSpeed").As<Napi::Number>().Int32Value());
            }

            // Mirror Encoder::EncodeMeshToDracoBuffer.
            if (mesh.GetNamedAttributeId(draco::GeometryAttribute::POSITION) == -1)
            {
                throw Napi::Error::New(env, "Draco: Missing position attribute for encoding.");
            }
            if (!mesh.DeduplicateAttributeValues())
            {
                throw Napi::Error::New(env, "Draco: Failed to deduplicate attribute values.");
            }
            mesh.DeduplicatePointIds();

            draco::EncoderBuffer buffer;
            const draco::Status status = encoder.EncodeMeshToBuffer(mesh, &buffer);
            if (!status.ok())
            {
                throw Napi::Error::New(env, std::string("Draco: Failed to encode: ") + status.error_msg());
            }

            auto encodedData = Napi::Int8Array::New(env, buffer.size());
            std::memcpy(encodedData.Data(), buffer.data(), buffer.size());

            auto result = Napi::Object::New(env);
            result.Set("data", encodedData);
            result.Set("attributeIds", attributeIds);
            return result;
        }
    }
}

namespace Babylon::Plugins::NativeDraco
{
    void BABYLON_API Initialize(Napi::Env env)
    {
        auto native{JsRuntime::NativeObject::GetFromJavaScript(env)};
        native.Set("decodeDracoMesh", Napi::Function::New(env, DecodeDracoMesh, "decodeDracoMesh"));
        native.Set("encodeDracoMesh", Napi::Function::New(env, EncodeDracoMesh, "encodeDracoMesh"));
    }
}
