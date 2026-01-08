#include "SimpleViewerApp.h"

namespace framework {
// Factory function defined in main_template.cpp
IClientApp *createClientApp() { return new demo::SimpleViewerApp(); }
} // namespace framework
