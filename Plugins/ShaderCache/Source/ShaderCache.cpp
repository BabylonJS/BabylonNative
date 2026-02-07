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

    uint32_t Save(std::ofstream& stream)
    {
        if (!ShaderCacheImpl::Instance)
        {
            throw std::runtime_error("ShaderCache is not enabled.");
        }

        return ShaderCacheImpl::Instance->Save(stream);
    }

    uint32_t Load(std::ifstream& stream)
    {
        if (!ShaderCacheImpl::Instance)
        {
            throw std::runtime_error("ShaderCache is not enabled.");
        }

        return ShaderCacheImpl::Instance->Load(stream);
    }

    std::shared_ptr<Graphics::BgfxShaderInfo> AddShader(std::string_view vertexSource, std::string_view fragmentSource, Graphics::BgfxShaderInfo shaderInfo)
    {
        if (!ShaderCacheImpl::Instance)
        {
            throw std::runtime_error("ShaderCache is not enabled.");
        }

        return ShaderCacheImpl::Instance->AddShader(vertexSource, fragmentSource, std::move(shaderInfo));
    }

    std::shared_ptr<Graphics::BgfxShaderInfo> GetShader(std::string_view vertexSource, std::string_view fragmentSource)
    {
        if (!ShaderCacheImpl::Instance)
        {
            throw std::runtime_error("ShaderCache is not enabled.");
        }

        return ShaderCacheImpl::Instance->GetShader(vertexSource, fragmentSource);
    }
}
