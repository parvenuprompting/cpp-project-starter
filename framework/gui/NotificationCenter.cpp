#include "NotificationCenter.h"
#include "../../external/IconsFontAwesome6.h"
#include "../../external/imgui/imgui.h"

namespace framework {

void NotificationCenter::post(const std::string &title,
                              const std::string &message, NotificationType type,
                              float duration) {
  notifications_.push_back(
      {title, message, type, std::chrono::steady_clock::now(), duration});
}

void NotificationCenter::render() {
  if (notifications_.empty())
    return;

  auto now = std::chrono::steady_clock::now();
  ImVec2 displaySize = ImGui::GetIO().DisplaySize;
  float yOffset = 20.0f;
  float margin = 20.0f;
  float width = 300.0f;

  for (auto it = notifications_.begin(); it != notifications_.end();) {
    float elapsed = std::chrono::duration<float>(now - it->startTime).count();
    if (elapsed > it->duration) {
      it = notifications_.erase(it);
      continue;
    }

    // Calculate opacity (fade in/out)
    float alpha = 1.0f;
    if (elapsed < 0.3f)
      alpha = elapsed / 0.3f; // Fade in
    else if (elapsed > it->duration - 0.5f)
      alpha = (it->duration - elapsed) / 0.5f; // Fade out

    // Window position (top right)
    ImGui::SetNextWindowPos(ImVec2(displaySize.x - width - margin, yOffset),
                            ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(width, 0));
    ImGui::SetNextWindowBgAlpha(alpha * 0.8f);

    char id[32];
    snprintf(id, sizeof(id), "##toast_%zu",
             std::distance(notifications_.begin(), it));

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

    if (ImGui::Begin(id, nullptr, flags)) {
      ImVec4 iconColor;
      const char *icon = ICON_FA_CIRCLE_INFO;

      switch (it->type) {
      case NotificationType::Success:
        icon = ICON_FA_CIRCLE_CHECK;
        iconColor = ImVec4(0.2f, 0.8f, 0.2f, alpha);
        break;
      case NotificationType::Warning:
        icon = ICON_FA_TRIANGLE_EXCLAMATION;
        iconColor = ImVec4(1.0f, 0.8f, 0.0f, alpha);
        break;
      case NotificationType::Error:
        icon = ICON_FA_CIRCLE_XMARK;
        iconColor = ImVec4(1.0f, 0.2f, 0.2f, alpha);
        break;
      default:
        iconColor = ImVec4(0.3f, 0.8f, 1.0f, alpha);
        break;
      }

      ImGui::TextColored(iconColor, "%s", icon);
      ImGui::SameLine();
      ImGui::Text("%s", it->title.c_str());
      ImGui::Spacing();
      ImGui::TextWrapped("%s", it->message.c_str());

      yOffset += ImGui::GetWindowHeight() + 10.0f;
    }
    ImGui::End();
    ++it;
  }
}

} // namespace framework
