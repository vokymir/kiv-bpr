#pragma once

#include "Graph.hpp"
#include <memory>
namespace ssoc::graph::generate {

std::unique_ptr<Graph> from_config();

} // namespace ssoc::graph::generate
