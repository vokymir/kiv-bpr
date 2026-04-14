
// library to create graphs
#import "@preview/fletcher:0.5.8" as fletcher: diagram, edge, node

// my own small lib to easily create diagrams
#import "diagram_functions.typ": boundary-style, square-lattice, uniform-style

#set text(lang: "en")
#set heading(numbering: "1.")
#set math.equation(numbering: "(1)")

// highlight all TODOs, which is WOW, easy yet not
#show regex("TODO(.*)"): it => text(fill: red, weight: "bold")[#it]

#title([
  Self-Organized Criticality: Sandpile Model on Undirected Graph, Influence of
  Topology and Dissipation Rules on Avalanches
])

= Introduction

Almost everything in nature tends to cluster around a center value. An example
might be the height of humans. This is described by a normal distribution which
itself got its name because this pattern is so often observed not only in nature
but in human-made systems as well.

Contrary to this, there are systems which could not be described by normal
distribution. One example are the forest fire sizes. If they were to adhere to
normal distribution, there would be known an _average forest fire size_. Even
though most of forest fires are small, there is nothing limiting the potential
size. This is one of key properties of systems better described by power-law,
scale-invariance.

Normal distribution is _heavy tailed_, meaning that most of values are
distributed within small range around the center (mean) and
values far from it are very impropable. The probability density function of
normal distribution is described in @eq:normal_distribution

$
        f(x) & = 1 / sqrt(2 pi sigma^2) e^(- (x - mu)^2 \/ (2 sigma^2)) \
  mu #h(1mm) & ... #text[mean] \
     sigma^2 & ... #text[variance]
$ <eq:normal_distribution>

TODO: plot normal dist?

As opposed to this, power-law  #footnote[Power-law on $(0, infinity)$ cannot be
  a probability distribution, because the areas near 0 and under the tail are
  infinite. However, with simple restrictions: $f(x) = a x^(-k), quad k > 1, quad
  x > x_(min)$ it is a valid distribution.] is _long tailed_. Its exponential
character described in @eq:power-law result in majority of the values being very
far. That corresponds with the scale-invariance: Any (forest fire) size is
realistically possible, even though smaller sizes are more likely. Power-law
exhibit linear relationship between $log f(x)$ and $log x$. In a `log-log plot`
it forms a straight line.

$
       f(x) & = a x^(-k) \
  k #h(1mm) & ... #text[constant exponent]
$ <eq:power-law>

TODO: plot power-law? in normal and log-log plot

The emergence of power-law distribution is often connected to systems critical
state. Consider phase transition of water from liquid to vapor. At the
liquid-vapour boundary curve, both liquid and vapour can coexist but are
distinctly separate. The boundary terminates at some critical temperature and
critical pressure at which the critical point of a system is defined. While the
system is in the critical state, the water is in not in any one state.

Systems in critical state are most responsive to input and behaves
unpredictable. This is the case for phase transition and forest fires. Although,
there is a significant difference. Criticality can be reached in many systems by
fine-tuning certain parameters. In the case of phase transition the parameters
were pressure and temperature. However, systems exist which reach criticality
independently of parameters.

TODO NOTE?: usually high degree of freedom systems

The phenomenon of systems reaching criticality by themselves is called
_self-organized criticality (SOC)_. The concept of SOC was first discovered in
1987 by Bak, Tang and Wiesenfeld and its properties were demonstrated on a
sandpile model @1987PhRvL..59..381B. This model and its properties is the
main focus of this thesis.

TODO: talk about the idea of putting it on random graphs, not just general?

The original sandpile model was a cellular automaton on a sqare and cubic
lattice. However, the idea is to use another underlying graph structures for the
automaton. Before this generalization we introduce few needed concepts from
graph theory.

= Graph Theory

An undirected graph is defined as a pair, containing set of vertices and set
of edges. The graph has $n$ vertices and $m$ edges.
$
  G & = (V,E) \
  V & = {v_1, v_2, ..., v_n} \
  E & subset.eq { {u, v} | u, v in V, u != v }, quad abs(E) = m
$

