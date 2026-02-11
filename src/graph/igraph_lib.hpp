#pragma once

#include "igraph.h"
#include <memory>
// underscore to make obvious its custom namespace for igraph
namespace ssoc::graph::igraph_ {

// custor raii functor deleter to allow unique_ptr to hold igraph
struct igraph_Deleter {
  void operator()(igraph_t *g) const;
};

// shorthand for using unique_ptr with igraph
using unique_ptr_ = std::unique_ptr<igraph_t, igraph_Deleter>;

} // namespace ssoc::graph::igraph_
