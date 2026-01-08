#pragma once
#include <functional>
#include <memory>
#include <mutex>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>

namespace framework {

/**
 * @brief Een lichtgewicht, type-safe Event Bus voor ontkoppelde communicatie.
 */
class EventBus {
public:
  static EventBus &instance() {
    static EventBus inst;
    return inst;
  }

  // Voorkom kopieëren
  EventBus(const EventBus &) = delete;
  EventBus &operator=(const EventBus &) = delete;

  /**
   * @brief Abonneer op een specifiek event type.
   * @tparam T Het type van het event (bijv. een struct).
   */
  template <typename T> void subscribe(std::function<void(const T &)> handler) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto index = std::type_index(typeid(T));
    handlers_[index].push_back([handler](const void *event) {
      handler(*static_cast<const T *>(event));
    });
  }

  /**
   * @brief Publiceer een event naar alle abonnees.
   */
  template <typename T> void publish(const T &event) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto index = std::type_index(typeid(T));
    if (handlers_.find(index) != handlers_.end()) {
      for (auto &handler : handlers_[index]) {
        handler(&event);
      }
    }
  }

private:
  EventBus() = default;

  using InternalHandler = std::function<void(const void *)>;
  std::unordered_map<std::type_index, std::vector<InternalHandler>> handlers_;
  std::mutex mutex_;
};

} // namespace framework
