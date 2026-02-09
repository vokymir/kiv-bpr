#pragma once

#include <vector>
namespace ssoc::graph{

/* Store the topology of graph, the nodes in neighbours. */
class Topology{
private:
    /* How many nodes are in a graph. */
    int node_count_;
    /* Store on which index in neighbours_ vector starts the list of neighbours
     * for current node.
     *
     * == Example:
     * node_count = 4
     * neighbour_idxs_ =
     * [0,2,2,4] <- at which index neighbour starts
     * neighbours_ =
     *  0 1 2 3 4 5 6   <- indexes
     *  0 0 2 2 3 3 3   <- whose neighbour list is it
     * [1,2,1,0,1,0,3]
     * */
    std::vector<int> neighbour_idxs_;
    /* Neighbours list, to know how many neighbours for node exists, look into
     * neighbour_idxs_ list. */
    std::vector<int> neighbours_;

};

}

