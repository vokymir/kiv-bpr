
// my own modules for various graphs
#import "diagrams.typ": diagram
#import "plots.typ": plot

#set text(lang: "en")
#set heading(numbering: "1.")
#set math.equation(numbering: "(1)")
#set page(
  paper: "a4",
  footer: place(center, context [#counter(page).display("1")]),
)

// highlight all TODOs, which is WOW, easy yet not
#show regex("TODO(.*)"): it => text(fill: red, weight: "bold")[#it]

// hide all figures and math (useful to count normo-pages)
// #show figure: it => []
// #show math.equation: it => []

// ensure all inline math equation won't break at the end of line
#show math.equation: it => box(it)
// ensure all bold expressions (almost sub-headings) won't break
#show strong: it => box(it)

// references should stay as close to default as possible, here is a list of
// needed changes:
// 1. Equation 1 => Equation (1)
#show ref: it => {
  let el = it.element

  if el == none {
    return it
  }

  let loc = it.element.location()

  if el.func() == math.equation {
    let num = counter(math.equation).at(loc)

    return link(
      loc,
      [Equation~#context { numbering(math.equation.numbering, ..num) }],
    )
  }

  it
}

#title([
  Self-Organized Criticality: Sandpile Model on Undirected Graph, Influence of
  Topology and Dissipation Rules on Avalanches
])

= Introduction <h:intro>

In everyday life, person's outburst is rarely caused by a single large event.
Rather, the tension builds up from small frustrations over time. When certain
treshold is reached, even a minor additional stress can trigger an outburst.
That redistributes the tension to others in their social circle. This may in
turn cause further reactions, leading to a cascade of reach and size which are
difficult to predict. A local event may therefore propagate through a network,
depending on individual thresholds and the shape of the network.

This may seem irregular or unpredictable, but many complex systems exhibit
stable statistical properties when the number of observation is large. This
phenomenon is explained by the law of large numbers @Durrett_2019 which applies,
for example, to repeated coin tosses, dice rolls and also many natural
phenomenon such as average temperature.

Furthermore, if many independent effects contribute to an outcome, the
resulting distribution is often well approximated by a normal distribution
according to central limit theorem @Durrett_2019. A common example is human
height which clusters around an _average_ with roughly symmetric variation.

#figure(
  caption: [Normal distribution plot.],
  gap: 1.5em,
  (plot.normal)((:)),
) <fig:normal_distribution>

Contrary to this, there are systems which could not be described by normal
distribution @doi:10.1137_070710111 . One example are the forest fire sizes
@doi:10.1126_science.281.5384.1840. If they were to adhere to normal
distribution, there would be a known _average forest fire size_. Even though
most forest fires are small, there is nothing limiting the potential size. This
is one of the key properties of systems better described by the power law,
scale-invariance.

Normal distribution is exponentially bounded, meaning that most of values are
distributed within small range around the center (mean) and values far from it
are very impropable. The probability density function of normal distribution
with mean $mu in RR$ and variance $sigma^2 > 0$ is described in
@eq:normal_distribution.

$
  f(x) & = 1 / sqrt(2 pi sigma^2) e^(- (x - mu)^2 \/ (2 sigma^2)) #h(1mm) .
$ <eq:normal_distribution>

As opposed to this, the power law #footnote[Power law on $(0, infinity)$ cannot
  be a probability distribution, because the areas near 0 and under the tail are
  infinite. However, with simple restrictions: $f(x) = a x^(-k), quad k > 1,
  quad x > x_(min)$ it is a valid distribution. TODO: nerozumím poznámce; stačí
  asymptoticky] is _heavy tailed_. Its
exponential character described in @eq:power-law result in higher probability
of extreme values. That corresponds with the scale-invariance: Any (forest
fire) size is realistically possible, even though smaller sizes are more likely.
Power law exhibit linear relationship between $log f(x)$ and $log x$. In a
`log-log plot` it forms a straight line, as can be seen on @fig:cmp_scale.

$
       f(x) & = a x^(-k) \
  k #h(1mm) & ... #text[constant exponent]
$ <eq:power-law>

The emergence of the power law distribution is often connected to systems
critical state. Consider phase transition of water from liquid to vapor. At the
liquid-vapour boundary curve, both liquid and vapour can coexist but are
distinctly separate. The boundary terminates at some critical temperature and
critical pressure at which the critical point of a system is defined. While the
system is in the critical state, the water is not in a single state.

