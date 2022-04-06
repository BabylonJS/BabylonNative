#pragma once

#include <memory>
#include <utility>

namespace Babylon
{
    namespace Internal
    {
        template<typename...>
        class DispatchableImplBase;

        template<typename ReturnT, typename... ArgsT>
        class DispatchableImplBase<ReturnT(ArgsT...)>
        {
        public:
            virtual ~DispatchableImplBase()
            {
            }

            virtual ReturnT operator()(ArgsT... args) = 0;
        };

        template<typename...>
        class DispatchableImpl;

        template<typename CallableT, typename ReturnT, typename... ArgsT>
        class DispatchableImpl<CallableT, ReturnT(ArgsT...)> : public DispatchableImplBase<ReturnT(ArgsT...)>
        {
        public:
            DispatchableImpl(DispatchableImpl<CallableT, ReturnT(ArgsT...)>&& other) = default;
        
            DispatchableImpl(CallableT&& callable)
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
    class Dispatchable
    {
    public:
        Dispatchable() = default;
        Dispatchable(const Dispatchable&) = delete;
        Dispatchable(Dispatchable&&) = default;

        template<typename CallableT>
        Dispatchable(CallableT&& callable)
            : m_impl{std::make_unique<Internal::DispatchableImpl<CallableT, SignatureT>>(std::forward<CallableT>(callable))}
        {
        }

        template<typename... ArgsT>
        void operator()(ArgsT... args)
        {
            (*m_impl)(args...);
        }

    private:
        std::unique_ptr<Internal::DispatchableImplBase<SignatureT>> m_impl{};
    };
}
