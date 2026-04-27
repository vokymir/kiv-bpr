#pragma once

#include <cmath>
#include <cstddef>
#include <vector>
namespace ssoc::stat {

struct Fit_Base {
  size_t used_points = 0;
  bool dirty = true;

  virtual void recompute(const std::vector<double> &xs,
                         const std::vector<double> &ys) = 0;

  virtual ~Fit_Base() = default;
};

struct Power_Law_Fit : Fit_Base {
  double alpha = 0.0;
  double intercept = 0.0;
  double rss = 0.0;
  double rmse = 0.0;

  double sum_log_x = 0.0;
  double sum_log_y = 0.0;
  double sum_log_x2 = 0.0;
  double sum_log_xy = 0.0;

  void recompute(const std::vector<double> &xs,
                 const std::vector<double> &ys) override {
    sum_log_x = sum_log_y = sum_log_x2 = sum_log_xy = 0.0;
    used_points = 0;

    for (size_t i = 0; i < xs.size(); ++i) {
      double x = xs[i], y = ys[i];
      if (x <= 0.0 || y <= 0.0)
        continue;

      double lx = std::log(x);
      double ly = std::log(y);

      sum_log_x += lx;
      sum_log_y += ly;
      sum_log_x2 += lx * lx;
      sum_log_xy += lx * ly;

      used_points++;
    }

    finalize(xs, ys);
  }

private:
  void finalize(const std::vector<double> &xs, const std::vector<double> &ys) {

    if (used_points < 2)
      return;

    double n = static_cast<double>(used_points);
    double denom = n * sum_log_x2 - sum_log_x * sum_log_x;

    if (denom == 0.0)
      return;

    alpha = (n * sum_log_xy - sum_log_x * sum_log_y) / denom;
    intercept = (sum_log_y - alpha * sum_log_x) / n;

    // == RSS in log space
    rss = 0.0;

    for (size_t i = 0; i < xs.size(); ++i) {
      double x = xs[i], y = ys[i];
      if (x <= 0.0 || y <= 0.0)
        continue;

      double lx = std::log(x);
      double ly = std::log(y);

      double predicted = alpha * lx + intercept;
      double residual = ly - predicted;

      rss += residual * residual;
    }

    // normalized rss
    rmse = std::sqrt(rss / static_cast<double>(used_points));
  }
};

struct Log_Linear_Fit : Fit_Base {
  double a = 0.0, b = 0.0;
  double rss = 0.0;
  double rmse = 0.0;

  double sum_x = 0.0;
  double sum_y = 0.0;
  double sum_x2 = 0.0;
  double sum_xy = 0.0;

  void recompute(const std::vector<double> &xs,
                 const std::vector<double> &ys) override {

    sum_x = sum_y = sum_x2 = sum_xy = 0.0;
    used_points = 0;

    for (size_t i = 0; i < xs.size(); ++i) {
      double x = xs[i];
      double y = ys[i];

      if (x <= 0.0)
        continue;

      double lx = std::log(x);

      sum_x += lx;
      sum_y += y;
      sum_x2 += lx * lx;
      sum_xy += lx * y;

      used_points++;
    }

    finalize(xs, ys);
  }

private:
  void finalize(const std::vector<double> &xs, const std::vector<double> &ys) {

    if (used_points < 2)
      return;

    double n = static_cast<double>(used_points);
    double denom = n * sum_x2 - sum_x * sum_x;

    if (denom == 0.0)
      return;

    a = (n * sum_xy - sum_x * sum_y) / denom;
    b = (sum_y - a * sum_x) / n;

    // === RSS in original y-space
    rss = 0.0;

    for (size_t i = 0; i < xs.size(); ++i) {
      double x = xs[i];
      double y = ys[i];

      if (x <= 0.0)
        continue;

      double lx = std::log(x);
      double predicted = a * lx + b;
      double residual = y - predicted;

      rss += residual * residual;
    }

    rmse = std::sqrt(rss / static_cast<double>(used_points));
  }
};

} // namespace ssoc::stat
