#include "WindowManager.h"
#include "../../external/IconsFontAwesome6.h"
#include "../../external/imgui/backends/imgui_impl_glfw.h"
#include "../../external/imgui/backends/imgui_impl_opengl3.h"
#include "../../external/imgui/imgui.h"
#include "../utils/ResourceManager.h"
#include "Theme.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../../external/stb_image.h"

#include <filesystem>
#include <iostream>

namespace framework {

static void glfw_error_callback(int error, const char *description) {
  std::cerr << "GLFW Error " << error << ": " << description << "\n";
}

WindowManager &WindowManager::instance() {
  static WindowManager instance;
  return instance;
}

bool WindowManager::initialize(IClientApp *clientApp) {
  if (!clientApp) {
    std::cerr << "Error: ClientApp is null\n";
    return false;
  }

  clientApp_ = clientApp;

  // Get client configuration
  ClientAppConfig config = clientApp_->getConfig();

  // Initialize GLFW and create window
  if (!initGLFW(config)) {
    return false;
  }

  // Setup ImGui
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  // Load fonts
  loadFonts();

  // Apply theme
  Theme::applyGlassmorphism();

  // Setup ImGui backends
  const char *glsl_version = "#version 150";
  ImGui_ImplGlfw_InitForOpenGL(window_, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  // Load background texture if specified
  if (!config.backgroundImage.empty()) {
    std::string bgPath =
        ResourceManager::instance().getImage(config.backgroundImage);
    if (bgPath.empty()) {
      bgPath = config.backgroundImage; // Try as-is
    }
    loadBackgroundTexture(bgPath);
  }

  // Initialize client app
  clientApp_->onInit();

  lastFrameTime_ = glfwGetTime();

  return true;
}

bool WindowManager::initGLFW(const ClientAppConfig &config) {
  glfwSetErrorCallback(glfw_error_callback);

  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW\n";
    return false;
  }

  // OpenGL 3.3 Core Profile
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // macOS

  // Create window
  window_ = glfwCreateWindow(config.initialWidth, config.initialHeight,
                             config.appTitle.c_str(), nullptr, nullptr);

  if (!window_) {
    std::cerr << "Failed to create GLFW window\n";
    glfwTerminate();
    return false;
  }

  glfwMakeContextCurrent(window_);
  glfwSwapInterval(1); // VSync

  return true;
}

void WindowManager::loadFonts() {
  ImGuiIO &io = ImGui::GetIO();
  const float fontSize = 22.0f;

  // Try to load system font (macOS)
  ImFont *font = nullptr;
  if (std::filesystem::exists("/System/Library/Fonts/SFNS.ttf")) {
    font = io.Fonts->AddFontFromFileTTF("/System/Library/Fonts/SFNS.ttf",
                                        fontSize);
  } else if (std::filesystem::exists("/System/Library/Fonts/Helvetica.ttc")) {
    font = io.Fonts->AddFontFromFileTTF("/System/Library/Fonts/Helvetica.ttc",
                                        fontSize);
  } else {
    font = io.Fonts->AddFontDefault();
  }

  // Merge Font Awesome icons
  static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
  ImFontConfig icons_config;
  icons_config.MergeMode = true;
  icons_config.PixelSnapH = true;
  icons_config.GlyphMinAdvanceX = fontSize;

  std::string faPath = ResourceManager::instance().getFont("fa-solid-900");
  if (!faPath.empty()) {
    io.Fonts->AddFontFromFileTTF(faPath.c_str(), fontSize * 0.9f, &icons_config,
                                 icons_ranges);
  } else {
    std::cerr << "Warning: Could not find fa-solid-900.ttf\n";
  }
}

void WindowManager::loadBackgroundTexture(const std::string &path) {
  if (path.empty())
    return;

  int width = 0, height = 0;
  unsigned char *image_data = stbi_load(path.c_str(), &width, &height, NULL, 4);

  if (!image_data) {
    std::cerr << "Warning: Failed to load background image: " << path << "\n";
    return;
  }

  // Create OpenGL texture
  glGenTextures(1, &bgTexture_);
  glBindTexture(GL_TEXTURE_2D, bgTexture_);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, image_data);

  stbi_image_free(image_data);

  bgWidth_ = width;
  bgHeight_ = height;
}

void WindowManager::renderBackground() {
  if (bgTexture_ == 0)
    return;

  // Ken Burns effect - slow zoom and pan
  float t = (float)glfwGetTime();
  float zoom = 0.15f; // 15% zoom

  float s = 1.0f / (1.0f + zoom * (0.5f + 0.5f * sinf(t * 0.05f)));
  float ox = (1.0f - s) * (0.5f + 0.5f * cosf(t * 0.07f));
  float oy = (1.0f - s) * (0.5f + 0.5f * sinf(t * 0.03f));

  ImGui::GetBackgroundDrawList()->AddImage(
      (void *)(intptr_t)bgTexture_, ImVec2(0, 0), ImGui::GetIO().DisplaySize,
      ImVec2(ox, oy), ImVec2(ox + s, oy + s));
}

void WindowManager::run() {
  if (!window_ || !clientApp_) {
    std::cerr << "Error: WindowManager not initialized\n";
    return;
  }

  while (!shouldClose()) {
    beginFrame();
    clientApp_->onUpdate(deltaTime_);
    clientApp_->onRenderUI();
    endFrame();
  }
}

void WindowManager::beginFrame() {
  glfwPollEvents();

  // Calculate delta time
  double currentTime = glfwGetTime();
  deltaTime_ = static_cast<float>(currentTime - lastFrameTime_);
  lastFrameTime_ = currentTime;

  // Start ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  // Render background with Ken Burns effect
  renderBackground();
}

void WindowManager::endFrame() {
  // Finalize ImGui
  ImGui::Render();

  // OpenGL rendering
  int display_w, display_h;
  glfwGetFramebufferSize(window_, &display_w, &display_h);
  glViewport(0, 0, display_w, display_h);
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  glfwSwapBuffers(window_);
}

bool WindowManager::shouldClose() const {
  return window_ && glfwWindowShouldClose(window_);
}

void WindowManager::shutdown() {
  if (clientApp_) {
    clientApp_->onShutdown();
    clientApp_ = nullptr;
  }

  // Cleanup ImGui
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  // Cleanup GLFW
  if (window_) {
    glfwDestroyWindow(window_);
    window_ = nullptr;
  }
  glfwTerminate();

  // Cleanup texture
  if (bgTexture_) {
    glDeleteTextures(1, &bgTexture_);
    bgTexture_ = 0;
  }
}

} // namespace framework
