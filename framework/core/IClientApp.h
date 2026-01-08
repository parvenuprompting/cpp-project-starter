#pragma once

#include <string>

namespace framework {

/// Configuration for client application
struct ClientAppConfig {
  std::string appTitle = "Modern C++ App";
  int initialWidth = 1280;
  int initialHeight = 800;
  std::string backgroundImage = "";
  std::string iconPath = "";
};

/// Interface for client applications
/// Clients implement this to integrate with the framework
class IClientApp {
public:
  virtual ~IClientApp() = default;

  /// Get application configuration
  virtual ClientAppConfig getConfig() = 0;

  /// Called once after window initialization
  virtual void onInit() = 0;

  /// Called every frame for logic updates
  /// @param deltaTime Time since last frame in seconds
  virtual void onUpdate(float deltaTime) = 0;

  /// Called every frame for ImGui rendering
  virtual void onRenderUI() = 0;

  /// Called before shutdown
  virtual void onShutdown() = 0;

  /// Optional: Called when file is dropped onto window
  /// @param path Absolute path to dropped file
  virtual void onFileDropped(const std::string &path) {}

  /// Optional: Called when window is resized
  /// @param width New window width
  /// @param height New window height
  virtual void onWindowResize(int width, int height) {}
};

} // namespace framework
