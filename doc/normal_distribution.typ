
#import "@preview/cetz:0.5.0"
#import "@preview/cetz-plot:0.1.3"

#let legend_size = 8pt

#let mu = 0
#let sigma = 1

#let normal(x) = {
  let variance = calc.pow(sigma, 2)
  (1 / calc.sqrt(2 * calc.pi * variance)) * calc.exp(-calc.pow(x - mu, 2) / (2 * variance))
}

#let plot_normal_distribution() = {
  set text(size: legend_size)
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
      x-format: v => move(dy: 3.5pt, [$#v$]),
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

// =====

// define function to show
#let fn(x) =  calc.pow(x, -2)

// Generate data points
#let plot_data = range(1, 11).map(i => {
  let x = i
  (x, fn(x))
})

#let linear_plot() = {
  set text(size: legend_size)
  cetz.canvas({
    import cetz-plot: plot

    plot.plot(
      size: (6, 3),
      x-label: move(dy: 3pt, [$x$]),
      y-label: [$y$],
      x-format: v => move(dy: 3.5pt, [$#v$]),
      {
        plot.add(plot_data, style: (stroke: blue + 1.5pt))
      },
    )
  })
}

#let log_log_plot() = {
  set text(size: legend_size)
  cetz.canvas({
    import cetz-plot: plot

    // transform the data to log space
    let log_data = plot_data.map(((x, y)) => (
      calc.log(x, base: 10),
      calc.log(y, base: 10),
    ))

    plot.plot(
      size: (6, 3),
      x-label: move(dy: 6pt, [$log x$]),
      y-label: [$log y$],
      // use linear scales because we pre-transformed the data
      x-tick-step: 1,
      y-tick-step: 1,
      // format ticks to show 10^n
      x-format: v => move(dy: 3.5pt, [$10^#v$]),
      y-format: v => [$10^#v$],
      {
        plot.add(log_data, style: (stroke: red + 1.5pt))
      },
    )
  })
}

#let data_table() = {
  set text(size: 8pt)
  let display_data = plot_data

  align(center, table(
    // first column is auto-width for header, remaining are equal
    columns: (auto, ..(1fr,) * display_data.len()),
    inset: 6pt,
    align: center + horizon,
    stroke: 0.5pt + gray,

    fill: (x, y) => if x == 0 { gray.lighten(80%) },

    [*$x$*], ..display_data.map(d => [#calc.round(d.at(0), digits: 1)]),

    [*$y$*], ..display_data.map(d => [#calc.round(d.at(1), digits: 2)]),
  ))
}
