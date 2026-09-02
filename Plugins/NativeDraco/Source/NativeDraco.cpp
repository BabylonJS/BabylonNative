#include <Babylon/Plugins/NativeDraco.h>
#include <Babylon/JsRuntime.h>

#include <napi/napi.h>

#include <draco/compression/decode.h>
#include <draco/core/decoder_buffer.h>
#include <draco/compression/encode.h>
#include <draco/core/encoder_buffer.h>
#include <draco/mesh/mesh.h>
#include <draco/point_cloud/point_cloud.h>
#include <draco/attributes/geometry_attribute.h>
#include <draco/attributes/point_attribute.h>
#include <draco/core/draco_version.h>

#include <cassert>
#include <cstdint>
#include <cstring>
#include <limits>
#include <memory>
#include <string>
#include <vector>

namespace Babylon::Plugins
{
    namespace
    {
        // Enumerates an object's own keys.
        //
        // Deliberately not Napi::Object::GetPropertyNames(). JsRuntimeHost's JavaScriptCore
        // backend implements napi_get_property_names by calling Object.getOwnPropertyNames with
        // an argument count of zero, so the object under inspection is never passed and the call
        // evaluates getOwnPropertyNames(undefined), which throws. That makes GetPropertyNames
        // unusable on JavaScriptCore, which is the default engine on macOS and iOS.
        //
        // Supplying the missing argument would not be enough for a general fix: Node-API
        // specifies the enumerable properties including the prototype chain (what V8 returns),
        // whereas getOwnPropertyNames is own-only and includes non-enumerables. Chakra has the
        // same enumerability mismatch and QuickJS omits the prototype chain, so this is a
        // conformance gap across all three non-V8 backends.
        // Tracked by https://github.com/BabylonJS/JsRuntimeHost/issues/216 - remove this helper
        // in favour of GetPropertyNames() once that is fixed.
        //
        // Calling Object.keys through the global object goes through napi_call_function with the
        // argument actually supplied, and behaves identically on every engine for the plain data
        // objects this map is built from.
        Napi::Array OwnPropertyNames(Napi::Env env, const Napi::Object& object)
        {
            const auto objectCtor = env.Global().Get("Object").As<Napi::Object>();
            const auto keys = objectCtor.Get("keys").As<Napi::Function>();
            return keys.Call(objectCtor, {object}).As<Napi::Array>();
        }

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
                const auto keys = OwnPropertyNames(env, attributeIds);
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

        // Returns a typed pointer to the start of the typed array's data. napi_get_typedarray_info
        // already accounts for the view's byte offset, so this is the address of the first element
        // rather than the start of the backing ArrayBuffer. Going through TypedArrayOf<T>::Data()
        // also avoids materializing a temporary ArrayBuffer handle just to read the pointer.
        template<typename T>
        const T* TypedArrayData(const Napi::TypedArray& array)
        {
            return array.As<Napi::TypedArrayOf<T>>().Data();
        }

        // Replicates draco's emscripten PointCloudBuilder::AddAttribute<T>: creates a de-interleaved
        // per-point attribute and returns its attribute id (which equals its unique id, see
        // PointCloud::SetAttribute -> set_unique_id).
        template<typename T>
        int AddAttributeToMesh(draco::Mesh& mesh, draco::GeometryAttribute::Type type, draco::DataType dataType, uint32_t numVertices, int8_t numComponents, const T* values)
        {
            std::unique_ptr<draco::PointAttribute> att(new draco::PointAttribute());
            att->Init(type, numComponents, dataType, /* normalized */ false, numVertices);
            const int attId = mesh.AddAttribute(std::move(att));
            draco::PointAttribute* attPtr = mesh.attribute(attId);
            // PointAttribute::Init ends with SetIdentityMapping(), so mapped_index(i) is
            // AttributeValueIndex(i) here. Asserted so a future Draco change cannot silently
            // start folding distinct points onto the same attribute value.
            assert(attPtr->is_mapping_identity());
            for (draco::PointIndex i(0); i < numVertices; ++i)
            {
                attPtr->SetAttributeValue(attPtr->mapped_index(i), &values[static_cast<size_t>(i.value()) * numComponents]);
            }
            if (mesh.num_points() == 0)
            {
                mesh.set_num_points(numVertices);
            }
            else if (mesh.num_points() != numVertices)
            {
                return -1;
            }
            return attId;
        }

