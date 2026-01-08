#include "../../external/catch2/catch_amalgamated.hpp"
#include "../../framework/gui/Theme.h"

using Catch::Approx;

TEST_CASE("Theme colors can be customized", "[theme]") {
  framework::ThemeColors colors;
  colors.accentColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red

  // Apply theme with custom colors
  framework::Theme::applyGlassmorphism(colors);

  // Verify colors were applied
  auto &appliedColors = framework::Theme::getColors();
  REQUIRE(appliedColors.accentColor.x == 1.0f);
  REQUIRE(appliedColors.accentColor.y == 0.0f);
  REQUIRE(appliedColors.accentColor.z == 0.0f);
}

TEST_CASE("Default glassmorphism theme applies", "[theme]") {
  framework::Theme::applyGlassmorphism();

  auto &colors = framework::Theme::getColors();

  // Check default accent color (Soft Cyan)
  REQUIRE(colors.accentColor.x == Approx(0.30f));
  REQUIRE(colors.accentColor.y == Approx(0.79f));
  REQUIRE(colors.accentColor.z == Approx(0.94f));
}
