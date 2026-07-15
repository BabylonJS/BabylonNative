#include <Babylon/Plugins/NativeDraco.h>
#include <Babylon/JsRuntime.h>

#include <napi/napi.h>

#include <draco/compression/decode.h>
#include <draco/core/decoder_buffer.h>
#include <draco/mesh/mesh.h>
#include <draco/point_cloud/point_cloud.h>
#include <draco/attributes/geometry_attribute.h>
#include <draco/attributes/point_attribute.h>

#include <cstdint>
#include <memory>
#include <string>

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
    }
}

namespace Babylon::Plugins::NativeDraco
{
    void BABYLON_API Initialize(Napi::Env env)
    {
        auto native{JsRuntime::NativeObject::GetFromJavaScript(env)};
        native.Set("decodeDracoMesh", Napi::Function::New(env, DecodeDracoMesh, "decodeDracoMesh"));
    }
}
