#include "NativeCamera.h"
#include <stdexcept>

namespace Babylon::Plugins::Internal
{
    CameraInterface* CameraInterface::CreateInterface(uint32_t /*width*/, uint32_t /*height*/, bool /*frontCamera*/)
    {
        throw std::runtime_error{"Camera unimplemented on Win32"};
    }
}