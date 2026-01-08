#include "framework/core/IClientApp.h"
#include "framework/gui/WindowManager.h"
#include "framework/utils/ResourceManager.h"
#include <iostream>

namespace framework {

// Client app factory - must be defined by user
extern IClientApp *createClientApp();

/// Main application entry point
/// Call this from your main() function
int runApplication(int argc, char *argv[]) {
  // Setup resource paths
  ResourceManager::instance().setExecutablePath(argv[0]);

  // Create client app
  IClientApp *app = createClientApp();
  if (!app) {
    std::cerr << "Error: createClientApp() returned null\n";
    return 1;
  }

  // Initialize window manager
  if (!WindowManager::instance().initialize(app)) {
    delete app;
    return 1;
  }

  // Run main loop
  WindowManager::instance().run();

  // Cleanup
  WindowManager::instance().shutdown();
  delete app;

  return 0;
}

} // namespace framework

// Standard main - links to framework
int main(int argc, char *argv[]) {
  return framework::runApplication(argc, argv);
}
