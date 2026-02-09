#pragma once

#include <vector>
namespace ssoc::graph {

/* Store the state of sand on every node in graph. */
class SandState{
private:
    std::vector<int> counts_;
};

}
