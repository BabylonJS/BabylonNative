#include "../Shared/Tests.cpp"

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

int main() {
    // bgfx D3D12 implementation device refcount is not 0 when shutting down. This only happens when no HWND is provided.
    // https://github.com/bkaradzic/bgfx/blob/60c64a3f77aca01fea715fa8ba7a2b1f0ca77f38/src/renderer_d3d12.cpp#L1470
    // Until the fix is done in bgfx, create a window and associate the context with it.
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, "BabylonNative", NULL };
    ::RegisterClassEx(&wc);
    HWND hwnd = ::CreateWindow(wc.lpszClassName, "BabylonNative", WS_OVERLAPPEDWINDOW, -1, -1, -1, -1, NULL, NULL, wc.hInstance, NULL);

    Babylon::WindowConfiguration windowConfig{};
    windowConfig.Window = hwnd;
    windowConfig.Width = 600;
    windowConfig.Height = 400;
    return Run(Babylon::Graphics::CreateGraphics(windowConfig));
}
