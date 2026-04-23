// MODULE
// only import 'plot', it contains everything

#import "@preview/cetz:0.5.0"
#import "@preview/cetz-plot:0.1.3"

// ===========================
// MODULE SECTION
#let default_config = (
  legend_size: 8pt,
)

#let setup_config(config: (:)) = { default_config + config }


// ==============================
// NORMAL DISTRIBUTION SECTION
#let mu = 0
#let sigma = 1

#let normal(x) = {
  let variance = calc.pow(sigma, 2)
  (1 / calc.sqrt(2 * calc.pi * variance)) * calc.exp(-calc.pow(x - mu, 2) / (2 * variance))
}

#let plot_normal_distribution(config) = {
  let cfg = setup_config(config: config)

  set text(size: cfg.legend_size)

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

// ===============
// FUNCTION ON LINEAR VS LOG-LOG SCALE

// define function to show
#let fn(x) = calc.pow(x, -2)

// return [steps] samples of [fn] on x in <[start], [end]>
#let sample(fn, start, end, steps) = {
  range(0, steps).map(i => {
    let t = i / (steps - 1)
    let x = start + (end - start) * t
    (x, fn(x))
  })
}

// get data on x in <1,10>
#let plot_data = sample(fn, 1, 10, 200)

#let linear_plot(config) = {
  let cfg = setup_config(config: config)

  set text(size: cfg.legend_size)
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

#let log_log_plot(config) = {
  let cfg = setup_config(config: config)

  set text(size: cfg.legend_size)

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

#let data_table(config) = {
  let cfg = setup_config(config: config)

  set text(size: cfg.legend_size)

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

// ============
// PUBLIC API
#let plot = (
  // GAUSS
  // plot normal distribution heeey
  normal: plot_normal_distribution,
  // COMPARE PLOTTING
  // standard linear plot
  cmp_linear: linear_plot,
  // log-log plot
  cmp_loglog: log_log_plot,
  // table showing x and y(x)
  cmp_table: data_table,
)
