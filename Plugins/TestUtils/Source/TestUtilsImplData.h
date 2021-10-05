#pragma once
#include <Babylon/Graphics.h>
#include "TestUtils.h"

namespace Babylon::Plugins::Internal
{
    class TestUtils::ImplData final : public std::enable_shared_from_this<TestUtils::ImplData>
    {
    public:
        TestUtils::ImplData(WindowType nativeWindowPtr)
            : m_nativeWindowPtr{nativeWindowPtr}
        {
        }

        WindowType m_nativeWindowPtr{};
    };

} // namespace