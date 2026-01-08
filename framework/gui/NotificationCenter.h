#pragma once
#include <chrono>
#include <string>
#include <vector>

namespace framework {

enum class NotificationType { Info, Success, Warning, Error };

struct Notification {
  std::string title;
  std::string message;
  NotificationType type;
  std::chrono::steady_clock::time_point startTime;
  float duration; // seconds
};

/**
 * @brief Beheert non-blocking "Toast" notificaties die bovenop de UI zweven.
 */
class NotificationCenter {
public:
  static NotificationCenter &instance() {
    static NotificationCenter inst;
    return inst;
  }

  void post(const std::string &title, const std::string &message,
            NotificationType type = NotificationType::Info,
            float duration = 4.0f);

  void info(const std::string &m) { post("Info", m, NotificationType::Info); }
  void success(const std::string &m) {
    post("Success", m, NotificationType::Success);
  }
  void error(const std::string &m) {
    post("Error", m, NotificationType::Error);
  }

  void render();

private:
  NotificationCenter() = default;
  std::vector<Notification> notifications_;
};

} // namespace framework
