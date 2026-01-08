#include "SimpleViewerApp.h"
#include "../../external/IconsFontAwesome6.h"
#include "../../external/imgui/imgui.h"
#include "../../framework/core/ActionRegistry.h"
#include "../../framework/gui/GuiUtils.h"
#include "../../framework/gui/NotificationCenter.h"
#include "../../framework/gui/Theme.h"
#include <GLFW/glfw3.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mach-o/dyld.h> // For _NSGetExecutablePath on macOS
#include <unistd.h>      // For fork/execv

namespace demo {

SimpleViewerApp::SimpleViewerApp() {}

SimpleViewerApp::~SimpleViewerApp() { onShutdown(); }

framework::ClientAppConfig SimpleViewerApp::getConfig() {
  framework::ClientAppConfig config;
  config.appTitle = "Modern Viewer Pro";
  config.initialWidth = 1600;
  config.initialHeight = 1000;
  config.backgroundImage = "background_calm.png";
  return config;
}

void SimpleViewerApp::onInit() {
  // Register Actions for Command Palette
  auto &registry = framework::ActionRegistry::instance();

  registry.registerAction(
      "file.open", "Open File", "File", [this]() { showFilePicker_ = true; },
      "Cmd+O");

  registry.registerAction(
      "file.close", "Close Tab", "File",
      [this]() {
        if (!tabs_.empty())
          closeTab(activeTabIndex_);
      },
      "Cmd+W");

  registry.registerAction("view.dashboard", "Go to Dashboard", "View",
                          [this]() { currentPage_ = AppPage::Dashboard; });

  registry.registerAction("view.viewer", "Go to Viewer", "View",
                          [this]() { currentPage_ = AppPage::Viewer; });

  registry.registerAction("view.settings", "Open Settings", "View",
                          [this]() { currentPage_ = AppPage::Settings; });

  registry.registerAction("help.about", "About Modern Viewer", "Help",
                          [this]() { showAbout_ = true; });

  std::cout << "SimpleViewerApp initialized\n";
}

void SimpleViewerApp::onUpdate(float deltaTime) { (void)deltaTime; }

void SimpleViewerApp::onRenderUI() {
  const ImGuiViewport *viewport = ImGui::GetMainViewport();

  // Full screen background
  ImGui::SetNextWindowPos(viewport->Pos);
  ImGui::SetNextWindowSize(viewport->Size);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  ImGui::Begin("##Background", nullptr,
               ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs |
                   ImGuiWindowFlags_NoBackground |
                   ImGuiWindowFlags_NoBringToFrontOnFocus);
  ImGui::End();
  ImGui::PopStyleVar(2);

  renderMenuBar();
  renderSidebar();

  // Main content area (adjusted for menu bar and sidebar)
  float menuBarHeight = 30.0f;
  float sidebarWidth = 70.0f;
  float statusBarHeight = 28.0f;
  float contentX = viewport->Pos.x + sidebarWidth;
  float contentY = viewport->Pos.y + menuBarHeight;
  float contentW = viewport->Size.x - sidebarWidth;
  float contentH = viewport->Size.y - menuBarHeight - statusBarHeight;

  ImGui::SetNextWindowPos(ImVec2(contentX, contentY));
  ImGui::SetNextWindowSize(ImVec2(contentW, contentH));
  ImGui::Begin("##MainContent", nullptr,
               ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);

  switch (currentPage_) {
  case AppPage::Dashboard:
    renderDashboardPage();
    break;
  case AppPage::Viewer:
    renderViewerPage();
    break;
  case AppPage::Settings:
    renderSettingsPage();
    break;
  }

  ImGui::End();

  renderStatusBar();
  renderAboutDialog();
  renderFilePicker();
}

void SimpleViewerApp::renderMenuBar() {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN " Open...", "Cmd+O")) {
        showFilePicker_ = true;
      }

      if (ImGui::BeginMenu(ICON_FA_CLOCK_ROTATE_LEFT " Recent Files")) {
        if (recentFiles_.empty()) {
          ImGui::TextDisabled("No recent files");
        } else {
          for (const auto &path : recentFiles_) {
            if (ImGui::MenuItem(
                    std::filesystem::path(path).filename().string().c_str())) {
              openFile(path);
            }
          }
        }
        ImGui::EndMenu();
      }

      ImGui::Separator();

      if (ImGui::MenuItem(ICON_FA_XMARK " Close Tab", "Cmd+W", false,
                          !tabs_.empty())) {
        closeTab(activeTabIndex_);
      }

      ImGui::Separator();

      if (ImGui::MenuItem(ICON_FA_ARROWS_ROTATE " Restart", "Cmd+R")) {
        // Get the path to the current executable
        char exePath[1024];
        uint32_t size = sizeof(exePath);
        if (_NSGetExecutablePath(exePath, &size) == 0) {
          framework::NotificationCenter::instance().info("Restarting...");
          // Fork and exec to restart
          pid_t pid = fork();
          if (pid == 0) {
            // Child process: wait a moment then launch new instance
            usleep(500000); // 500ms delay
            char *argv[] = {exePath, nullptr};
            execv(exePath, argv);
            exit(0);
          }
          // Parent: request window close
          glfwSetWindowShouldClose(glfwGetCurrentContext(), GLFW_TRUE);
        }
      }

      if (ImGui::MenuItem(ICON_FA_ARROW_RIGHT_FROM_BRACKET " Quit", "Cmd+Q")) {
        glfwSetWindowShouldClose(glfwGetCurrentContext(), GLFW_TRUE);
      }
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("View")) {
      if (ImGui::MenuItem(ICON_FA_HOUSE " Dashboard", nullptr,
                          currentPage_ == AppPage::Dashboard)) {
        currentPage_ = AppPage::Dashboard;
      }
      if (ImGui::MenuItem(ICON_FA_FILE_LINES " Viewer", nullptr,
                          currentPage_ == AppPage::Viewer)) {
        currentPage_ = AppPage::Viewer;
      }
      if (ImGui::MenuItem(ICON_FA_GEARS " Settings", nullptr,
                          currentPage_ == AppPage::Settings)) {
        currentPage_ = AppPage::Settings;
      }
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Help")) {
      if (ImGui::MenuItem(ICON_FA_CIRCLE_INFO " About Modern Viewer")) {
        showAbout_ = true;
      }
      if (ImGui::MenuItem(ICON_FA_KEYBOARD " Keyboard Shortcuts", "Cmd+P")) {
        framework::ActionRegistry::instance().execute("palette.toggle");
      }
      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }
}

