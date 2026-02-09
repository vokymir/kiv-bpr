#pragma once

#include <utility>
#include <vector>
namespace ssoc::graph {

/* Store information related to graphs layout in UI. */
class Geometry{
private:
    std::vector<std::pair<int, int>> positions_;

};

}
