#include "CommandPalette.h"
#include "../../external/IconsFontAwesome6.h"
#include "../../external/imgui/imgui.h"
#include "../core/ActionRegistry.h"
#include <algorithm>
#include <vector>

namespace framework {

bool CommandPalette::isOpen_ = false;
char CommandPalette::searchBuf_[128] = "";
int CommandPalette::selectedIndex_ = 0;

void CommandPalette::render() {
  if (!isOpen_)
    return;

  ImGuiIO &io = ImGui::GetIO();
  ImVec2 center = io.DisplaySize;
  center.x *= 0.5f;
  center.y *= 0.3f;

  ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
  ImGui::SetNextWindowSize(ImVec2(600, 400));

  ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |
                           ImGuiWindowFlags_NoMove |
                           ImGuiWindowFlags_NoSavedSettings;

  if (ImGui::Begin("##CommandPalette", &isOpen_, flags)) {
    // Search Input
    ImGui::PushFont(io.Fonts->Fonts[0]); // Use larger font if available
    if (ImGui::IsWindowAppearing())
      ImGui::SetKeyboardFocusHere();

    ImGui::TextDisabled(ICON_FA_MAGNIFYING_GLASS);
    ImGui::SameLine();
    if (ImGui::InputText("##palette_search", searchBuf_, sizeof(searchBuf_))) {
      selectedIndex_ = 0; // Reset selection on search
    }
    ImGui::PopFont();

    ImGui::Separator();

    // Filter Actions
    std::string searchStr = searchBuf_;
    std::transform(searchStr.begin(), searchStr.end(), searchStr.begin(),
                   ::tolower);

    std::vector<const Action *> filtered;
    for (const auto &[id, action] : ActionRegistry::instance().getActions()) {
      std::string labelLower = action.label;
      std::transform(labelLower.begin(), labelLower.end(), labelLower.begin(),
                     ::tolower);

      if (searchStr.empty() ||
          labelLower.find(searchStr) != std::string::npos ||
          action.category.find(searchStr) != std::string::npos) {
        filtered.push_back(&action);
      }
    }

    // List Resultaten
    ImGui::BeginChild("##Results", ImVec2(0, 0), false);
    for (int i = 0; i < filtered.size(); ++i) {
      bool isSelected = (i == selectedIndex_);

      ImGui::PushID(i);
      if (isSelected) {
        ImGui::PushStyleColor(ImGuiCol_Header,
                              ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]);
      }

      std::string label = filtered[i]->category + ": " + filtered[i]->label;
      if (ImGui::Selectable(label.c_str(), isSelected)) {
        filtered[i]->callback();
        isOpen_ = false;
      }

      if (isSelected) {
        ImGui::PopStyleColor();
        if (ImGui::IsKeyPressed(ImGuiKey_Enter)) {
          filtered[i]->callback();
          isOpen_ = false;
        }
      }

      if (filtered[i]->shortcut != "") {
        ImGui::SameLine(ImGui::GetWindowWidth() - 100);
        ImGui::TextDisabled("%s", filtered[i]->shortcut.c_str());
      }

      ImGui::PopID();
    }

    // Keyboard Navigation
    if (ImGui::IsKeyPressed(ImGuiKey_UpArrow))
      selectedIndex_ = std::max(0, selectedIndex_ - 1);
    if (ImGui::IsKeyPressed(ImGuiKey_DownArrow))
      selectedIndex_ = std::min((int)filtered.size() - 1, selectedIndex_ + 1);
    if (ImGui::IsKeyPressed(ImGuiKey_Escape))
      isOpen_ = false;

    ImGui::EndChild();
    ImGui::End();
  }
}

} // namespace framework
