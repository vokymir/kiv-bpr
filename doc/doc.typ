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
the critical state, rather it is an inherent property of theirs. Systems in a
critical state are very sensitive to inputs and this leads to its
unpredictability.

The concept of SOC was first discovered in 1987 by Bak, Tang and Wiesenfeld and
its properties were demonstrated on a sandpile model @1987PhRvL..59..381B. This
model was later generalized from square lattice to arbitrary graph. That
generalization is important because it allows us to study the model's behaviour
for different graph topologies.

= Background

== Sandpile Model <h:sandpile_model_general>

The original sandpile model presents a simple chessboard and a falling sand
grains. Each grain falls on some chess square determined by a random
distribution. If the number of grains on any square reaches four, the sandpile
topples and the four grains fall on adjacent squares, possibly causing
additional squares to topple. In case the square does not have four adjacent
squares (i.e. it is on the edge of chessboard) the remaining grains fall off the
board and are therefore removed from the model. (TODO dissipation here???)

This model can be expressed as an undirected graph, a square lattice of the size
$8 times 8$. This however is not a correct representation until an additional
vertex is introduced. The often called sink vertex has an important property
that it cannot be toppled. When the sink is connected to every vertex on the
edge of the square lattice (twice to corner vertices) the graph representation
becomes valid. This is a foundational observation, for it allows us to use any
underlying graph for the sandpile model.

TODO: define avalanche

TODO: sink is needed for the model to not end-up in infinite loop - dissipation

TODO: what is dissipation and why is it needed (as before)

TODO: at least one vertex connected to sink needed (or on random graph one
connection to the sink for every vertex is needed)

== Self-Organized Criticality
TODO: critical state, power-law

== Graph Theory

In order to explore the behaviour of the sandpile model on a graph, we must
establish these concepts. First is _vertex degree_ $d(v_i)$ which is defined as
equal to the number of vertices connected to $v_i$ via an edge.

_Path length_ describes the distance between two vertices for a given path. The
path length is equal to the number of vertices in between the first and last
vertex on a path. The _shortest path length_ refers to a distance between two
vertices such as any other path between them produces at least the same path
length. It's useful to also define an _average shortest path length_, an average
of shortest paths between any two vertices in a graph.

TODO: small-world network properties (the previous needed for that)

= Model Definition

TODO: how can the model be extended, rules and graphs. foundation for it is in
@h:sandpile_model_general

== Dissipation Rules

As discussed in @h:sandpile_model_general the existence of a sink vertex is
necessary for any graph representation of a sandpile model. However there are
various rules by which you can determine if a given vertex should be connected
to a sink vertex and how many times if so.

For example the original model uses a rule (named by me as) _Fill to Four_. This
rule states that the degree of vertex must be exactly four. The sink vertex is
connected to all vertices as many times is needed to fulfill that rule. For the
square lattice, only vertices on the edge are connected (exactly once except for
corner vertices, which are connected twice). This rule precisely reproduces the
original chessboard idea.

For an arbitrary graph though, the rule is insufficient. It doesn't ensure
dissipation which can lead to graph supersaturation and never-ending avalanche.
This rule doesn't reflect on the vertex degree if that is higher than four.

One possible rule which guarantees dissipation on any graph may be _All Once_.
The rule is fair in a sense that all vertices have connection to the sink and is
uniform because every vertex, no matter its degree, is connected exactly once.

Finally another rule which assure dissipation is _As Many As Neighbours_. This
rule connects every vertex to sink proportionally to the number of its
neighbouring vertices, therefore emphasizing how many connections it has.

Every rule represents a different approach and not all can be applied to all
types of graph topology.

== Underlying Graphs

Standard choice for graph in sandpile model is square lattice with arbitrary
size.

TODO: square lattice - properties

TODO: ER graph - we don't have

TODO: WS graph - we have

= Observed Behaviour

TODO: how did it work for different types of graphs and dissipation rules? also
the graph configurations (eg $beta$ for WS)

TODO: metrics, how it changed criticality? origin distribution, avalanche size
(max, avg)

= Technical Documentation

TODO

#bibliography("refs.bib")