        // Dispatches AddAttributeToMesh on the typed array's element type, mirroring the WASM
        // encoder's addAttributeMap.
        int AddTypedAttributeToMesh(Napi::Env env, draco::Mesh& mesh, draco::GeometryAttribute::Type type, const Napi::TypedArray& data, int8_t numComponents)
        {
            if (numComponents <= 0)
            {
                throw Napi::TypeError::New(env, "Draco: Attribute component count must be greater than zero");
            }
            if (data.ElementLength() % static_cast<size_t>(numComponents) != 0)
            {
                throw Napi::TypeError::New(env, "Draco: Attribute length " + std::to_string(data.ElementLength()) +
                                                    " is not a multiple of its component count " + std::to_string(numComponents));
            }

            const uint32_t numVertices = static_cast<uint32_t>(data.ElementLength() / static_cast<size_t>(numComponents));
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

        // Reads an index typed array into a flat vector. Only Uint16Array and Uint32Array are
        // accepted: any other element type would be reinterpreted and silently produce a
        // corrupt mesh. Indices stay unsigned end to end so a value above INT_MAX cannot wrap
        // negative before it reaches draco::PointIndex.
        std::vector<uint32_t> ReadIndices(Napi::Env env, const Napi::TypedArray& data)
        {
            const size_t count = data.ElementLength();
            std::vector<uint32_t> out(count);
            switch (data.TypedArrayType())
            {
                case napi_uint32_array:
                {
                    const uint32_t* src = TypedArrayData<uint32_t>(data);
                    for (size_t i = 0; i < count; ++i) { out[i] = src[i]; }
                    break;
                }
                case napi_uint16_array:
                {
                    const uint16_t* src = TypedArrayData<uint16_t>(data);
                    for (size_t i = 0; i < count; ++i) { out[i] = src[i]; }
                    break;
                }
                default:
                    throw Napi::TypeError::New(env, "Draco: Indices must be a Uint16Array or a Uint32Array");
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
            uint32_t positionVerticesCount = 0;
            bool hasPosition = false;
            for (uint32_t i = 0; i < attributesIn.Length(); ++i)
            {
                const auto attr = attributesIn.Get(i).As<Napi::Object>();
                if (attr.Get("dracoName").As<Napi::String>().Utf8Value() == "POSITION")
                {
                    const auto data = attr.Get("data").As<Napi::TypedArray>();
                    const int8_t size = static_cast<int8_t>(attr.Get("size").As<Napi::Number>().Int32Value());
                    if (size <= 0)
                    {
                        throw Napi::TypeError::New(env, "Draco: Position component count must be greater than zero");
                    }
                    positionVerticesCount = static_cast<uint32_t>(data.ElementLength() / size);
                    hasPosition = true;
                    break;
                }
            }
            if (!hasPosition)
            {
                throw Napi::Error::New(env, "Draco: Missing position attribute for encoding.");
            }

            // Indices: use the provided buffer, or synthesize an identity list for unindexed meshes.
            std::vector<uint32_t> indices;
            if (info.Length() > 1 && info[1].IsTypedArray())
            {
                indices = ReadIndices(env, info[1].As<Napi::TypedArray>());
            }
            else
            {
                indices.resize(positionVerticesCount);
                for (uint32_t i = 0; i < positionVerticesCount; ++i) { indices[i] = i; }
            }

            // This path builds triangle faces, so a trailing partial triangle is a caller error
            // rather than something to silently drop.
            if (indices.size() % 3 != 0)
            {
                throw Napi::TypeError::New(env, "Draco: Index count " + std::to_string(indices.size()) +
                                                    " is not a multiple of 3");
            }

            // Every index has to address a real vertex. Without this an out of range index would
            // be stored in a face and later dereferenced by the deduplication passes and the
            // encoder, reading past the end of the attribute buffers.
            for (const uint32_t index : indices)
            {
                if (index >= positionVerticesCount)
                {
                    throw Napi::TypeError::New(env, "Draco: Index " + std::to_string(index) +
                                                        " is out of range for " + std::to_string(positionVerticesCount) +
                                                        " vertices");
                }
            }

            draco::Mesh mesh;
            const uint32_t numFaces = static_cast<uint32_t>(indices.size() / 3);
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

            // Mirror Encoder::EncodeMeshToDracoBuffer. NativeDraco builds Draco with
            // DRACO_GLTF_BITSTREAM=ON (glTF-compatible output); that subset may compile out
            // attribute deduplication, so guard those passes on the feature macros Draco
            // publishes. They only shrink the encoded output; skipping them still produces a
            // valid stream.
            if (mesh.GetNamedAttributeId(draco::GeometryAttribute::POSITION) == -1)
            {
                throw Napi::Error::New(env, "Draco: Missing position attribute for encoding.");
            }
#ifdef DRACO_ATTRIBUTE_VALUES_DEDUPLICATION_SUPPORTED
            if (!mesh.DeduplicateAttributeValues())
            {
                throw Napi::Error::New(env, "Draco: Failed to deduplicate attribute values.");
            }
#endif
#ifdef DRACO_ATTRIBUTE_INDICES_DEDUPLICATION_SUPPORTED
            mesh.DeduplicatePointIds();
#endif

            draco::EncoderBuffer buffer;
            const draco::Status status = encoder.EncodeMeshToBuffer(mesh, &buffer);
            if (!status.ok())
            {
                throw Napi::Error::New(env, std::string("Draco: Failed to encode: ") + status.error_msg());
            }

            // Int8Array rather than Uint8Array: Babylon.js's IDracoEncodedMeshData declares
            // `data: Int8Array`, because the WASM encoder hands back a view onto emscripten's
            // signed HEAP8. The bytes are identical either way, but this entry point is meant to
            // be a drop-in for that path, so the view type has to match what callers type-check
            // against and declare.
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

        // Exposed as a single object rather than free functions so that the JavaScript side
        // needs one feature probe instead of one per entry point, and so the object can carry
        // the version of the codec built into this binary. A bare function name cannot express
        // that, and Draco's bitstream is versioned, so a caller holding a stream this build is
        // too old to read has no other way to find out ahead of time.
        //
        // Upstream links the decoder only, mirroring Babylon.js: its default configuration
        // loads draco_decoder_gltf.wasm, and the encoder is a separate module fetched on demand
        // by DracoEncoder. Linking the encoder as well gives DracoEncoder a native path and
        // costs roughly 1.2 MB of binary size.
        auto codec = Napi::Object::New(env);
        codec.Set("Decode", Napi::Function::New(env, DecodeDracoMesh, "Decode"));
        codec.Set("Encode", Napi::Function::New(env, EncodeDracoMesh, "Encode"));
        codec.Set("Version", Napi::String::New(env, draco::kDracoVersion));
        native.Set("DracoCodec", codec);
    }
}
