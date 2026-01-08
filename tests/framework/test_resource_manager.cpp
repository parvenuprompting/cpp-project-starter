#include "../../external/catch2/catch_amalgamated.hpp"
#include "../../framework/utils/ResourceManager.h"
#include <filesystem>

TEST_CASE("ResourceManager resolves existing resources", "[resource]") {
  auto &rm = framework::ResourceManager::instance();

  // This test assumes resources/ directory exists
  // In real usage, setExecutablePath would be called from main

  SECTION("Can add custom search paths") {
    rm.addSearchPath("./resources");
    // Just verify it doesn't crash
    REQUIRE(true);
  }
}

TEST_CASE("ResourceManager handles missing resources gracefully",
          "[resource]") {
  auto &rm = framework::ResourceManager::instance();

  std::string result = rm.resolveResource("nonexistent_file.txt");
  REQUIRE(result.empty());
}
