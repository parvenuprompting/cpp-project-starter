#pragma once
#include <filesystem>
#include <string>
#include <vector>

namespace framework {

class GuiUtils {
public:
  static bool renderFilePicker(const char *popupName, bool &isOpen,
                               std::string &outPath);
  static void renderCenteredText(const char *text, float yOffset = 0.0f);
  static void renderSeparatorWithText(const char *text);
};

} // namespace framework
