#include "abimc_render.h"

namespace abimc {

long long mod(long long a, long long m) {
  if (m <= 0) {
    return 0;
  }
  return ((a % m) + m) % m;
}

int computeHand(int width, long offset, long long period, int pixels,
                unsigned long long millis, HandContribution* out) {
  if (period <= 0 || pixels <= 0 || width < 0) {
    return 0;
  }

  // Position is tracked in sub-pixels: every LED is FRACTIONAL_BRIGHTNESS - 1
  // sub-pixels wide. This mirrors the original ST() macro, which was called with
  // pixels * (FRACTIONAL_BRIGHTNESS - 1) as its pixel count.
  const long long subpixels = (long long)pixels * (FRACTIONAL_BRIGHTNESS - 1);
  const unsigned long long elapsed = millis % (unsigned long long)period;
  const unsigned long long travel = elapsed * (unsigned long long)subpixels / (unsigned long long)period;
  const long long position = mod((long long)offset + (long long)travel, subpixels);

  const int wholePosition = (int)(position / (FRACTIONAL_BRIGHTNESS - 1));
  const int fractionalPosition = (int)(position % (FRACTIONAL_BRIGHTNESS - 1));

  for (int i = 0; i <= width; i++) {
    out[i].index = (int)mod((long long)wholePosition + i, pixels);
    if (i == 0) {
      // Leading edge fades out as the hand advances past this pixel.
      out[i].fadeBy = (uint8_t)fractionalPosition;
    } else if (i == width) {
      // Trailing edge fades in as the hand advances into the next pixel.
      out[i].fadeBy = (uint8_t)(FRACTIONAL_BRIGHTNESS - 1 - fractionalPosition);
    } else {
      out[i].fadeBy = 0;
    }
  }
  return width + 1;
}

}  // namespace abimc