void SimpleViewerApp::renderSidebar() {
  const ImGuiViewport *viewport = ImGui::GetMainViewport();
  float sidebarWidth = 70.0f;
  float menuBarHeight = 30.0f;

  ImGui::SetNextWindowPos(
      ImVec2(viewport->Pos.x, viewport->Pos.y + menuBarHeight));
  ImGui::SetNextWindowSize(
      ImVec2(sidebarWidth, viewport->Size.y - menuBarHeight));
  ImGui::Begin("##Sidebar", nullptr,
               ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);

  auto navButton = [&](const char *icon, const char *tooltip, AppPage page) {
    bool active = (currentPage_ == page);
    if (active) {
      ImGui::PushStyleColor(ImGuiCol_Button,
                            ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
      ImGui::PushStyleColor(ImGuiCol_Text,
                            ImGui::GetStyle().Colors[ImGuiCol_CheckMark]);
    }

    ImGui::SetCursorPosX((sidebarWidth - 40) * 0.5f);
    if (ImGui::Button(icon, ImVec2(40, 40))) {
      currentPage_ = page;
    }
    if (ImGui::IsItemHovered())
      ImGui::SetTooltip("%s", tooltip);

    if (active)
      ImGui::PopStyleColor(2);
    ImGui::Spacing();
  };

  ImGui::Spacing();
  ImGui::Spacing();

  // Logo
  ImGui::SetCursorPosX((sidebarWidth - 30) * 0.5f);
  ImGui::PushStyleColor(
      ImGuiCol_Text, framework::Theme::getColor(framework::ColorRole::Accent));
  ImGui::Text("%s", ICON_FA_SHIELD_HALVED);
  ImGui::PopStyleColor();

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();
  ImGui::Spacing();

  // Navigation
  navButton(ICON_FA_HOUSE, "Dashboard", AppPage::Dashboard);
  navButton(ICON_FA_FILE_LINES, "File Viewer", AppPage::Viewer);
  navButton(ICON_FA_GEARS, "Settings", AppPage::Settings);

  // Bottom icons
  ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 100);

  ImGui::SetCursorPosX((sidebarWidth - 40) * 0.5f);
  if (ImGui::Button(ICON_FA_CIRCLE_INFO, ImVec2(40, 40))) {
    showAbout_ = true;
  }
  if (ImGui::IsItemHovered())
    ImGui::SetTooltip("About");

  ImGui::End();
}

