#pragma once
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace framework {

struct Action {
  std::string id;
  std::string label;
  std::string category;
  std::string shortcut;
  std::function<void()> callback;
};

/**
 * @brief Beheert alle beschikbare acties in de applicatie.
 * Fundering voor de Command Palette.
 */
class ActionRegistry {
public:
  static ActionRegistry &instance() {
    static ActionRegistry inst;
    return inst;
  }

  void registerAction(const std::string &id, const std::string &label,
                      const std::string &category,
                      std::function<void()> callback,
                      const std::string &shortcut = "") {
    actions_[id] = {id, label, category, shortcut, callback};
  }

  void execute(const std::string &id) {
    if (actions_.count(id)) {
      actions_[id].callback();
    }
  }

  const std::unordered_map<std::string, Action> &getActions() const {
    return actions_;
  }

private:
  ActionRegistry() = default;
  std::unordered_map<std::string, Action> actions_;
};

} // namespace framework
