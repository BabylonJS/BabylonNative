#pragma once
#include <Babylon/Graphics/Device.h>
#include "TestUtils.h"

namespace Babylon::Plugins::Internal
{
    class TestUtils::ImplData final : public std::enable_shared_from_this<TestUtils::ImplData>
    {
    public:
        ImplData(Graphics::WindowType nativeWindowPtr)
            : m_nativeWindowPtr{nativeWindowPtr}
        {
        }

        Graphics::WindowType m_nativeWindowPtr{};
    };

} // namespace
