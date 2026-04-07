#set text(lang: "en")
#set heading(numbering: "1.")
#set math.equation(numbering: "1.")

#show ref: it => {
  if it.has("element") and it.element != none and it.element.func() == heading {
    let loc = it.element.location()
    let page = loc.page()

    link(
    loc,
    [_#it.element.body (page #page)_]
    )
  } else {
    it
  }
}

#title([
Self-Organized Criticality: Sandpile Model on Undirected Graph, Influence of
  Topology and Dissipation Rules on Avalanches
])

= Introduction

Natural systems with high degree of freedom such as earthquakes often exhibit a
tendency to reach a critical state. This phenomenon is called self-organized
criticality. These systems most famously need no fine-tuning in order to reach
the critical state, rather it is an inherit property of theirs. Systems
in a critical state are very sensitive to inputs and this leads to
its unpredictability.

The concept of SOC was first discovered in 1987 by Bak, Tang and Wiesenfeld and
its property demonstrated on a sandpile model @1987PhRvL..59..381B. This model
was later generalized from square lattice to arbitrary graph. That
generalization is important because it allows us to study the models behaviour
for different graph topologies.

= Background

== Sandpile Model <h:sandpile_model_general>

The original sandpile model presents a simple chessboard and a falling sand
grains. Each grain fall on some chess square determined by a random
distribution. If the number of grains on any square reach four, the sandpile
topples and the four grains fall on adjoint squares, possibly causing additional
squares to topple. In case the square does not have four adjacent squares (ie it
is on the edge of chessboard) the remaining grains fall off the board and are
therefore removed from the model.

This model can be expressed as an undirected graph, a square lattice of the size
$8 times 8$. This however is not a correct representation until an additional
vertex is introduced. The often called sink vertex have an important property
that it cannot be toppled. When the sink is connected to every vertex on the
edge of the square lattice (twice to corner vertices) the graph representation
becomes valid. This is foundational observation, for it allows us to use any
underlying graph for the sandpile model.

TODO: sink is needed for the model to not end-up in infinite loop

== Self-Organized Criticality
TODO: critical state, power-law

== Graph Theory

In order to explore the behaviour of the sandpile model on a graph, we must
estabilish these concepts. First is _vertex degree_ $d(v_i)$ which is defined as
equal to the number of vertices connected to $v_i$ via an edge.

_Path legth_ describes the distance between two vertexes for a given path. The
path length is equal to the number of vertices in between the first and last
vertex on a path. The _shortest path length_ refers to a distance between two
vertexes such as any other path between them produces at least the same path
length. It's useful to also define an _average shortest path length_, an average
of shortest paths between any two vertexes in a graph.

TODO: small-world network properties (the previous needed for that)

= Model Definition

== Sandpile on Graphs

As discussed in @h:sandpile_model_general the existence of a sink vertex is
necessary for any graph representation of a sandpile model.

#bibliography("refs.bib")
