#pragma once

#include "../core/IClientApp.h"
#include <GLFW/glfw3.h>
#include <string>

namespace framework {

/// Window manager - owns GLFW window and main rendering loop
/// This is the "engine" that drives the application
class WindowManager {
public:
  static WindowManager &instance();

  /// Initialize GLFW, create window, setup ImGui
  /// @param clientApp Pointer to client application (must remain valid)
  /// @return true if initialization succeeded
  bool initialize(IClientApp *clientApp);

  /// Run the main loop - calls client's onUpdate/onRenderUI
  /// Blocks until window is closed
  void run();

  /// Start a new frame (Poll events, calc delta, NewFrame)
  void beginFrame();

  /// End the frame (Render, SwapBuffers)
  void endFrame();

  /// Shutdown and cleanup
  void shutdown();

  /// Get GLFW window handle
  GLFWwindow *getWindow() const { return window_; }

  /// Get delta time since last frame
  float getDeltaTime() const { return deltaTime_; }

  /// Check if window should close
  bool shouldClose() const;

private:
  WindowManager() = default;
  ~WindowManager() = default;

  // Disable copy/move
  WindowManager(const WindowManager &) = delete;
  WindowManager &operator=(const WindowManager &) = delete;

  bool initGLFW(const ClientAppConfig &config);
  void loadFonts();
  void loadBackgroundTexture(const std::string &path);
  void renderBackground();
  void renderFrame();

  GLFWwindow *window_ = nullptr;
  IClientApp *clientApp_ = nullptr;
  GLuint bgTexture_ = 0;
  int bgWidth_ = 0;
  int bgHeight_ = 0;
  float deltaTime_ = 0.0f;
  double lastFrameTime_ = 0.0;
};

} // namespace framework
