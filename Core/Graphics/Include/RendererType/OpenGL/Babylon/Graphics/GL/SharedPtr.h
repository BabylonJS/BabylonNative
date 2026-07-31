#pragma once

#include <utility>

namespace Babylon::Graphics::GL
{
    // Holds one reference to a reference-counted OpenGL object and removes it on destruction.
    // T must provide AddReference() and RemoveReference(); see GL::Texture.
    //
    // D3D and Metal objects get an equivalent holder from their platform SDKs, but OpenGL has no
    // ecosystem to borrow one from, so this is it. A raw T* stays copyable and reference-neutral,
    // which is why taking a new reference and taking over an existing one are spelled as distinct
    // operations rather than as a constructor.
    template<typename T>
    class SharedPtr final
    {
    public:
        SharedPtr() noexcept = default;
        ~SharedPtr() { Reset(); }

        SharedPtr(const SharedPtr& other) noexcept
            : m_ptr{other.m_ptr}
        {
            if (m_ptr != nullptr)
            {
                m_ptr->AddReference();
            }
        }

        SharedPtr(SharedPtr&& other) noexcept
            : m_ptr{std::exchange(other.m_ptr, nullptr)}
        {
        }

        SharedPtr& operator=(const SharedPtr& other) noexcept
        {
            SharedPtr{other}.Swap(*this);
            return *this;
        }

        SharedPtr& operator=(SharedPtr&& other) noexcept
        {
            SharedPtr{std::move(other)}.Swap(*this);
            return *this;
        }

        // Takes a reference of our own, leaving the caller's reference intact.
        void CopyFrom(T* ptr) noexcept
        {
            // Add the new reference before removing the one we already hold, so copying from
            // what we are already holding cannot destroy it.
            if (ptr != nullptr)
            {
                ptr->AddReference();
            }
            Reset();
            m_ptr = ptr;
        }

        // Takes over the reference the caller already holds (e.g. straight out of Create).
        void Attach(T* ptr) noexcept
        {
            Reset();
            m_ptr = ptr;
        }

        // Hands our reference back to the caller, who becomes responsible for removing it.
        T* Detach() noexcept { return std::exchange(m_ptr, nullptr); }

        T* Get() const noexcept { return m_ptr; }
        T* operator->() const noexcept { return m_ptr; }
        explicit operator bool() const noexcept { return m_ptr != nullptr; }

        void Reset() noexcept
        {
            if (m_ptr != nullptr)
            {
                m_ptr->RemoveReference();
                m_ptr = nullptr;
            }
        }

        void Swap(SharedPtr& other) noexcept { std::swap(m_ptr, other.m_ptr); }

    private:
        T* m_ptr{};
    };
}
