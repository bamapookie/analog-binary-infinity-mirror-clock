#include <FastLED.h>

#define ABIMC_V_1_3

#include <abimc_pcb.h>

#define STRAND_1_PIXELS 144
#define STRAND_2_PIXELS 120
#define STRAND_3_PIXELS 96
#define STRAND_4_PIXELS 72
#define STRAND_5_PIXELS 48
#define STRAND_6_PIXELS 24

CRGB strand1[STRAND_1_PIXELS];
CRGB strand2[STRAND_2_PIXELS];
CRGB strand3[STRAND_3_PIXELS];
CRGB strand4[STRAND_4_PIXELS];
CRGB strand5[STRAND_5_PIXELS];
CRGB strand6[STRAND_6_PIXELS];

void setup() {
  FastLED.addLeds<DOTSTAR, STRAND_1_DATA_PIN, STRAND_1_CLOCK_PIN, BRG>(strand1, STRAND_1_PIXELS);
  FastLED.addLeds<DOTSTAR, STRAND_2_DATA_PIN, STRAND_2_CLOCK_PIN, BRG>(strand2, STRAND_2_PIXELS);
  FastLED.addLeds<DOTSTAR, STRAND_3_DATA_PIN, STRAND_3_CLOCK_PIN, BRG>(strand3, STRAND_3_PIXELS);
  FastLED.addLeds<DOTSTAR, STRAND_4_DATA_PIN, STRAND_4_CLOCK_PIN, BRG>(strand4, STRAND_4_PIXELS);
  FastLED.addLeds<DOTSTAR, STRAND_5_DATA_PIN, STRAND_5_CLOCK_PIN, BRG>(strand5, STRAND_5_PIXELS);
  FastLED.addLeds<DOTSTAR, STRAND_6_DATA_PIN, STRAND_6_CLOCK_PIN, BRG>(strand6, STRAND_6_PIXELS);
  FastLED.clear();
}

int head1 = 0, tail1 = -3;
int head2 = 0, tail2 = -3;
int head3 = 0, tail3 = -3;
int head4 = 0, tail4 = -3;
int head5 = 0, tail5 = -3;
int head6 = 0, tail6 = -3;
CHSV color1 = CHSV(0, 255, 127);
CHSV color2 = CHSV(0, 255, 127);
CHSV color3 = CHSV(0, 255, 127);
CHSV color4 = CHSV(0, 255, 127);
CHSV color5 = CHSV(0, 255, 127);
CHSV color6 = CHSV(0, 255, 127);

void loop() {
  strand1[head1] = color1;
  strand1[tail1] = CRGB::Black;
  strand2[head2] = color2;
  strand2[tail2] = CRGB::Black;
  strand3[head3] = color3;
  strand3[tail3] = CRGB::Black;
  strand4[head4] = color4;
  strand4[tail4] = CRGB::Black;
  strand5[head5] = color5;
  strand5[tail5] = CRGB::Black;
  strand6[head6] = color6;
  strand6[tail6] = CRGB::Black;
  FastLED.show();
  delay(20);
  if (++head1 >= STRAND_1_PIXELS) {
    head1 %= STRAND_1_PIXELS;
    color1.h = (color1.h + 224) % 256;
  }
  tail1 = (tail1 + 1) % STRAND_1_PIXELS;
  if (++head2 >= STRAND_2_PIXELS) {
    head2 %= STRAND_2_PIXELS;
    color2.h = (color2.h + 224) % 256;
  }
  tail2 = (tail2 + 1) % STRAND_2_PIXELS;
  if (++head3 >= STRAND_3_PIXELS) {
    head3 %= STRAND_3_PIXELS;
    color3.h = (color3.h + 224) % 256;
  }
  tail3 = (tail3 + 1) % STRAND_3_PIXELS;
  if (++head4 >= STRAND_4_PIXELS) {
    head4 %= STRAND_4_PIXELS;
    color4.h = (color4.h + 224) % 256;
  }
  tail4 = (tail4 + 1) % STRAND_4_PIXELS;
  if (++head4 >= STRAND_4_PIXELS) {
    head4 %= STRAND_4_PIXELS;
    color4.h = (color4.h + 224) % 256;
  }
  tail4 = (tail4 + 1) % STRAND_4_PIXELS;
  if (++head5 >= STRAND_5_PIXELS) {
    head5 %= STRAND_5_PIXELS;
    color5.h = (color5.h + 224) % 256;
  }
  tail5 = (tail5 + 1) % STRAND_5_PIXELS;
  if (++head6 >= STRAND_6_PIXELS) {
    head6 %= STRAND_6_PIXELS;
    color6.h = (color6.h + 224) % 256;
  }
  tail6 = (tail6 + 1) % STRAND_6_PIXELS;
}