#include "ResourceManager.h"
#include <iostream>

namespace framework {

ResourceManager &ResourceManager::instance() {
  static ResourceManager instance;
  return instance;
}

void ResourceManager::setExecutablePath(const std::string &path) {
  executablePath_ = std::filesystem::absolute(path).parent_path();
  initializeDefaultPaths();
}

void ResourceManager::addSearchPath(const std::string &path) {
  searchPaths_.push_back(std::filesystem::absolute(path));
}

void ResourceManager::initializeDefaultPaths() {
  searchPaths_.clear();

  // 1. Development: ./resources/ or ./ (relative to current directory)
  searchPaths_.push_back(std::filesystem::current_path() / "resources");
  searchPaths_.push_back(std::filesystem::current_path());

  // 2. macOS Bundle: ../Resources/ (relative to executable)
  if (!executablePath_.empty()) {
    searchPaths_.push_back(executablePath_.parent_path() / "Resources");
  }

  // 3. Executable directory: ./ (relative to executable)
  if (!executablePath_.empty()) {
    searchPaths_.push_back(executablePath_);
  }
}

std::string ResourceManager::resolveResource(const std::string &relativePath) {
  // Try each search path
  for (const auto &searchPath : searchPaths_) {
    auto fullPath = searchPath / relativePath;
    if (std::filesystem::exists(fullPath)) {
      return fullPath.string();
    }
  }

  // Not found - return empty string
  return "";
}

std::string ResourceManager::getFont(const std::string &name) {
  // Try with .ttf extension if not provided
  std::string path = resolveResource(name);
  if (path.empty() && name.find('.') == std::string::npos) {
    path = resolveResource(name + ".ttf");
  }
  return path;
}

std::string ResourceManager::getImage(const std::string &name) {
  // Try common image extensions
  std::string path = resolveResource(name);
  if (path.empty() && name.find('.') == std::string::npos) {
    const char *extensions[] = {".png", ".jpg", ".jpeg", ".bmp"};
    for (const char *ext : extensions) {
      path = resolveResource(name + ext);
      if (!path.empty())
        break;
    }
  }
  return path;
}

std::string ResourceManager::getConfig(const std::string &name) {
  // Try with .ini extension if not provided
  std::string path = resolveResource(name);
  if (path.empty() && name.find('.') == std::string::npos) {
    path = resolveResource(name + ".ini");
  }
  return path;
}

} // namespace framework