void SimpleViewerApp::renderStatusBar() {
  const ImGuiViewport *viewport = ImGui::GetMainViewport();
  float statusBarHeight = 28.0f;

  ImGui::SetNextWindowPos(ImVec2(
      viewport->Pos.x, viewport->Pos.y + viewport->Size.y - statusBarHeight));
  ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, statusBarHeight));

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 5));
  ImGui::Begin("##StatusBar", nullptr,
               ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoScrollbar);

  // Left side: page info
  ImGui::TextDisabled("%s", currentPage_ == AppPage::Dashboard ? "Dashboard"
                            : currentPage_ == AppPage::Viewer  ? "Viewer"
                                                               : "Settings");

  // Center: file info
  if (!tabs_.empty() && activeTabIndex_ < tabs_.size()) {
    ImGui::SameLine(ImGui::GetWindowWidth() * 0.3f);
    auto &tab = tabs_[activeTabIndex_];
    if (tab.isIndexing) {
      ImGui::Text(ICON_FA_SPINNER " Indexing...");
    } else {
      ImGui::Text("%s | %zu lines", tab.name.c_str(), tab.lineOffsets.size());
    }
  }

  // Right side: tabs count & keyboard hint
  ImGui::SameLine(ImGui::GetWindowWidth() - 200);
  ImGui::TextDisabled("%zu file(s) open", tabs_.size());

  ImGui::SameLine(ImGui::GetWindowWidth() - 80);
  ImGui::TextDisabled("Cmd+P");

  ImGui::End();
  ImGui::PopStyleVar();
}