The neighbourhood of a vertex is a set of vertices connected to it via an edge.
$
  "nei" (v_i) = {v_j | {v_i, v_j} in E},
$

The vertex degree is defined as the size of vertex neighbourhood.
$
  d(v_i) = abs("nei" (v_i))
$

Path between two vertices $v_i$ and $v_j$ is a sequence of vertices beginning in
the first and ending in the second where every two vertices in path must be
directly connected.
$
  P = (v_1, v_2, ..., v_k), quad v_1 = v_i, quad v_k = v_j, \
  quad {v_l, v_(l+1)} in E quad forall l in {1, ..., l-1}
$
If there exist no such path, let $P = ()$.

Similarily, path length is the number of _steps_ (edges) needed to walk the
path. If path doesn't exist we define its legth as zero.
$
  "len"(P) = cases(
    abs(P) - 1 quad & abs(P) >= 2,
    0 quad & "else"
  )
$

The shortest path between vertices $v_i$ and $v_j$ is the path with minimal
length among all paths connecting these vertices. Let $PP(v_i, v_j)$ be the set
of all paths between $v_i$ and $v_j$:
$
  P^*_(i j) = min_(P in PP(v_i, v_j)) "len"(P)
$

A graph has an average (shortest) path length $"len"(G)$. Let $PP^*$ be the set
of all shortest paths:
$
  "len"(G) = 1/n(n-1) Sigma_(i != j) "len"(P^*_(i j))
$

TODO: small-world network properties (the previous needed for that)

= Sandpile Model <h:sandpile_model_general>

The very original sandpile model is a cellular automaton on a lattice. Every
cell can hold up to $K$ grains, the height of a cell is a function of position
$z(x,y,...)$. Fixed boundary conditions are used, on a boundary $z = 0$ cannot
be changed. In two dimensions $z$ is updated as follows:
$
                z(x,y) & arrow z(x,y) - 4 \
  z(x plus.minus 1, y) & arrow z(x plus.minus 1, y) + 1 \
  z(x, y plus.minus 1) & arrow z(x, y plus.minus 1) + 1
$ <eq:cell-automaton_topple_rules_2d>
if $z$ exceeds (or meets) critical value $K = 4$. The system is initialized
randomly but with $z >> K$. That initial state or rather the size of a lattice
along with $ZZ$ (the height of all cells) is called _configuration_.

For simplicity we can imagine a simple chessboard and falling sand grains. Each
grain falls on a chess square determined by a random distribution #footnote[In
  the original model, all sand grains were distributed initially. It might be
  more obvious if we let grains fall one at a time for demonstration purposes.].
If the number of grains on any square reaches four, the sandpile topples and
the four grains fall on adjacent squares, possibly causing additional squares to
topple. In case the square does not have four adjacent squares (i.e. it is on
the edge of chessboard) the remaining grains fall off the board and are
therefore removed from the model.

We say that cell (square) is stable if it contains less than $K$ grains. The
system is stable if all of its cells are stable. If a grain is added to any
stable cell, it can destabilize it resulting in an avalanche. Avalanche is the
set of topplings which occur before the system is stabilized again.

The sandpile model is a dissipative dynamical system. The dissipation is
demonstrating on the boundaries. When a grain leaves the chessboard (or enter a
boundary cell) it dissipated from the system. Dissipation is an important
property of the model. Without dissipation the model supersaturates, leading to
one infinite avalanche.


We can express the square lattice (chessboard) as a graph $G = (V,E)$ with size
#footnote[In a square lattice, the $"size"^2 = n$, which is the number of
  vertices in graph.] $sqrt(n)$. For a boundary condition, the boundary vertices
height is fixed to zero $z = 0$. Alternatively, we may imagine only one vertex,
called _sink vertex_, with such condition. This sink is connected to all
boundary vertices, thus creating the same effect. Please note, that this
simplification creates a multigraph, because corner vertices and sink are
connected via two edges. For the rest of this thesis we will consider the
simplified version with one sink vertex and therefore a multigraph. However, it
is trivial to create multiple sink vertices to restore a simple undirected
graph.

