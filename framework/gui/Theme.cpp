#include "Theme.h"

namespace framework {

ThemeColors Theme::colors_;

ThemeColors &Theme::getColors() { return colors_; }

void Theme::applyGlassmorphism() { applyGlassmorphism(colors_); }

void Theme::applyGlassmorphism(const ThemeColors &colors) {
  colors_ = colors;

  ImGuiStyle &style = ImGui::GetStyle();

  // Rounding
  style.WindowRounding = 12.0f;
  style.ChildRounding = 12.0f;
  style.FrameRounding = 12.0f;
  style.GrabRounding = 12.0f;
  style.PopupRounding = 12.0f;
  style.ScrollbarRounding = 12.0f;
  style.TabRounding = 12.0f;

  // Spacing & Padding
  style.WindowPadding = ImVec2(20, 20);
  style.FramePadding = ImVec2(10, 8);
  style.ItemSpacing = ImVec2(12, 10);
  style.ItemInnerSpacing = ImVec2(8, 6);
  style.IndentSpacing = 25.0f;
  style.ScrollbarSize = 16.0f;
  style.GrabMinSize = 12.0f;

  // Borders
  style.WindowBorderSize = 1.0f;
  style.ChildBorderSize = 1.0f;
  style.PopupBorderSize = 1.0f;
  style.FrameBorderSize = 1.0f;

  ImVec4 *styleColors = style.Colors;

  // Glassmorphism Palette
  styleColors[ImGuiCol_Text] = colors.textBase;
  styleColors[ImGuiCol_TextDisabled] = colors.textDim;
  styleColors[ImGuiCol_WindowBg] = colors.glassBg;
  styleColors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.20f);
  styleColors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.12f, 0.95f);
  styleColors[ImGuiCol_Border] = colors.glassBorder;
  styleColors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

  // Inputs (Frames)
  styleColors[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.30f);
  styleColors[ImGuiCol_FrameBgHovered] = ImVec4(0.30f, 0.79f, 0.94f, 0.20f);
  styleColors[ImGuiCol_FrameBgActive] = ImVec4(0.30f, 0.79f, 0.94f, 0.30f);

  // Title Bar
  styleColors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.06f, 0.80f);
  styleColors[ImGuiCol_TitleBgActive] = ImVec4(0.04f, 0.04f, 0.06f, 0.80f);
  styleColors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);

  // Menu
  styleColors[ImGuiCol_MenuBarBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.20f);

  // Scrollbar
  styleColors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.00f);
  styleColors[ImGuiCol_ScrollbarGrab] = ImVec4(1.00f, 1.00f, 1.00f, 0.20f);
  styleColors[ImGuiCol_ScrollbarGrabHovered] = colors.accentColor;
  styleColors[ImGuiCol_ScrollbarGrabActive] = colors.accentActive;

  // Checkmark / Slider
  styleColors[ImGuiCol_CheckMark] = colors.accentColor;
  styleColors[ImGuiCol_SliderGrab] = colors.accentColor;
  styleColors[ImGuiCol_SliderGrabActive] = colors.accentActive;

  // Buttons
  styleColors[ImGuiCol_Button] = ImVec4(0.30f, 0.79f, 0.94f, 0.20f);
  styleColors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.79f, 0.94f, 0.40f);
  styleColors[ImGuiCol_ButtonActive] = ImVec4(0.30f, 0.79f, 0.94f, 0.60f);

  // Headers (CollapsingHeader, etc)
  styleColors[ImGuiCol_Header] = ImVec4(1.00f, 1.00f, 1.00f, 0.05f);
  styleColors[ImGuiCol_HeaderHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
  styleColors[ImGuiCol_HeaderActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.15f);

  // Separator
  styleColors[ImGuiCol_Separator] = colors.glassBorder;
  styleColors[ImGuiCol_SeparatorHovered] = colors.accentColor;
  styleColors[ImGuiCol_SeparatorActive] = colors.accentColor;

  // Resize Grip
  styleColors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  styleColors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
  styleColors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.20f);

  // Tabs
  styleColors[ImGuiCol_Tab] = ImVec4(1.00f, 1.00f, 1.00f, 0.05f);
  styleColors[ImGuiCol_TabHovered] = ImVec4(0.30f, 0.79f, 0.94f, 0.40f);
  styleColors[ImGuiCol_TabActive] = ImVec4(0.30f, 0.79f, 0.94f, 0.60f);
  styleColors[ImGuiCol_TabUnfocused] = ImVec4(1.00f, 1.00f, 1.00f, 0.02f);
  styleColors[ImGuiCol_TabUnfocusedActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);

  // Plot
  styleColors[ImGuiCol_PlotLines] = colors.accentColor;
  styleColors[ImGuiCol_PlotLinesHovered] = colors.textBase;
  styleColors[ImGuiCol_PlotHistogram] = colors.accentColor;
  styleColors[ImGuiCol_PlotHistogramHovered] = colors.accentActive;

  // Table
  styleColors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.20f);
  styleColors[ImGuiCol_TableBorderStrong] = colors.glassBorder;
  styleColors[ImGuiCol_TableBorderLight] = ImVec4(1.00f, 1.00f, 1.00f, 0.05f);
  styleColors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  styleColors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.02f);

  // Text Selection
  styleColors[ImGuiCol_TextSelectedBg] = ImVec4(0.30f, 0.79f, 0.94f, 0.35f);
  styleColors[ImGuiCol_DragDropTarget] = colors.accentColor;
  styleColors[ImGuiCol_NavHighlight] = colors.accentColor;
  styleColors[ImGuiCol_NavWindowingHighlight] = colors.textBase;
  styleColors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);
  styleColors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.60f);
}

} // namespace framework
