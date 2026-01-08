#pragma once

#include "../../framework/core/IClientApp.h"
#include "../../framework/io/MemoryMappedFile.h"
#include <atomic>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace demo {

// Represents a single open file tab
struct FileTab {
  std::string path;
  std::string name;
  std::unique_ptr<framework::MemoryMappedFile> file;
  std::vector<size_t> lineOffsets;
  int selectedLine = -1;
  bool isIndexing = false;
  float indexingProgress = 0.0f;
};

// Current page/view in the application
enum class AppPage { Dashboard, Viewer, Settings };

class SimpleViewerApp : public framework::IClientApp {
public:
  SimpleViewerApp();
  ~SimpleViewerApp() override;

  // IClientApp interface
  framework::ClientAppConfig getConfig() override;
  void onInit() override;
  void onUpdate(float deltaTime) override;
  void onRenderUI() override;
  void onShutdown() override;
  void onFileDropped(const std::string &path) override;

private:
  // File operations
  void openFile(const std::string &path);
  void closeTab(size_t index);
  void indexFileAsync(size_t tabIndex, const std::string &path);

  // UI Rendering
  void renderMenuBar();
  void renderSidebar();
  void renderStatusBar();
  void renderDashboardPage();
  void renderViewerPage();
  void renderSettingsPage();
  void renderAboutDialog();
  void renderFilePicker();

  // State
  AppPage currentPage_ = AppPage::Dashboard;
  std::vector<FileTab> tabs_;
  size_t activeTabIndex_ = 0;

  std::mutex viewerMutex_;
  std::thread indexerThread_;

  // UI state
  bool showFilePicker_ = false;
  bool showAbout_ = false;
  char searchBuffer_[256] = "";

  // Settings
  float fontSize_ = 22.0f;
  bool wordWrap_ = false;
  bool showLineNumbers_ = true;
  int recentFilesLimit_ = 5;
  std::vector<std::string> recentFiles_;
};

} // namespace demo
