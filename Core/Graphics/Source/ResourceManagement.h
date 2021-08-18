#pragma once

#include <memory>
#include <unordered_map>

namespace Babylon
{
    template<typename T>
    class ResourceTable final
    {
    public:
        uint32_t Add(T resource)
        {
            const uint32_t resourceHandle{m_nextResourceId};
            m_resources.insert({resourceHandle, std::move(resource)});
            m_nextResourceId++;
            return resourceHandle;
        }

        T& Get(uint32_t resourceHandle)
        {
            return m_resources.at(resourceHandle);
        }

        void Remove(uint32_t resourceHandle)
        {
            m_resources.erase(resourceHandle);
        }

    private:
        uint32_t m_nextResourceId{1};
        std::unordered_map<uint32_t, T> m_resources{};
    };

    template<typename T>
    struct NativeResource
    {
    public:
        static T& Get(uint32_t handle)
        {
            return *s_resources.Get(handle);
        }

        template<typename... Args>
        static uint32_t Create(Args&&... args)
        {
            return s_resources.Add(std::make_unique<T>(std::forward<Args>(args)...));
        }

        static void Delete(uint32_t handle)
        {
            s_resources.Remove(handle);
        }

    private:
        inline static ResourceTable<std::unique_ptr<T>> s_resources{};
    };
}
