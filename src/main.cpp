
#include "App.hpp"
#include <backward.hpp>

// register this for beautiful error outputs
backward::SignalHandling sh;

int main(void) {
  ssoc::App app{};

  app.run();

  return 0;
}
