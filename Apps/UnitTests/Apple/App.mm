#include "../Shared/Tests.cpp"

int main() {
    Babylon::Graphics::ContextConfiguration graphicsConfig{};
    return Run(Babylon::Graphics::Device::Create(graphicsConfig));
}
