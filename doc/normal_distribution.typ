
#import "@preview/cetz:0.5.0"
#import "@preview/cetz-plot:0.1.3"

#let mu = 0
#let sigma = 1

#let normal(x) = {
  let variance = calc.pow(sigma, 2)
  (1 / calc.sqrt(2 * calc.pi * variance)) * calc.exp(-calc.pow(x - mu, 2) / (2 * variance))
}

#let plot_normal_distribution() = {
  set text(size: 8pt)
  cetz.canvas({
    import cetz-plot: plot


    plot.plot(
      size: (6, 3),
      x-tick-step: 1,
      y-tick-step: 0.1,
      y-min: 0,
      y-max: 0.5,
      x-label: [],
      y-label: [],
      y-format: v => $#v$,
      x-format: v => move(dy: 4pt, [$#v$]),
      {
        plot.add(
          domain: (-4, 4),
          normal,
          style: (stroke: blue + 1.5pt),
        )
      },
    )
  })
}