void SimpleViewerApp::renderDashboardPage() {
  ImGui::Spacing();
  ImGui::Spacing();

  // Welcome header
  ImGui::PushStyleColor(
      ImGuiCol_Text, framework::Theme::getColor(framework::ColorRole::Accent));
  ImGui::Text(ICON_FA_HOUSE " Dashboard");
  ImGui::PopStyleColor();
  ImGui::Separator();
  ImGui::Spacing();

  // Quick actions
  ImGui::Text("Quick Actions");
  ImGui::Spacing();

  if (ImGui::Button(ICON_FA_FOLDER_OPEN " Open File", ImVec2(200, 50))) {
    showFilePicker_ = true;
  }
  ImGui::SameLine();
  if (ImGui::Button(ICON_FA_FILE_LINES " Go to Viewer", ImVec2(200, 50))) {
    currentPage_ = AppPage::Viewer;
  }
  ImGui::SameLine();
  if (ImGui::Button(ICON_FA_GEARS " Settings", ImVec2(200, 50))) {
    currentPage_ = AppPage::Settings;
  }

  ImGui::Spacing();
  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  // Open files list
  ImGui::Text(ICON_FA_FILE_LINES " Open Files (%zu)", tabs_.size());
  ImGui::Spacing();

  if (tabs_.empty()) {
    ImGui::TextDisabled("No files open. Use 'Open File' to get started.");
  } else {
    for (size_t i = 0; i < tabs_.size(); ++i) {
      ImGui::PushID((int)i);

      bool isActive = (i == activeTabIndex_);
      if (isActive) {
        ImGui::PushStyleColor(ImGuiCol_Button,
                              ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
      }

      if (ImGui::Button(tabs_[i].name.c_str(), ImVec2(-60, 35))) {
        activeTabIndex_ = i;
        currentPage_ = AppPage::Viewer;
      }

      if (isActive)
        ImGui::PopStyleColor();

      ImGui::SameLine();
      if (ImGui::Button(ICON_FA_XMARK, ImVec2(50, 35))) {
        closeTab(i);
      }

      ImGui::PopID();
    }
  }

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  // Recent files
  ImGui::Text(ICON_FA_CLOCK_ROTATE_LEFT " Recent Files");
  ImGui::Spacing();

  if (recentFiles_.empty()) {
    ImGui::TextDisabled("No recent files.");
  } else {
    for (const auto &path : recentFiles_) {
      if (ImGui::Selectable(
              std::filesystem::path(path).filename().string().c_str())) {
        openFile(path);
      }
    }
  }
}

void SimpleViewerApp::renderViewerPage() {
  // Tab bar for open files
  if (ImGui::BeginTabBar("##FileTabs",
                         ImGuiTabBarFlags_Reorderable |
                             ImGuiTabBarFlags_AutoSelectNewTabs)) {
    for (size_t i = 0; i < tabs_.size(); ++i) {
      bool open = true;
      ImGuiTabItemFlags flags =
          (i == activeTabIndex_) ? ImGuiTabItemFlags_SetSelected : 0;

      if (ImGui::BeginTabItem(tabs_[i].name.c_str(), &open, flags)) {
        activeTabIndex_ = i;
        ImGui::EndTabItem();
      }

      if (!open) {
        closeTab(i);
        if (i > 0)
          --i; // Adjust for removed tab
      }
    }
    ImGui::EndTabBar();
  }

  ImGui::Separator();

  if (tabs_.empty()) {
    // Empty state
    ImGui::SetCursorPosY(ImGui::GetWindowHeight() * 0.35f);

    ImDrawList *drawList = ImGui::GetWindowDrawList();
    ImVec2 center =
        ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowWidth() * 0.5f,
               ImGui::GetWindowPos().y + ImGui::GetWindowHeight() * 0.4f);
    drawList->AddCircleFilled(center, 150, IM_COL32(76, 201, 240, 15), 64);

    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 500) * 0.5f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 16.0f);
    ImGui::BeginChild("##WelcomeCard", ImVec2(500, 260), true,
                      ImGuiWindowFlags_NoScrollbar);

    ImGui::SetCursorPosY(40);
    ImGui::SetCursorPosX((500 - 64) * 0.5f);
    ImGui::PushStyleColor(ImGuiCol_Text, framework::Theme::getColor(
                                             framework::ColorRole::Accent));
    ImGui::Text("%s", ICON_FA_ROCKET);
    ImGui::PopStyleColor();

    ImGui::SetCursorPosY(100);
    const char *title = "High Performance Engine Ready";
    ImGui::SetCursorPosX((500 - ImGui::CalcTextSize(title).x) * 0.5f);
    ImGui::TextUnformatted(title);

    ImGui::SetCursorPosY(140);
    const char *subtitle = "Open a file to start viewing.";
    ImGui::SetCursorPosX((500 - ImGui::CalcTextSize(subtitle).x) * 0.5f);
    ImGui::TextDisabled("%s", subtitle);

    ImGui::SetCursorPosY(190);
    ImGui::SetCursorPosX((500 - 150) * 0.5f);
    if (ImGui::Button("Open File", ImVec2(150, 45))) {
      showFilePicker_ = true;
    }

    ImGui::EndChild();
    ImGui::PopStyleVar();
    return;
  }

  // Active file viewer
  auto &tab = tabs_[activeTabIndex_];

  if (tab.isIndexing) {
    ImGui::SetCursorPosY(ImGui::GetWindowHeight() * 0.45f);
    float progressWidth = ImGui::GetWindowWidth() * 0.5f;
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - progressWidth) * 0.5f);

    ImGui::BeginGroup();
    ImGui::Text(ICON_FA_GEARS " Indexing: %s", tab.name.c_str());
    ImGui::ProgressBar(tab.indexingProgress, ImVec2(progressWidth, 12));
    ImGui::TextDisabled("Optimizing memory mapping...");
    ImGui::EndGroup();
    return;
  }

  // Search bar
  ImGui::SetNextItemWidth(300);
  ImGui::InputTextWithHint("##search",
                           ICON_FA_MAGNIFYING_GLASS " Search in file...",
                           searchBuffer_, sizeof(searchBuffer_));
  ImGui::SameLine();
  ImGui::TextDisabled("(%zu lines)", tab.lineOffsets.size());

  ImGui::Spacing();

  // File content
  ImGui::BeginChild("##FileContent", ImVec2(0, 0), true,
                    ImGuiWindowFlags_HorizontalScrollbar);

  if (tab.file && tab.file->isOpen()) {
    ImGuiListClipper clipper;
    clipper.Begin((int)tab.lineOffsets.size());

    std::string_view data = tab.file->getView();
    std::string searchStr = searchBuffer_;

    while (clipper.Step()) {
      for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
        size_t start = tab.lineOffsets[i];
        size_t end = (i + 1 < (int)tab.lineOffsets.size())
                         ? tab.lineOffsets[i + 1]
                         : data.size();

        if (end > start && data[end - 1] == '\n')
          end--;
        if (end > start && data[end - 1] == '\r')
          end--;

        std::string_view line = data.substr(start, end - start);

        // Line number
        if (showLineNumbers_) {
          ImGui::TextDisabled("%5d", i + 1);
          ImGui::SameLine();
        }

        // Highlight search matches
        if (!searchStr.empty() &&
            line.find(searchStr) != std::string_view::npos) {
          ImGui::PushStyleColor(
              ImGuiCol_Text,
              framework::Theme::getColor(framework::ColorRole::Accent));
          ImGui::TextUnformatted(line.data(), line.data() + line.size());
          ImGui::PopStyleColor();
        } else {
          ImGui::TextUnformatted(line.data(), line.data() + line.size());
        }
      }
    }
  }

  ImGui::EndChild();
}

