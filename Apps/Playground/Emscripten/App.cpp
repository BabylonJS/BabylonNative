#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/Plugins/NativeInput.h>
#include <Babylon/Plugins/NativeOptimizations.h>
#include <Babylon/ScriptLoader.h>
#include <GLFW/glfw3.h>
#include <emscripten.h>

#include <optional>

static const char *s_applicationName = "BabylonNative Playground";
std::optional<Babylon::Graphics::Device> device{};
std::optional<Babylon::Graphics::DeviceUpdate> update{};
std::optional<Babylon::AppRuntime> runtime{};
Babylon::Plugins::NativeInput *nativeInput{};

namespace {

void Uninitialize() {
  if (device) {
    update->Finish();
    device->FinishRenderingCurrentFrame();
  }

  nativeInput = {};
  runtime.reset();
  update.reset();
  device.reset();
}

void mouse_button_callback(GLFWwindow *window, int button, int action,
                           int mods) {
  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);
  int32_t x = static_cast<int32_t>(xpos);
  int32_t y = static_cast<int32_t>(ypos);

  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    nativeInput->MouseDown(Babylon::Plugins::NativeInput::LEFT_MOUSE_BUTTON_ID,
                           x, y);
  else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    nativeInput->MouseUp(Babylon::Plugins::NativeInput::LEFT_MOUSE_BUTTON_ID, x,
                         y);
  else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    nativeInput->MouseDown(Babylon::Plugins::NativeInput::RIGHT_MOUSE_BUTTON_ID,
                           x, y);
  else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    nativeInput->MouseUp(Babylon::Plugins::NativeInput::RIGHT_MOUSE_BUTTON_ID,
                         x, y);
  else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
    nativeInput->MouseDown(
        Babylon::Plugins::NativeInput::MIDDLE_MOUSE_BUTTON_ID, x, y);
  else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
    nativeInput->MouseUp(Babylon::Plugins::NativeInput::MIDDLE_MOUSE_BUTTON_ID,
                         x, y);
}

static void cursor_position_callback(GLFWwindow *window, double xpos,
                                     double ypos) {
  int32_t x = static_cast<int32_t>(xpos);
  int32_t y = static_cast<int32_t>(ypos);

  nativeInput->MouseMove(x, y);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  nativeInput->MouseWheel(Babylon::Plugins::NativeInput::MOUSEWHEEL_Y_ID,
                          static_cast<int>(-yoffset * 100.0));
}

static void window_resize_callback(GLFWwindow *window, int width, int height) {
  device->UpdateSize(width, height);
}
GLFWwindow *init_glfw(int width, int height) {
  if (!glfwInit()) {
    return 0;
  }

  glfwDefaultWindowHints();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);

  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLFWwindow *window = glfwCreateWindow(width, height, "BabylonNative test app",
                                        nullptr, nullptr);

  if (!window) {
    return 0;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);

  int w, h;
  glfwGetFramebufferSize(window, &w, &h);
  glViewport(0, 0, w, h);

  //	glfwSetKeyCallback(window, key_callback);
  glfwSetWindowSizeCallback(window, window_resize_callback);
  glfwSetCursorPosCallback(window, cursor_position_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetScrollCallback(window, scroll_callback);

  return window;
}

void InitBabylon(int width, int height) {

  Uninitialize();

  Babylon::Graphics::Configuration graphicsConfig{};
  graphicsConfig.Window = (Babylon::Graphics::WindowT) "#canvas";
  graphicsConfig.Width = static_cast<size_t>(width);
  graphicsConfig.Height = static_cast<size_t>(height);
  graphicsConfig.MSAASamples = 4;

  device.emplace(graphicsConfig);
  update.emplace(device->GetUpdate("update"));
  device->StartRenderingCurrentFrame();
  update->Start();

  runtime.emplace();

  runtime->Dispatch([](Napi::Env env) {
    // Initialize NativeEngine plugin.
    device->AddToJavaScript(env);
    Babylon::Plugins::NativeEngine::Initialize(env);

    Babylon::Plugins::NativeOptimizations::Initialize(env);

    nativeInput = &Babylon::Plugins::NativeInput::CreateForJavaScript(env);
  });

  // Load Babylon.js scripts
  std::string moduleRootUrl = "./Scripts/";
  Babylon::ScriptLoader loader{*runtime};
  loader.LoadScript(moduleRootUrl + "ammo.js");
  loader.LoadScript(moduleRootUrl + "recast.js");
  loader.LoadScript(moduleRootUrl + "babylon.max.js");
  loader.LoadScript(moduleRootUrl + "babylonjs.loaders.js");
  loader.LoadScript(moduleRootUrl + "babylonjs.materials.js");
  loader.LoadScript(moduleRootUrl + "babylon.gui.js");
  loader.LoadScript(moduleRootUrl + "experience.js");
}

void MainLoop(void *window) {
  if (device) {
    update->Finish();
    device->FinishRenderingCurrentFrame();
    device->StartRenderingCurrentFrame();
    update->Start();
  }
  glfwPollEvents();
  glfwSwapBuffers((GLFWwindow *)window);
}
} // namespace

int main(int argc, const char *const *argv) {
  // Initialize canvas and Babylon.js
  int width = 640;
  int height = 480;

  GLFWwindow *window = init_glfw(width, height);
  glfwGetWindowSize(window, &width, &height);
  InitBabylon(width, height);
  emscripten_set_main_loop_arg(MainLoop, window, 0, 1);

  return 0;
}
