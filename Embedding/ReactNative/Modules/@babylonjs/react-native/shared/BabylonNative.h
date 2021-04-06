#pragma once

#include <jsi/jsi.h>

namespace Babylon
{
    using Dispatcher = std::function<void(std::function<void()>)>;

    void Initialize(facebook::jsi::Runtime& jsiRuntime, Dispatcher jsDispatcher);
    void Deinitialize();
    void UpdateView(void* windowPtr, size_t width, size_t height);
    void RenderView();
    void ResetView();
    void SetMouseButtonState(uint32_t buttonId, bool isDown, uint32_t x, uint32_t y);
    void SetMousePosition(uint32_t x, uint32_t y);
    void SetTouchButtonState(uint32_t pointerId, bool isDown, uint32_t x, uint32_t y);
    void SetTouchPosition(uint32_t pointerId, uint32_t x, uint32_t y);
    bool IsXRActive();
    void UpdateXRView(void* windowPtr);

    extern const uint32_t LEFT_MOUSE_BUTTON_ID;
    extern const uint32_t MIDDLE_MOUSE_BUTTON_ID;
    extern const uint32_t RIGHT_MOUSE_BUTTON_ID;
}
