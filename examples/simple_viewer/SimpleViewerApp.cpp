#include "SimpleViewerApp.h"
#include "../../external/IconsFontAwesome6.h"
#include "../../external/imgui/imgui.h"
#include "../../framework/core/ActionRegistry.h"
#include "../../framework/gui/GuiUtils.h"
#include "../../framework/gui/NotificationCenter.h"
#include <filesystem>
#include <iostream>

namespace demo {

SimpleViewerApp::SimpleViewerApp() {}

SimpleViewerApp::~SimpleViewerApp() { onShutdown(); }

framework::ClientAppConfig SimpleViewerApp::getConfig() {
  framework::ClientAppConfig config;
  config.appTitle = "Simple Viewer - Framework Demo";
  config.initialWidth = 1600;
  config.initialHeight = 1000;
  config.backgroundImage = "background_calm.png";
  return config;
}

void SimpleViewerApp::onInit() {
  // Register Actions for Command Palette
  framework::ActionRegistry::instance().registerAction(
      "file.open", "Open File", "File", [this]() { showFilePicker_ = true; },
      "Cmd+O");

  framework::ActionRegistry::instance().registerAction(
      "app.about", "Show About", "App", [this]() { /* Show about later */ });
  std::cout << "SimpleViewerApp initialized\n";
}

void SimpleViewerApp::onUpdate(float deltaTime) {
  // Logic updates here if needed
  (void)deltaTime;
}

void SimpleViewerApp::onRenderUI() {
  // Setup whole screen layout
  const ImGuiViewport *viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->Pos);
  ImGui::SetNextWindowSize(viewport->Size);

  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  ImGui::Begin("BackgroundWindow", nullptr,
               ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs |
                   ImGuiWindowFlags_NoBackground |
                   ImGuiWindowFlags_NoBringToFrontOnFocus);
  ImGui::End();
  ImGui::PopStyleVar(2);

  // --- Sidebar ---
  float sidebarWidth = 70.0f;
  ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y));
  ImGui::SetNextWindowSize(ImVec2(sidebarWidth, viewport->Size.y));
  ImGui::Begin("Sidebar", nullptr,
               ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);

  float iconSize = 24.0f;
  ImGui::SetCursorPosY(20);
  ImGui::SetCursorPosX((sidebarWidth - iconSize) * 0.5f);
  ImGui::Text("%s", ICON_FA_SHIELD_HALVED); // App Logo

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  auto sidebarIcon = [&](const char *icon, bool active) {
    if (active)
      ImGui::PushStyleColor(ImGuiCol_Text,
                            ImGui::GetStyle().Colors[ImGuiCol_CheckMark]);
    ImGui::SetCursorPosX((sidebarWidth - iconSize) * 0.5f);
    ImGui::Text("%s", icon);
    if (active)
      ImGui::PopStyleColor();
    ImGui::Spacing();
    ImGui::Spacing();
  };

  sidebarIcon(ICON_FA_HOUSE, true);
  sidebarIcon(ICON_FA_FILE_LINES, false);
  sidebarIcon(ICON_FA_GEARS, false);

  ImGui::SetCursorPosY(viewport->Size.y - 40);
  sidebarIcon(ICON_FA_CIRCLE_INFO, false);

  ImGui::End();

  // --- Main Content Area ---
  float contentX = viewport->Pos.x + sidebarWidth + 20;
  float contentY = viewport->Pos.y + 20;
  ImGui::SetNextWindowPos(ImVec2(contentX, contentY));
  ImGui::SetNextWindowSize(
      ImVec2(viewport->Size.x - sidebarWidth - 40, viewport->Size.y - 40));

  ImGui::Begin("MainPanel", nullptr,
               ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);

  // Header with search and title
  ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.30f, 0.79f, 0.94f, 1.0f));
  ImGui::Text("MODERN VIEWER");
  ImGui::PopStyleColor();

  ImGui::SameLine(ImGui::GetWindowWidth() - 340);
  ImGui::SetNextItemWidth(200);
  static char dummySearch[128] = "";
  ImGui::InputTextWithHint("##search", ICON_FA_MAGNIFYING_GLASS " Search...",
                           dummySearch, IM_ARRAYSIZE(dummySearch));

  ImGui::SameLine();
  if (ImGui::Button(ICON_FA_FOLDER_OPEN " Open")) {
    showFilePicker_ = true;
    ImGui::OpenPopup("Select File");
  }

  ImGui::Separator();

  if (isIndexing_) {
    ImGui::SetCursorPosY(ImGui::GetWindowHeight() * 0.45f);
    float progressWidth = ImGui::GetWindowWidth() * 0.5f;
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - progressWidth) * 0.5f);

    ImGui::BeginGroup();
    ImGui::Text(
        ICON_FA_GEARS " Processing Engine: %s",
        std::filesystem::path(currentPath_).filename().string().c_str());
    ImGui::ProgressBar(indexingProgress_, ImVec2(progressWidth, 12));
    ImGui::TextDisabled("Optimizing memory mapping and line indexing...");
    ImGui::EndGroup();
  } else {
    if (!logFile_ || !logFile_->isOpen()) {
      // "Wow" Empty State
      ImGui::SetCursorPosY(ImGui::GetWindowHeight() * 0.35f);

      ImDrawList *drawList = ImGui::GetWindowDrawList();
      ImVec2 center =
          ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowWidth() * 0.5f,
                 ImGui::GetWindowPos().y + ImGui::GetWindowHeight() * 0.4f);

      // Subtle glow behind the welcome
      drawList->AddCircleFilled(center, 150, IM_COL32(76, 201, 240, 15), 64);

      ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 500) * 0.5f);
      ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 16.0f);
      ImGui::BeginChild("WelcomeCard", ImVec2(500, 260), true,
                        ImGuiWindowFlags_NoScrollbar);

      ImGui::SetCursorPosY(40);
      ImGui::SetCursorPosX((500 - 64) * 0.5f);
      // Make rocket look bigger and cyan
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.8f, 0.95f, 1.0f));
      ImGui::Text("%s", ICON_FA_ROCKET);
      ImGui::PopStyleColor();

      ImGui::SetCursorPosY(100);
      const char *title = "High Performance Engine Ready";
      ImGui::SetCursorPosX((500 - ImGui::CalcTextSize(title).x) * 0.5f);
      ImGui::TextUnformatted(title);

      ImGui::SetCursorPosY(140);
      const char *subtitle =
          "Drop a file here or use the selector below to start.";
      ImGui::SetCursorPosX((500 - ImGui::CalcTextSize(subtitle).x) * 0.5f);
      ImGui::TextDisabled("%s", subtitle);

      ImGui::SetCursorPosY(190);
      ImGui::SetCursorPosX((500 - 150) * 0.5f);
      if (ImGui::Button("Select File", ImVec2(150, 45))) {
        showFilePicker_ = true;
        ImGui::OpenPopup("Select File");
      }

      ImGui::EndChild();
      ImGui::PopStyleVar();
    } else {
      // Glass Viewport
      ImGui::BeginChild("LogViewport", ImVec2(0, -45), true,
                        ImGuiWindowFlags_HorizontalScrollbar);

      ImGuiListClipper clipper;
      clipper.Begin((int)lineOffsets_.size());

      std::string_view data = logFile_->getView();

      while (clipper.Step()) {
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
          size_t start = lineOffsets_[i];
          size_t end = (i + 1 < (int)lineOffsets_.size()) ? lineOffsets_[i + 1]
                                                          : data.size();

          if (end > start && data[end - 1] == '\n')
            end--;
          if (end > start && data[end - 1] == '\r')
            end--;

          std::string_view line = data.substr(start, end - start);

          // Subtle line numbering
          ImGui::TextDisabled("%5d", i + 1);
          ImGui::SameLine();
          ImGui::TextUnformatted(line.data(), line.data() + line.length());
        }
      }
      ImGui::EndChild();

      // Status bar info
      ImGui::TextDisabled("%s Source: %s", ICON_FA_CIRCLE_INFO,
                          currentPath_.c_str());
      ImGui::SameLine(ImGui::GetWindowWidth() - 150);
      ImGui::TextDisabled("Lines: %zu", lineOffsets_.size());
    }
  }

  ImGui::End();

  // File Picker
  if (showFilePicker_) {
    std::string newPath;
    if (framework::GuiUtils::renderFilePicker("Select File", showFilePicker_,
                                              newPath)) {
      openFile(newPath);
    }
  }
}

