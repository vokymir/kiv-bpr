
#include "App.hpp"
#include <print>
int main(void) {
  std::print("before app");
  ssoc::App app{};
  std::print("after app");

  app.run();

  return 0;
}
