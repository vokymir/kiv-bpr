// TIS' A MODULE
// please only import diagram (at the bottom)

#import "@preview/fletcher:0.5.8" as fletcher: diagram, edge, node

// ============
// BUILDING BLOCKS

#let square-lattice(
  n,
  m,
  skip-boundary-edges: false,
  styles-fn: none,
) = {
  let nodes = ()
  let edges = ()

  // no labels anymore → position-based identity only
  let id(x, y) = (x, y)

  // --- nodes ---
  for x in range(n) {
    for y in range(m) {
      let styles = if styles-fn != none {
        styles-fn(x, y, n, m)
      } else {
        ()
      }

      nodes.push(
        node(
          pos: (x, y),

          // defaults if not provided
          width: if "width" in styles { styles.width } else { 5pt },
          height: if "height" in styles { styles.height } else { 5pt },

          fill: if "fill" in styles { styles.fill } else { none },
          stroke: if "stroke" in styles { styles.stroke } else { 1pt + black },

          shape: if "shape" in styles { styles.shape } else { circle },
        ),
      )
    }
  }

  // --- edges (pure positional, no labels) ---
  let is-boundary(x, y, n, m) = x == 0 or x == n - 1 or y == 0 or y == m - 1

  for x in range(n) {
    for y in range(m) {
      // --- right neighbor ---
      if x + 1 < n {
        let nx = x + 1
        let ny = y

        if not (
          skip-boundary-edges and is-boundary(x, y, n, m) and is-boundary(nx, ny, n, m)
        ) {
          edges.push(edge((x, y), (nx, ny)))
        }
      }

      // --- bottom neighbor ---
      if y + 1 < m {
        let nx = x
        let ny = y + 1

        if not (
          skip-boundary-edges and is-boundary(x, y, n, m) and is-boundary(nx, ny, n, m)
        ) {
          edges.push(edge((x, y), (nx, ny)))
        }
      }
    }
  }

  nodes + edges
}

// ===
// FILL-STYLES

#let uniform-style(fill: none, stroke: 1pt + black, width: 5pt, height: 5pt) = (
  fill: fill,
  stroke: stroke,
  width: width,
  height: height,
)

// x,y is the position of a node
// n,m is the total size of lattice
#let boundary-style(
  x,
  y,
  n,
  m,
  boundary-fill: red,
  inner-fill: none,
  boundary-stroke: 1pt + black,
  inner-stroke: 1pt + black,
  width: 5pt,
  height: 5pt,
) = (
  is-boundary: x == 0 or x == n - 1 or y == 0 or y == m - 1,
  fill: if x == 0 or x == n - 1 or y == 0 or y == m - 1 {
    boundary-fill
  } else {
    inner-fill
  },
  stroke: if x == 0 or x == n - 1 or y == 0 or y == m - 1 {
    boundary-stroke
  } else {
    inner-stroke
  },
  width: width,
  height: height,
)

// =========================================
// COMPLETE DIAGRAMS USED IN THESIS

#let boundary_sinks() = diagram(
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
)

#let single_sink() = diagram(
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
)

// ===================
// PUBLIC API
#let diagram = (
  // generate a square lattice of any size
  square_lattice: square-lattice,
  // show graph with sinks on the boundary
  boundary_sinks: boundary_sinks,
  // show graph with only one sink
  single_sink: single_sink,
)
