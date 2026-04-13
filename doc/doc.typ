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
        f(x) & = 1 / sqrt(2 pi sigma^2) exp (- (x - mu)^2 / (2 sigma^2)) \
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

TODO: where to put

This model was later generalized from square lattice to arbitrary graph
@Holroyd_2008. That generalization is important because it allows us to study
the model's behaviour for different graph topologies.

The original sandpile model presents a simple chessboard and falling sand
grains. Each grain falls on a chess square determined by a random distribution.
If the number of grains on any square reaches four, the sandpile topples and the
four grains fall on adjacent squares, possibly causing additional squares to
topple. In case the square does not have four adjacent squares (i.e. it is on
the edge of chessboard) the remaining grains fall off the board and are
therefore removed from the model.

The topology of this model can be expressed as an undirected graph, a square
lattice of the size $8 times 8$. However, it is not a correct representation
until an additional vertex is introduced. The often called sink vertex has an
important property that it cannot be toppled. When the sink is connected to
every vertex on the edge of the square lattice (twice to corner vertices) the
graph representation becomes valid #footnote([If a sink is connected to any
  vertex multiple times, the graph becomes multigraph. However, that is not a
  bad thing for this thesis where the important thing is that the topology
  itself is a graph. The different ways of connecting a sink to vertices are
  explored in @h:dissipation_rules. The word graph is used interchangibly with
  multipgraph.]). This is a foundational observation, for it allows us to use
any underlying graph for the sandpile model.

There are two possible state for any vertex. It is stable if it contains less
grains than its degree. That is, if the number of edges from that vertex
(including edges to sink) is higher than the number of grains. Otherwise the
vertex is considered unstable. The system is stable if all vertices are stable.

If a grain is added to any stable vertex, it can destabilize it resulting in an
avalanche. Avalanche is the set of topplings which occur before the system is
stabilized again. One extreme case is an avalanche of size one, which is only
the origin of avalanche toppling. The other extreme for chessboard is an
avalanche of size $64$, which is the whole board toppled.

TODO: proof?

The process of a grain leaving the board is called dissipation and it is an
important property of the model. Without dissipation the model supersaturates,
leading to one infinite avalanche. For a connected graph (i.e. graph where for
every pair of vertices exist at least one path from one to another) to dissipate
at least one vertex connected to a sink is necessary. For any arbitrary graph
the requirement heighten to every vertex being connected to a sink.


TODO: how can the model be extended, rules and graphs. foundation for it is in
@h:sandpile_model_general

== Dissipation Rules <h:dissipation_rules>

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

#heading([TODO], numbering: none)

- multigraph?

#heading([TODO PS], numbering: none)

- oponent
- předběžná verze L. Vášovi
