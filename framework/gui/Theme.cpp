#include "Theme.h"

namespace framework {

ThemeColors Theme::currentColors_;

ThemeColors Theme::getDefaultColors() {
  return ThemeColors(); // Use default values from struct
}

ImVec4 Theme::getColor(ColorRole role) {
  switch (role) {
  case ColorRole::Accent:
    return currentColors_.accentColor;
  case ColorRole::Surface:
    return currentColors_.glassBg;
  case ColorRole::Background:
    return currentColors_.glassBg;
  case ColorRole::Text:
    return currentColors_.textBase;
  case ColorRole::Error:
    return currentColors_.errorColor;
  case ColorRole::Success:
    return currentColors_.successColor;
  case ColorRole::TextDisabled:
    return currentColors_.textDim;
  case ColorRole::SurfaceVariant:
    return currentColors_.glassBgHover;
  default:
    return currentColors_.textBase;
  }
}

void Theme::applyGlassmorphism(const ThemeColors &colors) {
  currentColors_ = colors;
  auto &style = ImGui::GetStyle();
  auto &styleColors = style.Colors;

  // Rounding & Spacing
  style.WindowRounding = 12.0f;
  style.ChildRounding = 8.0f;
  style.FrameRounding = 6.0f;
  style.PopupRounding = 8.0f;
  style.ScrollbarRounding = 12.0f;
  style.GrabRounding = 6.0f;
  style.TabRounding = 6.0f;
  style.WindowPadding = ImVec2(15, 15);
  style.FramePadding = ImVec2(10, 8);
  style.ItemSpacing = ImVec2(12, 10);
  style.SeparatorTextBorderSize = 1.0f;

  // Colors
  styleColors[ImGuiCol_Text] = colors.textBase;
  styleColors[ImGuiCol_TextDisabled] = colors.textDim;
  styleColors[ImGuiCol_WindowBg] = colors.glassBg;
  styleColors[ImGuiCol_ChildBg] = ImVec4(0, 0, 0, 0);
  styleColors[ImGuiCol_PopupBg] =
      ImVec4(0.08f, 0.08f, 0.12f, 0.95f); // Solid-ish for popups
  styleColors[ImGuiCol_Border] = colors.glassBorder;
  styleColors[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);

  styleColors[ImGuiCol_FrameBg] = ImVec4(1, 1, 1, 0.05f);
  styleColors[ImGuiCol_FrameBgHovered] = ImVec4(1, 1, 1, 0.10f);
  styleColors[ImGuiCol_FrameBgActive] = ImVec4(1, 1, 1, 0.15f);

  styleColors[ImGuiCol_TitleBg] = ImVec4(0, 0, 0, 0);
  styleColors[ImGuiCol_TitleBgActive] = ImVec4(0, 0, 0, 0);
  styleColors[ImGuiCol_TitleBgCollapsed] = ImVec4(0, 0, 0, 0);

  styleColors[ImGuiCol_MenuBarBg] = ImVec4(0, 0, 0, 0);
  styleColors[ImGuiCol_ScrollbarBg] = ImVec4(0, 0, 0, 0);
  styleColors[ImGuiCol_ScrollbarGrab] = ImVec4(1, 1, 1, 0.1f);
  styleColors[ImGuiCol_ScrollbarGrabHovered] = colors.accentColor;
  styleColors[ImGuiCol_ScrollbarGrabActive] = colors.accentActive;

  styleColors[ImGuiCol_CheckMark] = colors.accentColor;
  styleColors[ImGuiCol_SliderGrab] = colors.accentColor;
  styleColors[ImGuiCol_SliderGrabActive] = colors.accentActive;

  styleColors[ImGuiCol_Button] = ImVec4(1, 1, 1, 0.05f);
  styleColors[ImGuiCol_ButtonHovered] = colors.accentColor;
  styleColors[ImGuiCol_ButtonActive] = colors.accentActive;

  styleColors[ImGuiCol_Header] = ImVec4(1, 1, 1, 0.1f);
  styleColors[ImGuiCol_HeaderHovered] = colors.accentColor;
  styleColors[ImGuiCol_HeaderActive] = colors.accentActive;

  styleColors[ImGuiCol_Separator] = colors.glassBorder;
  styleColors[ImGuiCol_SeparatorHovered] = colors.accentColor;
  styleColors[ImGuiCol_SeparatorActive] = colors.accentActive;

  styleColors[ImGuiCol_ResizeGrip] = ImVec4(1, 1, 1, 0.1f);
  styleColors[ImGuiCol_ResizeGripHovered] = colors.accentColor;
  styleColors[ImGuiCol_ResizeGripActive] = colors.accentActive;

  styleColors[ImGuiCol_Tab] = ImVec4(1, 1, 1, 0.05f);
  styleColors[ImGuiCol_TabHovered] = colors.accentColor;
  styleColors[ImGuiCol_TabActive] = colors.accentColor;
  styleColors[ImGuiCol_TabUnfocused] = ImVec4(1, 1, 1, 0.05f);
  styleColors[ImGuiCol_TabUnfocusedActive] = ImVec4(1, 1, 1, 0.10f);

  styleColors[ImGuiCol_PlotLines] = colors.accentColor;
  styleColors[ImGuiCol_PlotLinesHovered] = colors.accentActive;
  styleColors[ImGuiCol_PlotHistogram] = colors.accentColor;
  styleColors[ImGuiCol_PlotHistogramHovered] = colors.accentActive;

  styleColors[ImGuiCol_TableHeaderBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.0f);
  styleColors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.31f, 1.0f);
  styleColors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.23f, 1.0f);
  styleColors[ImGuiCol_TableRowBg] = ImVec4(0, 0, 0, 0);
  styleColors[ImGuiCol_TableRowBgAlt] = ImVec4(1, 1, 1, 0.02f);

  styleColors[ImGuiCol_TextSelectedBg] = ImVec4(
      colors.accentColor.x, colors.accentColor.y, colors.accentColor.z, 0.35f);
  styleColors[ImGuiCol_DragDropTarget] = colors.accentColor;
  styleColors[ImGuiCol_NavHighlight] = colors.accentColor;
  styleColors[ImGuiCol_NavWindowingHighlight] = colors.accentColor;
  styleColors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
  styleColors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.35f);
}

} // namespace framework
