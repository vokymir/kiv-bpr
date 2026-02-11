#pragma once

#include "igraph.h"
#include <memory>
namespace ssoc::graph::igraph_ {

// custor raii deleter to allow unique_ptr
struct igraph_Deleter {
  void operator()(igraph_t *g) const;
};

using unique_ptr_ = std::unique_ptr<igraph_t, igraph_Deleter>;

} // namespace ssoc::graph::igraph_
