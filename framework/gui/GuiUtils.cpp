#include "GuiUtils.h"
#include "../../external/IconsFontAwesome6.h"
#include "../../external/imgui/imgui.h"
#include <algorithm>
#include <iostream>

namespace framework {

bool GuiUtils::renderFilePicker(const char *popupName, bool &isOpen,
                                std::string &outPath) {
  bool selected = false;
  static std::filesystem::path currentDir =
      std::filesystem::exists(std::getenv("HOME") ? std::getenv("HOME") : "")
          ? std::getenv("HOME")
          : std::filesystem::current_path();
  static std::vector<std::filesystem::path> dirs;
  static std::vector<std::filesystem::path> files;
  static bool initialized = false;
  static bool needRefresh = true;

  auto updateList = [&]() {
    dirs.clear();
    files.clear();
    try {
      if (!std::filesystem::exists(currentDir))
        return;
      for (const auto &entry :
           std::filesystem::directory_iterator(currentDir)) {
        try {
          if (entry.is_directory())
            dirs.push_back(entry.path());
          else if (entry.is_regular_file())
            files.push_back(entry.path());
        } catch (...) {
        }
      }
      std::sort(dirs.begin(), dirs.end());
      std::sort(files.begin(), files.end());
    } catch (const std::exception &e) {
      std::cerr << "FilePicker Error: " << e.what() << std::endl;
    }
    needRefresh = false;
  };

  if (!initialized || needRefresh) {
    updateList();
    initialized = true;
  }

  ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
  if (ImGui::BeginPopupModal(popupName, &isOpen)) {
    // Navigation Bar
    ImGui::TextDisabled("LOCATION:");
    ImGui::SameLine();
    ImGui::Text("%s", currentDir.string().c_str());

    ImGui::Spacing();
    if (ImGui::Button(ICON_FA_ARROW_UP " Up")) {
      currentDir = currentDir.parent_path();
      needRefresh = true;
    }
    ImGui::SameLine();
    if (ImGui::Button(ICON_FA_HOUSE " Home")) {
      const char *home = std::getenv("HOME");
      if (home) {
        currentDir = home;
        needRefresh = true;
      }
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel")) {
      isOpen = false;
      ImGui::CloseCurrentPopup();
    }

    ImGui::Separator();

    ImGui::BeginChild("FileScrollArea", ImVec2(0, -10), true);

    // Use a copy of paths to avoid iterator invalidation during iteration
    std::vector<std::filesystem::path> currentDirs = dirs;
    std::vector<std::filesystem::path> currentFiles = files;

    for (const auto &d : currentDirs) {
      std::string label = ICON_FA_FOLDER "  " + d.filename().string();
      if (ImGui::Selectable(label.c_str(), false,
                            ImGuiSelectableFlags_AllowDoubleClick)) {
        if (ImGui::IsMouseDoubleClicked(0)) {
          currentDir = d;
          needRefresh = true;
        }
      }
      if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) {
        // Single click navigation for better feel if desired, but let's stick
        // to safe
        currentDir = d;
        needRefresh = true;
      }
    }

    for (const auto &f : currentFiles) {
      std::string label = ICON_FA_FILE "  " + f.filename().string();
      if (ImGui::Selectable(label.c_str())) {
        outPath = f.string();
        selected = true;
        isOpen = false;
        ImGui::CloseCurrentPopup();
      }
    }

    ImGui::EndChild();
    ImGui::EndPopup();
  }
  return selected;
}

void GuiUtils::renderCenteredText(const char *text, float yOffset) {
  ImVec2 textSize = ImGui::CalcTextSize(text);
  ImGui::SetCursorPos(
      ImVec2((ImGui::GetWindowSize().x - textSize.x) * 0.5f,
             (ImGui::GetWindowSize().y - textSize.y) * 0.5f + yOffset));
  ImGui::TextUnformatted(text);
}

void GuiUtils::renderSeparatorWithText(const char *text) {
  ImGui::Spacing();
  ImGui::SeparatorText(text);
  ImGui::Spacing();
}

} // namespace framework
