#pragma once

#include <memory>
#include <vector>
#include <unordered_map>

namespace Babylon
{
    template<typename T>
    class ResourceTable final
    {
    public:
        uint32_t Add(T resource)
        {
            const auto resourceHandle{m_freeHandles.empty() ? (m_nextResourceId++) : m_freeHandles.back()};
            if (!m_freeHandles.empty())
            {
                m_freeHandles.pop_back();
            }
            if (resourceHandle == 0)
            {
                throw std::runtime_error{"ResourceTable handle overflow."};
            }

            m_resources.insert({resourceHandle, std::move(resource)});
            return resourceHandle;
        }

        T& Get(uint32_t resourceHandle)
        {
            return m_resources.at(resourceHandle);
        }

        void Remove(uint32_t resourceHandle)
        {
            m_resources.erase(resourceHandle);
            m_freeHandles.push_back(resourceHandle);
        }

    private:
        uint32_t m_nextResourceId{1};
        std::unordered_map<uint32_t, T> m_resources{};
        std::vector<uint32_t> m_freeHandles{};
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
