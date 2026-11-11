#include <Babylon/Plugins/ShaderCache.h>
#include "ShaderCacheImpl.h"

namespace Babylon::Plugins::ShaderCache
{
    void Enable()
    {
        if (!IsEnabled())
        {
            ShaderCacheImpl::Instance = std::make_unique<ShaderCacheImpl>();
        }
    }

    void Disable()
    {
        ShaderCacheImpl::Instance.reset();
    }

    bool IsEnabled()
    {
        return !!ShaderCacheImpl::Instance;
    }

    void Clear()
    {
        if (!ShaderCacheImpl::Instance)
        {
            throw std::runtime_error("ShaderCache is not enabled.");
        }

        ShaderCacheImpl::Instance->Clear();
    }

    uint32_t Save(std::ostream& stream)
    {
        if (!ShaderCacheImpl::Instance)
        {
            throw std::runtime_error("ShaderCache is not enabled.");
        }

        return ShaderCacheImpl::Instance->Save(stream);
    }

    uint32_t Load(std::istream& stream)
    {
        if (!ShaderCacheImpl::Instance)
        {
            throw std::runtime_error("ShaderCache is not enabled.");
        }

        return ShaderCacheImpl::Instance->Load(stream);
    }

    std::shared_ptr<Graphics::BgfxShaderInfo> AddShader(std::string_view vertexSource, std::string_view fragmentSource, Graphics::BgfxShaderInfo shaderInfo,
        const std::map<std::string, uint32_t>& instancedAttributes)
    {
        if (!ShaderCacheImpl::Instance)
        {
            throw std::runtime_error("ShaderCache is not enabled.");
        }

        return ShaderCacheImpl::Instance->AddShader(vertexSource, fragmentSource, std::move(shaderInfo), instancedAttributes);
    }

    std::shared_ptr<Graphics::BgfxShaderInfo> GetShader(std::string_view vertexSource, std::string_view fragmentSource,
        const std::map<std::string, uint32_t>& instancedAttributes)
    {
        if (!ShaderCacheImpl::Instance)
        {
            throw std::runtime_error("ShaderCache is not enabled.");
        }

        return ShaderCacheImpl::Instance->GetShader(vertexSource, fragmentSource, instancedAttributes);
    }

    std::shared_ptr<Graphics::BgfxShaderInfo> AddTransformFeedbackShader(std::string_view vertexSource, const std::vector<std::string>& varyings, Graphics::BgfxShaderInfo shaderInfo)
    {
        if (!ShaderCacheImpl::Instance)
        {
            throw std::runtime_error("ShaderCache is not enabled.");
        }

        return ShaderCacheImpl::Instance->AddTransformFeedbackShader(vertexSource, varyings, std::move(shaderInfo));
    }

    std::shared_ptr<Graphics::BgfxShaderInfo> GetTransformFeedbackShader(std::string_view vertexSource, const std::vector<std::string>& varyings)
    {
        if (!ShaderCacheImpl::Instance)
        {
            throw std::runtime_error("ShaderCache is not enabled.");
        }

        return ShaderCacheImpl::Instance->GetTransformFeedbackShader(vertexSource, varyings);
    }
}
