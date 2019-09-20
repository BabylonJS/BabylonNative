#pragma once

#include <napi/napi.h>

#include <string>
#include <vector>

namespace babylon
{
    template<typename ImplT>
    struct NapiBridge final : public Napi::ObjectWrap<NapiBridge<ImplT>>
    {
        class Definition
        {
            friend NapiBridge<ImplT>;

        public:
            Definition(const Definition&) = delete;
            Definition& operator=(const Definition&) = delete;

            template<void(ImplT::*method)(const Napi::CallbackInfo&)>
            Definition& AddMethod(const char* name)
            {
                m_propertyDescriptors.push_back(Napi::ObjectWrap<NapiBridge<ImplT>>::InstanceMethod(name, &NapiBridge<ImplT>::InstanceMethodImpl<method>, napi_default, m_impl));
                return *this;
            }

            template<Napi::Value(ImplT::*method)(const Napi::CallbackInfo&)>
            Definition& AddMethod(const char* name)
            {
                m_propertyDescriptors.push_back(Napi::ObjectWrap<NapiBridge<ImplT>>::InstanceMethod(name, &NapiBridge<ImplT>::InstanceMethodImpl<method>, napi_default, m_impl));
                return *this;
            }

            Napi::Function Finalize()
            {
                return NapiBridge<ImplT>::DefineClass(m_env, m_name.c_str(), m_propertyDescriptors, m_impl);
            }

        private:
            Definition(const std::string& name, Napi::Env& env, ImplT* impl)
                : m_name{ name }
                , m_env{ env }
                , m_impl{ impl }
            {
            }

            const std::string m_name;
            Napi::Env& m_env;
            ImplT* m_impl{};
            std::vector<Napi::ClassPropertyDescriptor<NapiBridge<ImplT>>> m_propertyDescriptors;
        };

        static NapiBridge<ImplT>::Definition Define(const std::string& name, Napi::Env& env, ImplT* impl)
        {
            return NapiBridge<ImplT>::Definition{ name, env, impl };
        }

        explicit NapiBridge(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<NapiBridge<ImplT>>{ info }
        {}

        template<void (ImplT::*method)(const Napi::CallbackInfo& info)>
        inline void InstanceMethodImpl(const Napi::CallbackInfo& info)
        {
            auto impl = reinterpret_cast<ImplT*>(info.Data());
            (impl->*method)(info);
        }

        template<Napi::Value(ImplT::*method)(const Napi::CallbackInfo& info)>
        inline Napi::Value InstanceMethodImpl(const Napi::CallbackInfo& info)
        {
            auto impl = reinterpret_cast<ImplT*>(info.Data());
            return (impl->*method)(info);
        }
    };

