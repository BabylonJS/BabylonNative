#include "../Shared/Tests.h"

int main() {
    Babylon::Graphics::DeviceConfiguration graphicsConfig{};
    return Run(Babylon::Graphics::Device::Create(graphicsConfig));
}
