
#include "App.hpp"

#ifdef USE_BACKWARD
#include <backward.hpp>
backward::SignalHandling sh;
#endif

int main(void) {
  ssoc::App app{};

  app.run();

  return 0;
}