    template<typename NativeEngineT>
    struct NativeEngineDefiner
    {
        static void Define(Napi::Env& env, NativeEngineT* impl)
        {
            Napi::HandleScope scope{ env };
            auto func = NapiBridge<NativeEngineT>::Define("NativeEngine", env, impl)
                .template AddMethod<&NativeEngineT::RequestAnimationFrame>("requestAnimationFrame")
                .template AddMethod<&NativeEngineT::CreateVertexArray>("createVertexArray")
                .template AddMethod<&NativeEngineT::DeleteVertexArray>("deleteVertexArray")
                .template AddMethod<&NativeEngineT::BindVertexArray>("bindVertexArray")
                .template AddMethod<&NativeEngineT::CreateIndexBuffer>("createIndexBuffer")
                .template AddMethod<&NativeEngineT::DeleteIndexBuffer>("deleteIndexBuffer")
                .template AddMethod<&NativeEngineT::RecordIndexBuffer>("recordIndexBuffer")
                .template AddMethod<&NativeEngineT::CreateVertexBuffer>("createVertexBuffer")
                .template AddMethod<&NativeEngineT::DeleteVertexBuffer>("deleteVertexBuffer")
                .template AddMethod<&NativeEngineT::RecordVertexBuffer>("recordVertexBuffer")
                .template AddMethod<&NativeEngineT::CreateProgram>("createProgram")
                .template AddMethod<&NativeEngineT::GetUniforms>("getUniforms")
                .template AddMethod<&NativeEngineT::GetAttributes>("getAttributes")
                .template AddMethod<&NativeEngineT::SetProgram>("setProgram")
                .template AddMethod<&NativeEngineT::SetState>("setState")
                .template AddMethod<&NativeEngineT::SetZOffset>("setZOffset")
                .template AddMethod<&NativeEngineT::GetZOffset>("getZOffset")
                .template AddMethod<&NativeEngineT::SetDepthTest>("setDepthTest")
                .template AddMethod<&NativeEngineT::GetDepthWrite>("getDepthWrite")
                .template AddMethod<&NativeEngineT::SetDepthWrite>("setDepthWrite")
                .template AddMethod<&NativeEngineT::SetColorWrite>("setColorWrite")
                .template AddMethod<&NativeEngineT::SetBlendMode>("setBlendMode")
                .template AddMethod<&NativeEngineT::SetMatrix>("setMatrix")
                .template AddMethod<&NativeEngineT::SetIntArray>("setIntArray")
                .template AddMethod<&NativeEngineT::SetIntArray2>("setIntArray2")
                .template AddMethod<&NativeEngineT::SetIntArray3>("setIntArray3")
                .template AddMethod<&NativeEngineT::SetIntArray4>("setIntArray4")
                .template AddMethod<&NativeEngineT::SetFloatArray>("setFloatArray")
                .template AddMethod<&NativeEngineT::SetFloatArray2>("setFloatArray2")
                .template AddMethod<&NativeEngineT::SetFloatArray3>("setFloatArray3")
                .template AddMethod<&NativeEngineT::SetFloatArray4>("setFloatArray4")
                .template AddMethod<&NativeEngineT::SetMatrices>("setMatrices")
                .template AddMethod<&NativeEngineT::SetMatrix3x3>("setMatrix3x3")
                .template AddMethod<&NativeEngineT::SetMatrix2x2>("setMatrix2x2")
                .template AddMethod<&NativeEngineT::SetFloat>("setFloat")
                .template AddMethod<&NativeEngineT::SetFloat2>("setFloat2")
                .template AddMethod<&NativeEngineT::SetFloat3>("setFloat3")
                .template AddMethod<&NativeEngineT::SetFloat4>("setFloat4")
                .template AddMethod<&NativeEngineT::SetBool>("setBool")
                .template AddMethod<&NativeEngineT::CreateTexture>("createTexture")
                .template AddMethod<&NativeEngineT::LoadTexture>("loadTexture")
                .template AddMethod<&NativeEngineT::LoadCubeTexture>("loadCubeTexture")
                .template AddMethod<&NativeEngineT::GetTextureWidth>("getTextureWidth")
                .template AddMethod<&NativeEngineT::GetTextureHeight>("getTextureHeight")
                .template AddMethod<&NativeEngineT::SetTextureSampling>("setTextureSampling")
                .template AddMethod<&NativeEngineT::SetTextureWrapMode>("setTextureWrapMode")
                .template AddMethod<&NativeEngineT::SetTextureAnisotropicLevel>("setTextureAnisotropicLevel")
                .template AddMethod<&NativeEngineT::SetTexture>("setTexture")
                .template AddMethod<&NativeEngineT::DeleteTexture>("deleteTexture")
                .template AddMethod<&NativeEngineT::CreateFrameBuffer>("createFrameBuffer")
                .template AddMethod<&NativeEngineT::DrawIndexed>("drawIndexed")
                .template AddMethod<&NativeEngineT::BindFrameBuffer>("bindFrameBuffer")
                .template AddMethod<&NativeEngineT::UnbindFrameBuffer>("unbindFrameBuffer")
                .template AddMethod<&NativeEngineT::Draw>("draw")
                .template AddMethod<&NativeEngineT::Clear>("clear")
                .template AddMethod<&NativeEngineT::GetRenderWidth>("getRenderWidth")
                .template AddMethod<&NativeEngineT::GetRenderHeight>("getRenderHeight")
                .Finalize();

            env.Global().Set("nativeEngine", func.New({}));
        }
    };
}