#figure(
  caption: [Function $y = x^(-2)$ plotted on linear scale (left) and on
    log-log scale (right) on~interval~$chevron.l 1, 10 chevron.r$.],
  gap: 1.5em,
  grid(
    gutter: 2em,
    columns: 2,
    (plot.cmp_linear)((:)), (plot.cmp_loglog)((:)),
  ),
) <fig:cmp_scale>

Systems in critical state are most responsive to input and behave unpredictably
@Bak1996. This is the case for phase transition and forest fires. However,
there is a significant difference. Criticality can be reached in many systems by
fine-tuning certain parameters. In the case of phase transition the parameters
are pressure and temperature. However, there exist systems which reach
criticality independently of parameters.

The phenomenon of systems reaching criticality by themselves is called
_self-organized criticality (SOC)_. The concept of SOC was first discovered in
1987 by Bak, Tang and Wiesenfeld and its properties were demonstrated on a
sandpile model @1987PhRvL..59..381B. The original model was a cellular automaton
on a square and cubic lattice.

In this thesis, we investigate the behaviour of the sandpile model on different
graph structures. Specifically, on small-world networks which are a
class of random networks. Before this generalization we introduce few necessary concepts from
graph theory.

= Graph Theory <h:graph_theory>

An undirected graph is defined as a pair, containing set of vertices and set
of edges. The graph has $n$ vertices and $m$ edges.
$
  G & = (V,E) \
  V & = {v_1, v_2, ..., v_n} \
  E & subset.eq { {u, v} | u, v in V, u != v }, quad abs(E) = m
$

The neighbourhood of a vertex is a set of vertices connected to it via an edge.
$
  cal(N) (v) = {u | {u, v} in E},
$

The vertex degree is defined as the size of vertex neighbourhood.
$
  d(v) = abs(cal(N) (v))
$

Path between two vertices $u$ and $v$ is a sequence of vertices beginning in
the first and ending in the second where every two vertices in path must be
directly connected.
$
  P(u,v) = (v_1, v_2, ..., v_k), quad v_1 = u, quad v_k = v, \
  quad {v_l, v_(l+1)} in E quad forall l in {1, ..., k-1}
$
If there exist no such path, let $P_(u,v) = ()$.

Similarily, path length is the number of _steps_ (edges) needed to walk the
path. If path doesn't exist we define its legth as zero.
$
  "len"(P_(u,v)) = cases(
    abs(P_(u,v)) - 1 quad & abs(P_(u,v)) >= 2",",
    0 quad & "else ."
  )
$

The shortest path between vertices $u$ and $v$ is the path with minimal
length among all paths connecting these vertices. Let $PP(u, v)$ be the set
of all paths between $u$ and $v$:
$
  P^*_(u, v) = min_(P in PP(u, v)) "len"(P) " ."
$

A graph has an average (shortest) path length $"len"(G)$.
$
  "len"(G) = 1/n(n-1) Sigma_(u != v) "len"(P^*_(u, v)) " ."
$

We say that two vertices $u$, $v$ are connected if at least one path between
them exist, i.e.

$
  "len"(P_(u,v)) > 0 "."
$

The graph is connected if all its vertices are connected:

$
  forall u,v in V, u != v: quad "len"(P_(u,v)) > 0 "."
$

The measurement of how much do individual vertices in graph cluster together is
called global clustering coefficient and it is based on triplets of nodes. One
triplet is defined as three vertices with two or three edges between them. If
the number of mutual edges is two, the triplet is called open, if it is three,
the triplet is called closed. The global clustering coefficient of graph $C(G)$ is defined
as:

$
  C(G) = "# closed triplets" / "# all triplets"
$ <eq:clustering_coefficient>

Small world network is a graph with certain characteristic @Watts1998. It has
high clustering coefficient and low distances. One example of small world
network might be a human relationships network. The low distances might be
expressed with respect to number of vertices as:

$
  "len"(G) prop log(abs(V))
$

Small world networks tend to contain complete subgraphs (cliques). This is
direct result of high clustering coefficient. Additionally, because the average
path is small, they often contain _hubs_ (vertices with high degree) which
serves as connections between other vertices.

One of methods to quantify the _small-worldness_ of a network is a small-world
measure ($omega$) defined as follows @telesford2011ubiquitysmallworldnetworks:

$
  omega(G) = "len"(R) / "len"(G) - C(G) / C(L)
