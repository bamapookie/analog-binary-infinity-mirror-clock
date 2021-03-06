#include <Adafruit_DotStar.h>

#define ABIMC_V_1_3
#include <abimc_pcb.h>

#define STRAND_1_PIXELS 144
#define STRAND_2_PIXELS 120
#define STRAND_3_PIXELS 96
#define STRAND_4_PIXELS 72
#define STRAND_4_PIXELS 48
#define STRAND_4_PIXELS 24

Adafruit_DotStar strip1(STRAND_1_PIXELS, STRAND_1_DATA_PIN, STRAND_1_CLOCK_PIN, DOTSTAR_BRG);
Adafruit_DotStar strip2(STRAND_2_PIXELS, STRAND_2_DATA_PIN, STRAND_2_CLOCK_PIN, DOTSTAR_BRG);
Adafruit_DotStar strip3(STRAND_3_PIXELS, STRAND_3_DATA_PIN, STRAND_3_CLOCK_PIN, DOTSTAR_BRG);
Adafruit_DotStar strip4(STRAND_4_PIXELS, STRAND_4_DATA_PIN, STRAND_4_CLOCK_PIN, DOTSTAR_BRG);
Adafruit_DotStar strip4(STRAND_5_PIXELS, STRAND_5_DATA_PIN, STRAND_5_CLOCK_PIN, DOTSTAR_BRG);
Adafruit_DotStar strip4(STRAND_6_PIXELS, STRAND_6_DATA_PIN, STRAND_6_CLOCK_PIN, DOTSTAR_BRG);

void setup() {
  strip1.begin();
  strip2.begin();
  strip3.begin();
  strip4.begin();
  strip5.begin();
  strip6.begin();
  strip1.show();
  strip2.show();
  strip3.show();
  strip4.show();
  strip5.show();
  strip6.show();
}

int head1 = 0, tail1 = -3;
int head2 = 0, tail2 = -3;
int head3 = 0, tail3 = -3;
int head4 = 0, tail4 = -3;
int head5 = 0, tail5 = -3;
int head6 = 0, tail6 = -3;
uint32_t color1 = 0xFF0000;
uint32_t color2 = 0xFF0000;
uint32_t color3 = 0xFF0000;
uint32_t color4 = 0xFF0000;
uint32_t color5 = 0xFF0000;
uint32_t color6 = 0xFF0000;

void loop() {
  strip1.setPixelColor(head1, color1);
  strip1.setPixelColor(tail1, 0);
  strip1.show();
  strip2.setPixelColor(head2, color2);
  strip2.setPixelColor(tail2, 0);
  strip2.show();
  strip3.setPixelColor(head3, color3);
  strip3.setPixelColor(tail3, 0);
  strip3.show();
  strip4.setPixelColor(head4, color4);
  strip4.setPixelColor(tail4, 0);
  strip4.show();
  strip5.setPixelColor(head5, color5);
  strip5.setPixelColor(tail5, 0);
  strip5.show();
  strip6.setPixelColor(head6, color6);
  strip6.setPixelColor(tail6, 0);
  strip6.show();
  delay(20);

  if (++head1 >= STRAND_1_PIXELS) {
    head1 = 0;
    if ((color1 >>= 8) == 0)
      color1 = 0xFF0000;
    Serial.write("Loop1");
    Serial.write('\n');
  }
  if (++tail1 >= STRAND_1_PIXELS)
    tail1 = 0;
  if (++head2 >= STRAND_2_PIXELS) {
    head2 = 0;
    if ((color2 >>= 8) == 0)
      color2 = 0xFF0000;
  }
  if (++tail2 >= STRAND_2_PIXELS)
    tail2 = 0;
  if (++head3 >= STRAND_3_PIXELS) {
    head3 = 0;
    if ((color3 >>= 8) == 0)
      color3 = 0xFF0000;
  }
  if (++tail3 >= STRAND_3_PIXELS)
    tail3 = 0;
  if (++head4 >= STRAND_4_PIXELS) {
    head4 = 0;
    if ((color4 >>= 8) == 0)
      color4 = 0xFF0000;
  }
  if (++tail4 >= STRAND_4_PIXELS)
    tail4 = 0;
  if (++head5 >= STRAND_5_PIXELS) {
    head5 = 0;
    if ((color5 >>= 8) == 0)
      color5 = 0xFF0000;
  }
  if (++tail5 >= STRAND_5_PIXELS)
    tail5 = 0;
  if (++head6 >= STRAND_6_PIXELS) {
    head6 = 0;
    if ((color6 >>= 8) == 0)
      color6 = 0xFF0000;
  }
  if (++tail6 >= STRAND_6_PIXELS)
    tail6 = 0;
}