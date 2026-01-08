#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace framework {

/// Resource manager for resolving asset paths
/// Searches multiple locations to find resources
class ResourceManager {
public:
  static ResourceManager &instance();

  /// Set the executable path (call from main with argv[0])
  void setExecutablePath(const std::string &path);

  /// Add a custom search path
  void addSearchPath(const std::string &path);

  /// Resolve a resource path by searching in multiple locations
  /// Returns absolute path if found, empty string otherwise
  std::string resolveResource(const std::string &relativePath);

  /// Convenience methods for common resource types
  std::string getFont(const std::string &name);
  std::string getImage(const std::string &name);
  std::string getConfig(const std::string &name);

private:
  ResourceManager() = default;

  std::vector<std::filesystem::path> searchPaths_;
  std::filesystem::path executablePath_;

  void initializeDefaultPaths();
};

} // namespace framework
