#pragma once

#include <memory>
#include <unordered_map>

namespace Babylon
{
    namespace
    {
        static inline constexpr uint64_t JAVASCRIPT_MAX_SAFE_INTEGER = 9007199254740991; // (2 ^ 53) - 1
    }

    template<typename T>
    class ResourceTable final
    {
    public:
        using handle = uint64_t;

        handle Add(T resource)
        {
            const auto resourceHandle{m_nextResourceId++};
            if (resourceHandle > JAVASCRIPT_MAX_SAFE_INTEGER)
            {
                throw std::runtime_error{"ResourceTable handle overflow."};
            }

            m_resources.insert({resourceHandle, std::move(resource)});
            return resourceHandle;
        }

        T& Get(handle resourceHandle)
        {
            return m_resources.at(resourceHandle);
        }

        void Remove(handle resourceHandle)
        {
            m_resources.erase(resourceHandle);
        }

    private:
        handle m_nextResourceId{1};
        std::unordered_map<handle, T> m_resources{};
    };

    template<typename T>
    struct NativeResource
    {
    public:
        using resource_table_handle = typename ResourceTable<T>::handle;

        static T& Get(resource_table_handle handle)
        {
            return *s_resources.Get(handle);
        }

        template<typename... Args>
        static resource_table_handle Create(Args&&... args)
        {
            return s_resources.Add(std::make_unique<T>(std::forward<Args>(args)...));
        }

        static void Delete(resource_table_handle handle)
        {
            s_resources.Remove(handle);
        }

    private:
        inline static ResourceTable<std::unique_ptr<T>> s_resources{};
    };
}
