#include "../Shared/Tests.cpp"

int main() {
    Babylon::Graphics::DeviceConfiguration graphicsConfig{};
    return Run(Babylon::Graphics::Device::Create(graphicsConfig));
}
