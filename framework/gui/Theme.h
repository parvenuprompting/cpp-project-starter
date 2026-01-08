#pragma once

#include "external/imgui/imgui.h"

namespace framework {

enum class ColorRole {
  Accent,
  Surface,
  SurfaceVariant,
  Text,
  TextDisabled,
  Error,
  Success,
  Background
};

/// Glassmorphism theme configuration
struct ThemeColors {
  ImVec4 glassBg = ImVec4(0.08f, 0.08f, 0.12f, 0.70f);
  ImVec4 glassBgHover = ImVec4(0.12f, 0.12f, 0.18f, 0.75f);
  ImVec4 glassBorder = ImVec4(1.00f, 1.00f, 1.00f, 0.15f);
  ImVec4 accentColor = ImVec4(0.30f, 0.79f, 0.94f, 1.00f);  // Soft Cyan
  ImVec4 accentActive = ImVec4(0.45f, 0.04f, 0.72f, 1.00f); // Purple
  ImVec4 textBase = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
  ImVec4 textDim = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
  ImVec4 errorColor = ImVec4(0.92f, 0.26f, 0.35f, 1.00f);
  ImVec4 successColor = ImVec4(0.32f, 0.84f, 0.41f, 1.00f);
};

/// Theme manager for glassmorphism UI
class Theme {
public:
  static void
  applyGlassmorphism(const ThemeColors &colors = getDefaultColors());
  static const ThemeColors &getColors() { return currentColors_; }
  static ImVec4 getColor(ColorRole role);
  static ThemeColors getDefaultColors();

private:
  static ThemeColors currentColors_;
};

} // namespace framework
