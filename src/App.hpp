#pragma once

#include "ui/Ui.hpp"
namespace ssoc {

/* Orchestrates everything. */
class App {
private:
  ui::UI ui_{};

public:
  void run();
};

} // namespace ssoc
