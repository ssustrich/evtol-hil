#pragma once
#include <chrono>
#include <cstdint>
#include <functional>
#include <thread>

struct StepStats {
  uint64_t ticks = 0;
  uint64_t deadline_misses = 0;  // count of ticks where work finished after the deadline
  double worst_slack_s = 1e9;    // most negative (i.e., smallest) slack seen (seconds)
  double avg_slack_s = 0.0;      // running average of slack (seconds)
};

class FixedStep {
 public:
  explicit FixedStep(double dt_seconds) : dt_s_(dt_seconds) {}

  void run_for(double seconds, const std::function<void(double)>& step) {
    using clk = std::chrono::steady_clock;
    using dsec = std::chrono::duration<double>;

    const auto period = dsec(dt_s_);
    const uint64_t total_ticks = static_cast<uint64_t>(seconds / dt_s_);

    auto next = clk::now() + period;
    stats_ = StepStats{};  // reset

    for (uint64_t n = 0; n < total_ticks; ++n) {
      const auto t_start = clk::now();

      step(dt_s_);

      const auto t_end = clk::now();
      // Slack = time remaining until the deadline (can be negative if overrun)
      const double slack = std::chrono::duration_cast<dsec>(next - t_end).count();

      if (slack < 0.0) {
        ++stats_.deadline_misses;
      }
      if (slack < stats_.worst_slack_s) {
        stats_.worst_slack_s = slack;
      }
      // running average
      stats_.avg_slack_s += slack;
      ++stats_.ticks;

      // Wait until the next deadline (even if we overran, we still align to the next tick)
      std::this_thread::sleep_until(next);
      next += period;
    }
    if (stats_.ticks) stats_.avg_slack_s /= static_cast<double>(stats_.ticks);
  }

  const StepStats& stats() const { return stats_; }
  double dt_s() const { return dt_s_; }

 private:
  double dt_s_;
  StepStats stats_;
};
