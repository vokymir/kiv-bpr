
#include "App.hpp"
#include "Sim_Config.hpp"
#include "graph/Generator.hpp"
int main(void) {

  ssoc::Sim_Config cfg{};

  auto res = ssoc::graph::generate::igraph_from_config(cfg);

  ssoc::App app{};

  app.run(*res);

  return 0;
}
