
#include "App.hpp"
#include "Sim_Config.hpp"
#include "graph/Generator.hpp"
int main(void) {

  ssoc::Sim_Config cfg{};

  ssoc::graph::generate::igraph_from_config(cfg);

  ssoc::App app{};

  app.run();

  return 0;
}
