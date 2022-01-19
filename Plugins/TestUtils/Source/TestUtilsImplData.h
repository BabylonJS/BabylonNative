#pragma once
#include <Babylon/Graphics/Device.h>
#include "TestUtils.h"

namespace Babylon::Plugins::Internal
{
    class TestUtils::ImplData final : public std::enable_shared_from_this<TestUtils::ImplData>
    {
    public:
<<<<<<< HEAD
        ImplData(Graphics::WindowType nativeWindowPtr)
            : m_nativeWindowPtr{nativeWindowPtr}
        {
        }

        Graphics::WindowType m_nativeWindowPtr{};
=======
        ImplData(WindowType window)
            : m_window{window}
        {
        }

        WindowType m_window{};
>>>>>>> master
    };

} // namespace
