#pragma once

#include <memory>

namespace babylon
{
    // Attempts to store the instance, system, and related non-session state for an HMD.
    class HeadMountedDisplay
    {
        struct Session
        {
        private:
            struct Impl;
            std::unique_ptr<Impl> m_impl{};
        };

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl{};
    };
}