#figure(
  caption: "Comparison of square lattice sink vertices (blue). On the left is a
  graph with sinks at the boundary - as in the original model. On the right is
  a multigraph, square lattice with only one sink connected to all vertices at
  the boundary which is hinted by outgoing edges.",

  grid(
    columns: (1fr, 1fr),
    gutter: 2em,
    align: center,

    diagram(
      node-shape: circle,
      node-fill: none,
      node-stroke: 1pt + black,
      node-inset: 0pt,
      spacing: 10pt,
      label-size: 0pt,

      ..square-lattice(
        5,
        5,
        styles-fn: (x, y, n, m) => boundary-style(
          x,
          y,
          5,
          5,
          boundary-fill: rgb(39, 245, 245),
          inner-fill: none,
        ),
      ),
    ),

    diagram(
      node-shape: circle,
      node-fill: none,
      node-stroke: 1pt + black,
      node-inset: 0pt,
      spacing: 10pt,
      label-size: 0pt,

      ..square-lattice(
        5,
        5,
        skip-boundary-edges: true,
        styles-fn: (x, y, n, m) => boundary-style(
          x,
          y,
          5,
          5,
          boundary-fill: none,
          boundary-stroke: none,
        ),
      ),

      node(
        (-2, 2), //
        width: 6pt,
        height: 6pt,
        fill: rgb(39, 245, 245),
      ),

      edge((-2, 2), (-1, 2)),

      // ..square-lattice-with-sink(3, 3, sink-color: rgb(39, 245, 245)),
    ),
  ),
) <fig:sq-lat_comp_sinks>


All properties of the cellular automaton system holds on a square lattice. This
model was later generalized from square lattice to arbitrary graph
@Holroyd_2008. That generalization is important because it allows us to study
the models behaviour for different graph topologies.

== Graph Topologies

Standard choice for graph in sandpile model is square lattice with arbitrary
size.

TODO: square lattice - properties

TODO: ER graph - we don't have

TODO: WS graph - we have

== Dissipation Rules <h:dissipation_rules>

With the introduction of arbitrary graph we must reconsider dissipation rules.
The square lattice has explicit boundaries which serves as sinks. However, many
graphs do not even have an implicit boundary. The existence of sink is
fundamental for the model not to supersaturate.

A dissipation rule determines how many times a vertex should be connected to the
sink. There is not a single rule which would be generally applied to any graph.
We will discuss different rules, their properties and analogies.

As discussed in @h:sandpile_model_general the existence of a sink vertex is
necessary for any graph representation of a sandpile model. However there are
various rules by which you can determine if a given vertex should be connected
to a sink vertex and how many times if so.

For example the original model uses a rule (named by me as) _Fill to Four_. This
rule states that the degree of vertex must be exactly four. The sink vertex is
connected to all vertices as many times it is needed to fulfill that rule. For
the square lattice, only vertices on the edge are connected (exactly once except
for corner vertices, which are connected twice). This rule precisely reproduces
the original chessboard idea.

For an arbitrary graph though, the rule is insufficient. It does not ensure
dissipation which can lead to graph supersaturation and never-ending avalanche.
This rule does not reflect on the vertex degree if it is higher than four.

One possible rule which guarantees dissipation on any graph may be _All Once_.
The rule is fair in a sense that all vertices have connection to the sink and is
uniform because every vertex, no matter its degree, is connected exactly once.

Finally, another rule which assures dissipation is _As Many As Neighbours_. This
rule connects every vertex to sink proportionally to the number of its
neighbouring vertices, therefore emphasizing how many connections it has.

Every rule represents a different approach and not all can be applied to all
types of graph topology.


= Observed Behaviour

TODO: how did it work for different types of graphs and dissipation rules? also
the graph configurations (eg $beta$ for WS)

TODO: metrics, how it changed criticality? origin distribution, avalanche size
(max, avg)

= Technical Documentation

TODO

#bibliography("refs.bib")

#heading([TODO], numbering: none)

- multigraph?

#heading([TODO PS], numbering: none)

- oponent
- předběžná verze L. Vášovi