void SimpleViewerApp::renderSettingsPage() {
  ImGui::Spacing();
  ImGui::Spacing();

  ImGui::PushStyleColor(
      ImGuiCol_Text, framework::Theme::getColor(framework::ColorRole::Accent));
  ImGui::Text(ICON_FA_GEARS " Settings");
  ImGui::PopStyleColor();
  ImGui::Separator();
  ImGui::Spacing();

  // Appearance
  ImGui::Text(ICON_FA_PALETTE " Appearance");
  ImGui::Indent();

  ImGui::SliderFloat("Font Size", &fontSize_, 12.0f, 32.0f, "%.0f px");

  ImGui::Unindent();
  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  // Editor
  ImGui::Text(ICON_FA_CODE " Editor");
  ImGui::Indent();

  ImGui::Checkbox("Show Line Numbers", &showLineNumbers_);
  ImGui::Checkbox("Word Wrap", &wordWrap_);
  ImGui::SliderInt("Recent Files Limit", &recentFilesLimit_, 3, 20);

  ImGui::Unindent();
  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  // Actions
  ImGui::Text(ICON_FA_TRASH " Data");
  ImGui::Indent();

  if (ImGui::Button("Clear Recent Files")) {
    recentFiles_.clear();
    framework::NotificationCenter::instance().success("Recent files cleared.");
  }

  ImGui::Unindent();
}

