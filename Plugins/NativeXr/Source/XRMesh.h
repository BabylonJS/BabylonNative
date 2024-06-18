#pragma once

namespace Babylon
{
    namespace Plugins
    {
        class XRMesh : public Napi::ObjectWrap<XRMesh>
        {
            static constexpr auto JS_CLASS_NAME = "XRMesh";

        public:
            static void Initialize(Napi::Env env)
            {
                Napi::HandleScope scope{env};

                Napi::Function func = DefineClass(
                    env,
                    JS_CLASS_NAME,
                    {
                        InstanceAccessor("positions", &XRMesh::GetPositions, nullptr),
                        InstanceAccessor("indices", &XRMesh::GetIndices, nullptr),
                        InstanceAccessor("normals", &XRMesh::GetNormals, nullptr),
                        InstanceAccessor("lastChangedTime", &XRMesh::GetLastChangedTime, nullptr),
                        InstanceAccessor("parentSceneObject", &XRMesh::GetParentSceneObject, nullptr),
                    });

                env.Global().Set(JS_CLASS_NAME, func);
            }

            static Napi::Object New(const Napi::Env& env)
            {
                return env.Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({});
            }

            XRMesh(const Napi::CallbackInfo& info)
                : Napi::ObjectWrap<XRMesh>{info}
                , m_jsThis{Napi::Persistent(info.This().As<Napi::Object>())}
                , m_jsMeshSpace{Napi::External<xr::Space>::New(info.Env(), &m_meshSpace)}
            {
                // OpenXR positions vertices within reference space, so "meshSpace" is identity with respect to WebXR's
                // interpretation and our current implementation.
                m_jsThis.Set("meshSpace", m_jsMeshSpace);
            }

            ~XRMesh()
            {
                m_jsPositions.Reset();
                m_jsIndices.Reset();
                m_jsNormals.Reset();
            }

            void SetLastUpdatedTime(uint32_t timestamp)
            {
                m_lastUpdatedTimestamp = timestamp;
            }

            void SetNativeMeshId(xr::System::Session::Frame::Mesh::Identifier meshID)
            {
                m_nativeMeshID = meshID;
            }

            void SetXRFrame(Plugins::XRFrame* frame)
            {
                m_frame = frame;
            }

        private:
            xr::System::Session::Frame::Mesh& GetMesh();

            Napi::Value GetPositions(const Napi::CallbackInfo& info)
            {
                // NOTE: WebXR reports positions as right-handed coordinates
                const auto& mesh = GetMesh();
                constexpr uint8_t VECTOR3_NUM_FLOATS = 3;
                bool updateValues = false;
                if (!m_jsPositions ||
                    m_numJsPositions != VECTOR3_NUM_FLOATS * mesh.Positions.size())
                {
                    m_numJsPositions = VECTOR3_NUM_FLOATS * mesh.Positions.size();
                    m_jsPositions.Reset();
                    m_jsPositions = Napi::Persistent(Napi::Float32Array::New(info.Env(), m_numJsPositions));
                    updateValues = true;
                }
                else if (m_lastPositionsUpdatedTimestamp != m_lastUpdatedTimestamp)
                {
                    m_lastPositionsUpdatedTimestamp = m_lastUpdatedTimestamp;
                    updateValues = true;
                }

                if (updateValues)
                {
                    for (size_t n = 0; n < mesh.Positions.size(); n++)
                    {
                        m_jsPositions.Value()[VECTOR3_NUM_FLOATS * n] = mesh.Positions.at(n).X;
                        m_jsPositions.Value()[VECTOR3_NUM_FLOATS * n + 1] = mesh.Positions.at(n).Y;
                        m_jsPositions.Value()[VECTOR3_NUM_FLOATS * n + 2] = mesh.Positions.at(n).Z;
                    }
                }

                return m_jsPositions.Value();
            }

