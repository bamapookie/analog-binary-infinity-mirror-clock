# abimc_render

Pure, framework-agnostic rendering math for the clock. No Arduino or FastLED
dependency, so it builds and unit-tests on a host (`pio test -e native`).

Today it provides the hand-positioning geometry extracted from the firmware's
original `updateStrand()`:

- `abimc::mod(a, m)` — positive modulo (result always in `[0, m)`).
- `abimc::computeHand(...)` — given a hand width, ring size, period and elapsed
  time, returns the LEDs the hand lights and how much to fade the leading/
  trailing pixels (`fadeBy`, matching FastLED's `fadeToBlackBy`). The firmware
  applies the actual colour; this library only decides geometry and fade.

The split keeps the testable integer math (positions, sub-pixel fading, ring
wrap-around) separate from the FastLED colour application, and is where the
planned renderer motion rules (capped-speed slewing, the 60-on-64 catch-up,
leap-second pacing — see `ROADMAP.md`) will live.

Sub-pixel resolution is controlled by `FRACTIONAL_BRIGHTNESS`, which must match
the value used in `src/main.cpp`.