void SimpleViewerApp::renderAboutDialog() {
  if (!showAbout_)
    return;

  ImGui::OpenPopup("About Modern Viewer");

  ImVec2 center = ImGui::GetMainViewport()->GetCenter();
  ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
  ImGui::SetNextWindowSize(ImVec2(400, 300));

  if (ImGui::BeginPopupModal("About Modern Viewer", &showAbout_,
                             ImGuiWindowFlags_NoResize)) {
    ImGui::Spacing();
    ImGui::SetCursorPosX((400 - 50) * 0.5f);
    ImGui::PushStyleColor(ImGuiCol_Text, framework::Theme::getColor(
                                             framework::ColorRole::Accent));
    ImGui::Text("%s", ICON_FA_SHIELD_HALVED);
    ImGui::PopStyleColor();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    const char *title = "Modern Viewer Pro";
    ImGui::SetCursorPosX((400 - ImGui::CalcTextSize(title).x) * 0.5f);
    ImGui::Text("%s", title);

    ImGui::Spacing();

    const char *version = "Version 1.0.0";
    ImGui::SetCursorPosX((400 - ImGui::CalcTextSize(version).x) * 0.5f);
    ImGui::TextDisabled("%s", version);

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::TextWrapped(
        "A high-performance file viewer built on the Modern C++ Framework.");
    ImGui::TextWrapped(
        "Features: Memory-mapped I/O, async indexing, glassmorphism UI.");

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::SetCursorPosX((400 - 80) * 0.5f);
    if (ImGui::Button("Close", ImVec2(80, 35))) {
      showAbout_ = false;
      ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
  }
}

void SimpleViewerApp::renderFilePicker() {
  if (!showFilePicker_)
    return;

  ImGui::OpenPopup("Open File");

  std::string selectedPath;
  if (framework::GuiUtils::renderFilePicker("Open File", showFilePicker_,
                                            selectedPath)) {
    if (!selectedPath.empty()) {
      openFile(selectedPath);
    }
    showFilePicker_ = false;
  }
}

void SimpleViewerApp::openFile(const std::string &path) {
  // Check if already open
  for (size_t i = 0; i < tabs_.size(); ++i) {
    if (tabs_[i].path == path) {
      activeTabIndex_ = i;
      currentPage_ = AppPage::Viewer;
      return;
    }
  }

  // Create new tab
  FileTab newTab;
  newTab.path = path;
  newTab.name = std::filesystem::path(path).filename().string();
  newTab.isIndexing = true;
  newTab.indexingProgress = 0.0f;

  tabs_.push_back(std::move(newTab));
  activeTabIndex_ = tabs_.size() - 1;
  currentPage_ = AppPage::Viewer;

  // Add to recent files
  recentFiles_.erase(
      std::remove(recentFiles_.begin(), recentFiles_.end(), path),
      recentFiles_.end());
  recentFiles_.insert(recentFiles_.begin(), path);
  if ((int)recentFiles_.size() > recentFilesLimit_) {
    recentFiles_.resize(recentFilesLimit_);
  }

  framework::NotificationCenter::instance().info("Opening: " + newTab.name);

  // Start async indexing
  size_t tabIndex = activeTabIndex_;
  if (indexerThread_.joinable())
    indexerThread_.join();
  indexerThread_ =
      std::thread(&SimpleViewerApp::indexFileAsync, this, tabIndex, path);
}

void SimpleViewerApp::closeTab(size_t index) {
  if (index >= tabs_.size())
    return;

  std::string name = tabs_[index].name;
  tabs_.erase(tabs_.begin() + index);

  if (activeTabIndex_ >= tabs_.size() && !tabs_.empty()) {
    activeTabIndex_ = tabs_.size() - 1;
  }

  framework::NotificationCenter::instance().info("Closed: " + name);
}

void SimpleViewerApp::indexFileAsync(size_t tabIndex, const std::string &path) {
  if (tabIndex >= tabs_.size())
    return;

  auto &tab = tabs_[tabIndex];

  std::lock_guard<std::mutex> lock(viewerMutex_);

  tab.file = std::make_unique<framework::MemoryMappedFile>(path);
  if (!tab.file->isOpen()) {
    tab.isIndexing = false;
    framework::NotificationCenter::instance().error("Failed to open file.");
    return;
  }

  std::string_view data = tab.file->getView();
  tab.lineOffsets.clear();
  tab.lineOffsets.reserve(data.size() / 80); // Estimate

  tab.lineOffsets.push_back(0);
  size_t totalSize = data.size();

  for (size_t i = 0; i < totalSize; ++i) {
    if (data[i] == '\n') {
      tab.lineOffsets.push_back(i + 1);
    }
    if (i % 100000 == 0) {
      tab.indexingProgress = (float)i / (float)totalSize;
    }
  }

  tab.isIndexing = false;
  tab.indexingProgress = 1.0f;

  framework::NotificationCenter::instance().success(
      "Indexed: " + std::to_string(tab.lineOffsets.size()) + " lines");
}

void SimpleViewerApp::onShutdown() {
  if (indexerThread_.joinable()) {
    indexerThread_.join();
  }
  tabs_.clear();
}

void SimpleViewerApp::onFileDropped(const std::string &path) { openFile(path); }

} // namespace demo
