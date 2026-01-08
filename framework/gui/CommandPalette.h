#pragma once
#include <string>
#include <vector>

namespace framework {

class CommandPalette {
public:
  static void render();
  static void toggle() { isOpen_ = !isOpen_; }
  static bool isOpen() { return isOpen_; }

private:
  static bool isOpen_;
  static char searchBuf_[128];
  static int selectedIndex_;
};

} // namespace framework