void SimpleViewerApp::onShutdown() {
  if (indexerThread_.joinable()) {
    indexerThread_.join();
  }
}

void SimpleViewerApp::onFileDropped(const std::string &path) { openFile(path); }

void SimpleViewerApp::openFile(const std::string &path) {
  if (indexerThread_.joinable()) {
    indexerThread_.join();
  }

  currentPath_ = path;
  isIndexing_ = true;
  indexingProgress_ = 0.0f;

  framework::NotificationCenter::instance().post(
      "Indexing Started",
      "Analyzing " + std::filesystem::path(path).filename().string(),
      framework::NotificationType::Info);

  indexerThread_ = std::thread(&SimpleViewerApp::indexFileAsync, this, path);
}

void SimpleViewerApp::indexFileAsync(const std::string &path) {
  try {
    auto file = std::make_unique<framework::MemoryMappedFile>(path);
    if (!file->isOpen()) {
      isIndexing_ = false;
      return;
    }

    std::string_view data = file->getView();
    std::vector<size_t> localOffsets;

    if (!data.empty()) {
      localOffsets.reserve(data.size() / 100);
      localOffsets.push_back(0);
      size_t pos = 0;
      size_t totalSize = data.size();
      size_t lastUpdatePos = 0;

      while (true) {
        pos = data.find('\n', pos);
        if (pos == std::string_view::npos)
          break;
        pos++;
        if (pos < totalSize) {
          localOffsets.push_back(pos);
          if (pos - lastUpdatePos > 1024 * 1024) {
            indexingProgress_ = (float)pos / (float)totalSize;
            lastUpdatePos = pos;
          }
        } else
          break;
      }
    }

    {
      std::lock_guard<std::mutex> lock(viewerMutex_);
      lineOffsets_ = std::move(localOffsets);
      logFile_ = std::move(file);
    }
  } catch (...) {
  }
  isIndexing_ = false;
  indexingProgress_ = 1.0f;

  framework::NotificationCenter::instance().success(
      "Indexing Complete: " + std::to_string(lineOffsets_.size()) +
      " lines analyzed.");
}

} // namespace demo