$

Where $G$ is the analyzed network, $R$ is an equivalent random network to $G$,
and $L$ is an equivalent (ring) lattice network to $G$. This method aims to
quantify the formulating properties of small-world network: high clustering
coefficient which is _ideal_ in lattice networks, and low average path which is
_ideal_ in random networks.


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
along with $Z$ (the height of all cells) is called _configuration_.

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
demonstrating on the boundaries. When a grain leaves the chessboard (or enters a
boundary cell) it dissipated from the system. Dissipation is an important
property of the model. Without dissipation the model supersaturates, leading to
one infinite avalanche.


We can express the square lattice (chessboard) as a graph $G = (V,E)$ with the
size given by #footnote[In a square lattice, the $"size"^2 = n$, which is the
  number of vertices in graph.] $sqrt(n)$. For a boundary condition, the
boundary vertices height is fixed to zero $z = 0$. Alternatively, we may
imagine only one vertex, called _sink vertex_, with such condition. This sink is
connected to all boundary vertices, thus creating the same effect. Please note,
that this simplification creates a multigraph, because corner vertices and sink
are connected via two edges. For the rest of this thesis we will consider the
simplified version with one sink vertex and therefore a multigraph. However, it
is trivial to create multiple sink vertices to restore a simple undirected
graph.

#figure(
  caption: "Comparison of square lattice sink vertices (blue). On the left there
  is a graph with sinks at the boundary - as in the original model. On the right
  there is a multigraph, the square lattice with a single sink connected to all
  vertices at the boundary which is hinted by outgoing edges.",

  grid(
    columns: (1fr, 1fr),
    gutter: 2em,
    align: center,

    (diagram.boundary_sinks)(), (diagram.single_sink)(),
  ),
) <fig:sq-lat_comp_sinks>


All properties of the cellular automaton system holds on a square lattice. This
model was later generalized from square lattice to arbitrary graph
@Holroyd_2008. That generalization is important because it allows us to study
the model's behaviour for different graph topologies.

Classical model has important property. The final configuration is independent
of the order in which avalanches happen. This also implies that when adding two
grains on different cells, the final configuration does not depend on the order
of adding grains. It is because the addition of a sand grain can be seen as an
operator, and these operators were proved to form an Abelian group @Dhar1990.

This property was later shown to hold on finite (un)directed graphs
@Holroyd_2008, allowing us to study the behaviour of the model on different
graph topologies, rather than studying the consequences of changinga toppling order.

== Graph Topologies

Standard choice for sandpile model graph is square lattice with arbitrary size.
This however do not realistically represent the model from motivation story in
@h:intro. That is why we introduce different graph topologies.

Before leaving square lattice entirely, we discuss toppling and dissipation
rules on it. The original model presents to us a toppling rule described in
@eq:cell-automaton_topple_rules_2d and $K=4$, where $K$ is the number of grains
which cause a single cell (vertex) to topple.

Dissipation rules on general graph are more complex and further discussed later
in @h:dissipation_rules. While the different definitions of $K$ and toppling
rules might be more deeply explored, we will only consider $K$ being a function
of vertex:

$
  K(v) = d(v), quad v in V "."
$ <eq:definition_max_grains>

And the toppling rule is also expressed as a function of vertex. Similarily to
original definition as cellular automaton, the height of a vertex is expressed
as a function $z(v)$:

$
  z(v) arrow z(v) - K(v) \
  z(n) arrow z(n) + 1, quad n in cal(N)(v)
$ <eq:toppling_rule>

This simple rule states, that during the toppling process the original vertex
loses as many grains as many neighbours it has and all its neighbours gain one.
The benefit of this $K$ and toppling rule is their simplicity.

For example, it might be interesting to set $K = "const"$, but that would create
problems. If a vertex has less neighbours, it is unclear where the additional
grains go during a topple, and if it has more neighbours, does it artifically
generate more grains, or randomly distribute fewer than $K$.

Note that in these reasonings, the neighbourhood of a vertex $cal(N)(v)$
contains sink vertex, and possibly multiple times. The toppling rule will not
work if $K(v) = 0$. We acknowledge this constraint but leave its resolution to
dissipation rules.

With $K$, toppling rule and dissipation rules specified, we now continue onto
the properties of different graph topologies. Sqaure lattice is a regular graph,
meaning the degree of all vertices is the same @Chen1997GraphTheory, if we
consider sink vertex.

