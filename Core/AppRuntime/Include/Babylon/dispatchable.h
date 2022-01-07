#pragma once

#include <memory>
#include <utility>

namespace Babylon
{
    namespace internal
    {
        template<typename...>
        class dispatchable_impl_base;

        template<typename ReturnT, typename... ArgsT>
        class dispatchable_impl_base<ReturnT(ArgsT...)>
        {
        public:
            virtual ~dispatchable_impl_base()
            {
            }

            virtual ReturnT operator()(ArgsT... args) = 0;
        };

        template<typename...>
        class dispatchable_impl;

        template<typename CallableT, typename ReturnT, typename... ArgsT>
        class dispatchable_impl<CallableT, ReturnT(ArgsT...)> : public dispatchable_impl_base<ReturnT(ArgsT...)>
        {
        public:
            dispatchable_impl(dispatchable_impl<CallableT, ReturnT(ArgsT...)>&& other) = default;
        
            dispatchable_impl(CallableT&& callable)
                : m_callable{std::forward<CallableT>(callable)}
            {
            }

            ReturnT operator()(ArgsT... args) override
            {
                return m_callable(args...);
            }

        private:
            CallableT m_callable;
        };
    }

    template<typename SignatureT>
    class dispatchable
    {
    public:
        dispatchable() = default;
        dispatchable(const dispatchable&) = delete;
        dispatchable(dispatchable&&) = default;

        template<typename CallableT>
        dispatchable(CallableT&& callable)
            : m_impl{std::make_unique<internal::dispatchable_impl<CallableT, SignatureT>>(std::forward<CallableT>(callable))}
        {
        }

        template<typename... ArgsT>
        void operator()(ArgsT... args)
        {
            (*m_impl)(args...);
        }

    private:
        std::unique_ptr<internal::dispatchable_impl_base<SignatureT>> m_impl{};
    };
}
