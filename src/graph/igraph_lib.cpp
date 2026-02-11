#include "igraph_lib.hpp"

namespace ssoc::graph::igraph_ {

void igraph_Deleter::operator()(igraph_t *g) const {
  if (g) {
    igraph_destroy(g);
    delete g;
  }
}

} // namespace ssoc::graph::igraph_
