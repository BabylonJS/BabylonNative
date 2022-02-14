#pragma once
#include <Babylon/Graphics.h>
#include "TestUtils.h"

namespace Babylon::Plugins::Internal
{
    class TestUtils::ImplData final : public std::enable_shared_from_this<TestUtils::ImplData>
    {
    public:
        ImplData(WindowType window)
            : m_window{window}
        {
        }

        WindowType m_window{};
    };

} // namespace
