// abimc_render -- pure, framework-agnostic rendering math for the Analog Binary
// Infinity Mirror Clock.
//
// This library deliberately has no Arduino or FastLED dependency so it can be
// unit tested on a host with `pio test -e native`. It computes *where* a clock
// hand lands on a ring and *how much* to fade its leading/trailing pixels; the
// firmware applies the actual colour using FastLED.
#ifndef ABIMC_RENDER_H
#define ABIMC_RENDER_H

#include <cstdint>

namespace abimc {

// Sub-pixel brightness resolution. A hand's position is tracked in units of
// 1/(FRACTIONAL_BRIGHTNESS - 1) of a pixel so a hand can glide smoothly between
// adjacent LEDs instead of snapping from one to the next.
//
// NOTE: must stay equal to the FRACTIONAL_BRIGHTNESS used by the firmware in
// src/main.cpp.
constexpr int FRACTIONAL_BRIGHTNESS = 256;

// Generous upper bound on a hand's width, used to size contribution buffers.
constexpr int MAX_HAND_WIDTH = 32;

// One LED lit by a hand: which pixel on the ring, and how much to fade the hand
// colour when applying it. fadeBy matches FastLED's fadeToBlackBy(): 0 leaves
// the colour untouched, 255 fades it to black.
struct HandContribution {
  int index;
  uint8_t fadeBy;
};

// Positive modulo: the result is always in [0, m) for m > 0 (unlike C's % which
// keeps the sign of the dividend). Returns 0 if m <= 0.
long long mod(long long a, long long m);

// Compute the LEDs lit by a hand.
//
// The hand spans width + 1 LEDs. Interior LEDs get the full colour; the leading
// and trailing LEDs are faded in proportion to the hand's sub-pixel position so
// the hand appears to slide smoothly around the ring. The mapping is identical
// to the firmware's original updateStrand() math.
//
// Parameters:
//   width   - hand width in whole pixels (>= 1); the hand lights width + 1 LEDs.
//   offset  - alignment offset, in sub-pixels, applied before wrapping.
//   period  - the hand's full-revolution period in the same units as millis.
//   pixels  - number of LEDs on the ring.
//   millis  - elapsed time positioning the hand within its period.
//   out     - caller-provided buffer holding at least width + 1 contributions.
//
// Returns the number of contributions written (width + 1), or 0 if period <= 0.
int computeHand(int width, long offset, long long period, int pixels,
                unsigned long long millis, HandContribution* out);

}  // namespace abimc

#endif  // ABIMC_RENDER_H
