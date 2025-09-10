#include <cstdio>
#include <chrono>
#include <cmath>
#include <cstdint>
#include "sched/FixedStep.h"

// Tiny busy-work to simulate control/IO load (spin for ~target_us microseconds)
static void busy_work_us(int target_us) {
  using clk = std::chrono::high_resolution_clock;
  auto start = clk::now();
  volatile double sink = 0.0;
  while (std::chrono::duration_cast<std::chrono::microseconds>(clk::now() - start).count() < target_us) {
    // do a little floating-point to avoid being optimized out
    sink += std::sqrt(42.0);
  }
  (void)sink;
}



int main() {
  const double dt = 0.002;              // 2 ms => 500 Hz
  const double run_secs = 3.0;          // run for 3 seconds
  const int simulated_load_us = 200;    // ~0.2 ms of work each tick (tweak this)

  FixedStep loop(dt);

  uint64_t tick = 0;
  loop.run_for(run_secs, [&](double /*h*/) {
    // Do your “control” work here
    busy_work_us(simulated_load_us);

    // Print a heartbeat every 100 ticks (~0.2s at 500 Hz)
    if ((tick++ % 100) == 0) {
      std::puts("tick");
    }
    });

  const StepStats& s = loop.stats();
  std::printf(
    "DONE: ticks=%llu, misses=%llu, avg_slack=%.3f ms, worst_slack=%.3f ms\n",
    static_cast<unsigned long long>(s.ticks),
    static_cast<unsigned long long>(s.deadline_misses),
    s.avg_slack_s * 1000.0,
    s.worst_slack_s * 1000.0
  );
  return 0;

}