            Napi::Value GetIndices(const Napi::CallbackInfo& info)
            {
                // NOTE: WebXR reports indices in a counterclockwise winding order
                assert(sizeof(xr::System::Session::Frame::Mesh::IndexType) == sizeof(uint32_t));
                const auto& mesh = GetMesh();
                if (!m_jsIndices ||
                    m_numJsIndices != mesh.Indices.size())
                {
                    m_numJsIndices = mesh.Indices.size();
                    m_jsIndices.Reset();
                    m_jsIndices = Napi::Persistent(Napi::Uint32Array::New(info.Env(), m_numJsIndices));
                    memcpy(m_jsIndices.Value().Data(), mesh.Indices.data(), mesh.Indices.size() * sizeof(xr::System::Session::Frame::Mesh::IndexType));
                    m_lastIndicesUpdatedTimestamp = m_lastUpdatedTimestamp;
                }
                else if (m_lastIndicesUpdatedTimestamp != m_lastUpdatedTimestamp)
                {
                    memcpy(m_jsIndices.Value().Data(), mesh.Indices.data(), mesh.Indices.size() * sizeof(xr::System::Session::Frame::Mesh::IndexType));
                    m_lastIndicesUpdatedTimestamp = m_lastUpdatedTimestamp;
                }

                return m_jsIndices.Value();
            }

            Napi::Value GetNormals(const Napi::CallbackInfo& info)
            {
                // NOTE: WebXR reports normals as right-handed vectors
                const auto& mesh = GetMesh();
                if (!mesh.HasNormals)
                {
                    return info.Env().Undefined();
                }

                constexpr uint8_t VECTOR3_NUM_FLOATS = 3;
                if (!m_jsNormals ||
                    m_numJsNormals != VECTOR3_NUM_FLOATS * mesh.Normals.size())
                {
                    m_numJsNormals = VECTOR3_NUM_FLOATS * mesh.Normals.size();
                    m_jsNormals.Reset();
                    m_jsNormals = Napi::Persistent(Napi::Float32Array::New(info.Env(), m_numJsNormals));
                    for (size_t n = 0; n < mesh.Normals.size(); n++)
                    {
                        m_jsNormals.Value()[VECTOR3_NUM_FLOATS * n] = mesh.Normals.at(n).X;
                        m_jsNormals.Value()[VECTOR3_NUM_FLOATS * n + 1] = mesh.Normals.at(n).Y;
                        m_jsNormals.Value()[VECTOR3_NUM_FLOATS * n + 2] = mesh.Normals.at(n).Z;
                    }
                    m_lastNormalsUpdatedTimestamp = m_lastUpdatedTimestamp;
                }
                else if (m_lastNormalsUpdatedTimestamp != m_lastUpdatedTimestamp)
                {
                    for (size_t n = 0; n < mesh.Normals.size(); n++)
                    {
                        m_jsNormals.Value()[VECTOR3_NUM_FLOATS * n] = mesh.Normals.at(n).X;
                        m_jsNormals.Value()[VECTOR3_NUM_FLOATS * n + 1] = mesh.Normals.at(n).Y;
                        m_jsNormals.Value()[VECTOR3_NUM_FLOATS * n + 2] = mesh.Normals.at(n).Z;
                    }
                    m_lastNormalsUpdatedTimestamp = m_lastUpdatedTimestamp;
                }

                return m_jsNormals.Value();
            }

            Napi::Value GetLastChangedTime(const Napi::CallbackInfo& info)
            {
                return Napi::Value::From(info.Env(), m_lastUpdatedTimestamp);
            }

            Napi::Value GetParentSceneObject(const Napi::CallbackInfo& info);

            Napi::ObjectReference m_jsThis;
            xr::Space m_meshSpace{};
            const Napi::External<xr::Space> m_jsMeshSpace;

            // The last timestamp when this frame was updated (Pulled in from RequestAnimationFrame).
            uint32_t m_lastUpdatedTimestamp{0};
            uint32_t m_lastPositionsUpdatedTimestamp{0};
            uint32_t m_lastIndicesUpdatedTimestamp{0};
            uint32_t m_lastNormalsUpdatedTimestamp{0};

            size_t m_numJsPositions{0};
            Napi::Reference<Napi::Float32Array> m_jsPositions{};
            size_t m_numJsIndices{0};
            Napi::Reference<Napi::Uint32Array> m_jsIndices{};
            size_t m_numJsNormals{0};
            Napi::Reference<Napi::Float32Array> m_jsNormals{};

            // The underlying native mesh.
            xr::System::Session::Frame::Mesh::Identifier m_nativeMeshID{};

            // Pointer to the XRFrame object.
            Plugins::XRFrame* m_frame{};
        };
    } // Plugins
} // Babylon
