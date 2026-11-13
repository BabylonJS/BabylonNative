#include "App.h"
#include <Babylon/DebugTrace.h>
#include <Windows.h>
#include <cstdio>

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

std::filesystem::path GetExecutableDirectory()
{
    wchar_t buffer[MAX_PATH];
    const DWORD length = GetModuleFileNameW(nullptr, buffer, MAX_PATH);
    if (length == 0 || length == MAX_PATH)
    {
        throw std::runtime_error{"GetModuleFileNameW failed"};
    }
    return std::filesystem::path{buffer}.parent_path();
}

bool ResizeWindowClientArea(Babylon::Graphics::WindowT window, size_t width, size_t height)
{
    RECT rect{0, 0, static_cast<LONG>(width), static_cast<LONG>(height)};
    if (!::AdjustWindowRect(&rect, static_cast<DWORD>(::GetWindowLongPtr(window, GWL_STYLE)), ::GetMenu(window) != NULL))
    {
        std::fprintf(stderr, "AdjustWindowRect failed (error %lu)\n", ::GetLastError());
        return false;
    }

    if (!::SetWindowPos(window, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top,
            SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE))
    {
        std::fprintf(stderr, "SetWindowPos failed (error %lu)\n", ::GetLastError());
        return false;
    }

    return true;
}

int main(int argc, char* argv[])
{
    SetConsoleOutputCP(CP_UTF8);

    // bgfx D3D12 implementation device refcount is not 0 when shutting down. This only happens when no HWND is provided.
    // https://github.com/bkaradzic/bgfx/blob/60c64a3f77aca01fea715fa8ba7a2b1f0ca77f38/src/renderer_d3d12.cpp#L1470
    // Until the fix is done in bgfx, create a window and associate the context with it.
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, "BabylonNative", NULL };
    ::RegisterClassEx(&wc);
    // The window stays hidden, but its client area must match the back buffer: Vulkan swap chains
    // follow the surface extent and cannot be created for a window without a client area.
    constexpr LONG width = 600;
    constexpr LONG height = 400;
    HWND hWnd = ::CreateWindow(wc.lpszClassName, "BabylonNative", WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT,
        width, height, NULL, NULL, wc.hInstance, NULL);
    if (hWnd == NULL)
    {
        std::fprintf(stderr, "CreateWindow failed (error %lu)\n", ::GetLastError());
        return 1;
    }
    if (!ResizeWindowClientArea(hWnd, width, height))
    {
        return 1;
    }

    Babylon::Graphics::Configuration config{};
    config.Window = hWnd;
    config.Width = width;
    config.Height = height;

    Babylon::DebugTrace::EnableDebugTrace(true);
    Babylon::DebugTrace::SetTraceOutput([](const char* trace) {
        OutputDebugStringA(trace);
        OutputDebugStringA("\n");
    });

    return RunTests(config, argc, argv);
}