Regularity is not often found in human social networks, meaning graphs where
vertices represent individuals and edges their mutual acquaintance. Purely
random graph also would not be optimal for although it is not regular, human
networks are not completely random. Fortunately, a small-world network (better
described in @h:graph_theory) are almost perfect.

The three most known random graph generators are the Erdős–Rényi (ER) model
@Erdos2022OnRG, Barabási-Albert (BA) @Baraba_si_1999 model and Watts-Strogatz
(WS) model @Watts1998. Each of this methods generates the graph differently
resulting in various _small-worldness_.

The *ER model* was the first discovered. There are two variants, both of which
start with graph with $n$ vertices. The $G(n,m)$ model chooses a graph uniformly
from the set of all possible graphs with $n$ vertices and $m$ edges. The
$G(n,p)$ model creates all possible edges but only with the probability $p$.

The $G(n,m)$ model ensures that any edge is equally likely and the number of
edges is known before. This, however, do not provide the resulting graph being
connected. The $G(n,p)$ model ensures that every edge has fixed probability of
being present, also not ensuring connected graph.

Another drawback of this model is the low clustering coefficient. That is a
consequence of all edges having the same probability, which is not usually seen
in social networks and in turn in small-world networks.

The latest to be discovered is the *BA model*. The important observation BA made
is that many observed networks are _scale-free_, meaning they have power-law
degree distribution. Their method of generating has two key concepts: growth and
preferential attachment.

Growth simply means that the graph is build up from small number of vertices.
Preferential attachment means for a vertex, the higher degree it has, the more
likely it is to be connected to new vertex.

The algorithm for generating the graph is, simply put, a loop in which one
vertex is added to the graph and connected $m$ vertices. They are choosed from
all vertices with probability shown in @eq:ba_prob, where $v$ is the examined
vertex and $u$ represents all vertices. The newly added vertex is yet not
considered.

$
  p(v) = d(v) / (Sigma_u d(u)), quad u,v in V
$ <eq:ba_prob>

This yields in quick emergence of hubs, vertices with high degree. BA provide
example, newly create webpage is more likely to link to well-known websites
which already have high degree.

As the consequence of hubs existence, the average path is short. However,
interesting property of BA model is the dependance of clustering on $N =
abs(V)$, in @eq:ba_clustering (both @Baraba_si_1999). This behaviour is
different from small-world network where clustering is independent of size.

$
  C ~ ln(N^2) / N
$ <eq:ba_clustering>

This model generates a connected graph, because when new vertex is added, it is
connected to exactly $m$ existing vertices.

The *WS model* authors have also defined the small-world measure (#sym.omega)
discussed in @h:graph_theory. WS argued that ER model lacks two important
properties often observed in small-world network: high clustering coeficient and
degree distribution convergence to power law. Their model was designed to
address the first issue.

The generation algorithm is simple: 1. start with a regular ring lattice, 2.
rewire every edge with probability #sym.beta. The idea is to preserve the short
average path present in ER model, but simultaneously create high clustering.

This is done by interpolating between the regular ring lattice (which has high
clustering coefficient) and ER model. When $beta = 0$, no edge is rewired and
the ring lattice holds. It cannot approach the ER model, because rewiring does
not remove edges and therefore the graph is connected - not a guaranteed
property in ER model.

The algorithm for WS graph generation needs these parameters: number of
vertices $N$, mean degree $K$ (even integer) and rewiring probability $beta$.
When $beta = 1$ it approaches a structure close to ER $G(n,p)$ model with $p =
K/(N-1)$.

While this method was designed to have high clustering coefficient and low
average path length, the degree distribution is not power law as seen in BA
model. This divergence from observable small-world networks (e.g. airport
networks) is reflected in all vertices having similar degree.

TODO: mozna implementuju i ostatni grafy a bude potreba smazat odstavec

Either of these methods of generating small-world network has its imperfections.
In the interactive tool developed for the needs of this thesis is only
implemented WS method, solely because its authors concentrated specifically on
small-world #footnote[BA method is focused on scale-free networks and ER method
  on random graphs.]. For that reason, in @h:observed_behaviour we will focus
only on WS method.

== Dissipation Rules <h:dissipation_rules>

With the introduction of arbitrary graph we must reconsider dissipation rules.
The square lattice has explicit boundaries which serve as sinks. However, many
graphs do not have an implicit boundary. The existence of sink is fundamental
for the model not to supersaturate.

