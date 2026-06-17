// Host-side unit tests for abimc_render. Run with: pio test -e native
#include <unity.h>

#include <abimc_render.h>

using abimc::computeHand;
using abimc::FRACTIONAL_BRIGHTNESS;
using abimc::HandContribution;
using abimc::mod;

void setUp(void) {}
void tearDown(void) {}

void test_mod_wraps_negatives_into_range(void) {
  TEST_ASSERT_EQUAL_INT64(9, mod(-1, 10));
  TEST_ASSERT_EQUAL_INT64(0, mod(10, 10));
  TEST_ASSERT_EQUAL_INT64(5, mod(25, 10));
  TEST_ASSERT_EQUAL_INT64(7, mod(-3, 10));
}

void test_mod_guards_nonpositive_modulus(void) {
  TEST_ASSERT_EQUAL_INT64(0, mod(5, 0));
  TEST_ASSERT_EQUAL_INT64(0, mod(5, -3));
}

void test_compute_hand_rejects_bad_input(void) {
  HandContribution out[8];
  TEST_ASSERT_EQUAL_INT(0, computeHand(1, 0, /*period=*/0, 10, 0, out));
  TEST_ASSERT_EQUAL_INT(0, computeHand(1, 0, /*period=*/-1, 10, 0, out));
  TEST_ASSERT_EQUAL_INT(0, computeHand(1, 0, 1000, /*pixels=*/0, 0, out));
}

void test_compute_hand_at_period_start(void) {
  // millis == 0 -> the hand sits exactly on pixel 0.
  HandContribution out[8];
  int n = computeHand(/*width=*/1, /*offset=*/0, /*period=*/1000, /*pixels=*/10,
                      /*millis=*/0, out);
  TEST_ASSERT_EQUAL_INT(2, n);
  TEST_ASSERT_EQUAL_INT(0, out[0].index);
  TEST_ASSERT_EQUAL_UINT8(0, out[0].fadeBy);  // leading edge full brightness
  TEST_ASSERT_EQUAL_INT(1, out[1].index);
  // trailing edge fully faded (FRACTIONAL_BRIGHTNESS - 1)
  TEST_ASSERT_EQUAL_UINT8(FRACTIONAL_BRIGHTNESS - 1, out[1].fadeBy);
}

void test_compute_hand_lands_on_whole_pixel(void) {
  // Half a period on a 10-pixel ring -> exactly pixel 5, no fractional fade.
  HandContribution out[8];
  int n = computeHand(1, 0, 1000, 10, 500, out);
  TEST_ASSERT_EQUAL_INT(2, n);
  TEST_ASSERT_EQUAL_INT(5, out[0].index);
  TEST_ASSERT_EQUAL_UINT8(0, out[0].fadeBy);
  TEST_ASSERT_EQUAL_INT(6, out[1].index);
  TEST_ASSERT_EQUAL_UINT8(FRACTIONAL_BRIGHTNESS - 1, out[1].fadeBy);
}

void test_compute_hand_sub_pixel_fade(void) {
  // travel = 502 * (10*255) / 1000 = 1280 sub-pixels = pixel 5 + 5 sub-pixels.
  HandContribution out[8];
  int n = computeHand(1, 0, 1000, 10, 502, out);
  TEST_ASSERT_EQUAL_INT(2, n);
  TEST_ASSERT_EQUAL_INT(5, out[0].index);
  TEST_ASSERT_EQUAL_UINT8(5, out[0].fadeBy);
  TEST_ASSERT_EQUAL_INT(6, out[1].index);
  TEST_ASSERT_EQUAL_UINT8(FRACTIONAL_BRIGHTNESS - 1 - 5, out[1].fadeBy);
}

void test_compute_hand_wraps_around_ring(void) {
  // Near the end of the period the hand straddles pixel 9 -> pixel 0.
  HandContribution out[8];
  int n = computeHand(1, 0, 1000, 10, 999, out);
  TEST_ASSERT_EQUAL_INT(2, n);
  TEST_ASSERT_EQUAL_INT(9, out[0].index);
  TEST_ASSERT_EQUAL_INT(0, out[1].index);  // wrapped back to the start
}

void test_compute_hand_interior_pixels_full_brightness(void) {
  // A wider hand: only the first and last LEDs are faded.
  HandContribution out[8];
  int n = computeHand(/*width=*/3, 0, 1000, 24, 502, out);
  TEST_ASSERT_EQUAL_INT(4, n);
  TEST_ASSERT_EQUAL_UINT8(0, out[1].fadeBy);
  TEST_ASSERT_EQUAL_UINT8(0, out[2].fadeBy);
  TEST_ASSERT_NOT_EQUAL(0, out[0].fadeBy);
  TEST_ASSERT_NOT_EQUAL(0, out[3].fadeBy);
}

void test_compute_hand_negative_offset(void) {
  // A negative offset must still map into a valid pixel range.
  HandContribution out[8];
  int n = computeHand(1, /*offset=*/-255, 1000, 10, 0, out);
  TEST_ASSERT_EQUAL_INT(2, n);
  TEST_ASSERT_TRUE(out[0].index >= 0 && out[0].index < 10);
  TEST_ASSERT_TRUE(out[1].index >= 0 && out[1].index < 10);
}

int main(int, char**) {
  UNITY_BEGIN();
  RUN_TEST(test_mod_wraps_negatives_into_range);
  RUN_TEST(test_mod_guards_nonpositive_modulus);
  RUN_TEST(test_compute_hand_rejects_bad_input);
  RUN_TEST(test_compute_hand_at_period_start);
  RUN_TEST(test_compute_hand_lands_on_whole_pixel);
  RUN_TEST(test_compute_hand_sub_pixel_fade);
  RUN_TEST(test_compute_hand_wraps_around_ring);
  RUN_TEST(test_compute_hand_interior_pixels_full_brightness);
  RUN_TEST(test_compute_hand_negative_offset);
  return UNITY_END();
}
