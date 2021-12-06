#include "../Shared/Tests.cpp"

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

int main() {
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, "BabylonNative", NULL };
    ::RegisterClassEx(&wc);
    HWND hwnd = ::CreateWindow(wc.lpszClassName, "BabylonNative", WS_OVERLAPPEDWINDOW, -1, -1, -1, -1, NULL, NULL, wc.hInstance, NULL);

    Babylon::WindowConfiguration windowConfig{};
    windowConfig.WindowPtr = hwnd;
    windowConfig.Width = 600;
    windowConfig.Height = 400;
    return Run(Babylon::Graphics::CreateGraphics(windowConfig));
}