A dissipation rule determines how many times a vertex should be connected to the
sink. There is not a single rule which would be generally applied to any graph.
We discuss different rules, their properties and analogies. Dissipation
rules are not well-known thus their names are created for the purposes of this
thesis.

We can present a dissipation rule as a function of vertex returning an integer
which denotes how many times a vertex should be connected to the sink.
$
  D: v arrow NN " ."
$

For example, the graph representation of the original model uses a *Fill to
Four* rule. It states that the degree of any vertex must be four or more. The
sink vertex is connected to all vertices as many times it is needed to fulfil
that rule. For the square lattice, only vertices on the edge are connected
(exactly once except for corner vertices, which are connected twice).
$
  D(v) = cases(
    4 - d(v) quad d(v) >= 4",",
    0 #h(1mm) "."
  )
$

For different graphs though, this rule might be insufficient. It does not ensure
dissipation. This rule does not consider the vertex degree if it is higher
than four.

To ensure dissipation on any graph, the rule must connect all vertices to the
sink at least once. That is because there might exist vertices which are not
connected to any other vertex. A problem arises with such vertex if it were not
connected: $d(v) = 0 = K$ meaning it should trigger an avalanche but that itself
is nonsensical. Dissipation rules must prevent this from happening.

If we consider connected graphs the dissipation condition softens. It is
guaranteed that for all pairs of vertices exist at least one path therefore only
one sink is sufficient to ensure dissipation. This might be useful for the
sandpile model is only interested on connected graphs where they exhibit
critical behaviour.

One possible rule which guarantees dissipation on any graph is *All Once*. It
ensures just the necessary condition that every vertex must be connected to sink
at least once but nothing more. The rule is fair to all vertices as the
connection to the sink is not exclusive to vertices of some characteristic
#footnote[Possible characteristics include vertex neighbourhood size, shortest
  path by which it reaches all other vertices and more.].
$
  D(v) = 1 " ."
$

One rule sufficient for connected graph is *As Many As Neighbours*. This
rule connects every vertex to sink proportionally to its neighbourhood size.
When considering connected graph this rule ensures dissipation and even provide
a connection to the sink to all vertices.
$
  D(v) = d(v) " ."
$

The *Fill to four* rule may be seen as a generalization of *Fill to N* rule. The
specific case where $N = 4$ is useful for (two dimensional) lattice, but for
three dimensional lattice, for example, is more adept $N = 6$. The generalized
rule with static value (e.g. 4 or 6) might be central for particular graph
topologies (e.g. the lattice).

However, if we permit $N$ to vary based on the specific graph parameters, we can
introduce *Fill to D*, where $D$ is equal to the degree of vertex with largest
neighbourhood.

$
  D = max_(v in V) d(v)
$

TODO: proc je to husty

Every rule represents a different approach and not all can be applied to all
types of graph topology. Below are listed necessary requirements on graph for
all dissipation rules in order to ensure dissipation:

/ Fill to four:
  Each graph component has at least one vertex which has at most three edges.
/ All once:
  No requirement.
/ As many as neighbours:
  There is no vertex with degree zero, i.e. the graph is connected.

Different graph topologies are suitable for various subset of dissipation rules,
as shown in @tab:topo_diss. Note that specific graph generated by any
random method may be suitable for additional dissipation rules, the table
covers the general case.

#figure(
  caption: [Table comparing the suitabilty of different dissipation rules
    on graph topologies.],

  {
    let ok = sym.checkmark.heavy
    let no = sym.crossmark.heavy

    table(
      columns: 5,

      table.header([], [*Square lattice*], [*ER graph*], [*BA graph*], [*WS graph*]),

      [*Fill to four*], ok, no, no, no,
      [*All once*], ok, ok, ok, ok,
      [*As many as neighbours*], ok, no, ok, ok,
    )
  },
) <tab:topo_diss>

= Observed Behaviour <h:observed_behaviour>

TODO: how did it work for different types of graphs and dissipation rules? also
the graph configurations (eg $beta$ for WS)

TODO: metrics, how it changed criticality? origin distribution, avalanche size
(max, avg)

TODO: imagery from the app

= Conclusion

TODO

= Technical Documentation

TODO

#bibliography("refs.bib")

#heading([TODO], numbering: none)

- otázka: mám , a . v rovnicích dávat s mezerou, nebo bez?

#heading([TODO PS], numbering: none)

-
