#include "../Shared/Tests.cpp"

int main() {
    Babylon::ContextConfiguration graphicsConfig{};
    return run(Babylon::Graphics::CreateGraphics(graphicsConfig));
}
