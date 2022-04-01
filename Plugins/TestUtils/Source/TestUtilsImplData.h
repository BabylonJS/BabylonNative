#pragma once
#include <Babylon/Graphics/Device.h>
#include "TestUtils.h"

namespace Babylon::Plugins::Internal
{
    class TestUtils::ImplData final : public std::enable_shared_from_this<TestUtils::ImplData>
    {
    public:
        ImplData(Graphics::WindowType window)
            : m_window{window}
        {
        }

        Graphics::WindowType m_window{};
    };

} // namespace
