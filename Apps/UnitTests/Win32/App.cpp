#include "../Shared/Tests.cpp"

int main() {
    Babylon::ContextConfiguration graphicsConfig{};
    return Run(Babylon::Graphics::CreateGraphics(graphicsConfig));
}
