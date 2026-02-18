#pragma once

#include <algorithm>
#include <functional>
#include <vector>
namespace ssoc::stat {

template <typename TEvent> class Event_Bus {
  using Handler = std::function<void(const TEvent &)>;

protected:
  std::vector<Handler> handlers_;

public:
  void subscribe(Handler h) { handlers_.push_back(std::move(h)); }

  void emit(const TEvent &e) const {
    for (auto &h : handlers_) {
      h(e);
    }
  }
};

} // namespace ssoc::stat
