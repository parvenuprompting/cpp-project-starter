#pragma once

#include "../../framework/core/IClientApp.h"
#include "../../framework/io/MemoryMappedFile.h"
#include <atomic>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace demo {

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
  void openFile(const std::string &path);
  void indexFileAsync(const std::string &path);

  std::string currentPath_;
  std::unique_ptr<framework::MemoryMappedFile> logFile_;
  std::vector<size_t> lineOffsets_;

  std::mutex viewerMutex_;
  std::thread indexerThread_;
  std::atomic<bool> isIndexing_{false};
  std::atomic<float> indexingProgress_{0.0f};

  // UI state
  bool showFilePicker_ = false;
  std::string searchBuffer_;
  int selectedLine_ = -1;
};

} // namespace demo
