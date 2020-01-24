#include <Arduino.h>
#include <FastLED.h>
#include <SPI.h>
#include <TimeLib.h>
#include <Timezone.h>
#include <TinyGPS++.h>

#define ABIMC_V_1_3

#include <abimc_pcb.h>
#include <xa1110.h>

#define DEBUG
//
// Settings
// LED Settings
#define STRAND_1_PIXELS 144
#define STRAND_1_OFFSET 0
#define STRAND_1_MINUTES_PERIOD 2
#define STRAND_1_HOURS_PERIOD 2
#define STRAND_2_PIXELS 120
#define STRAND_2_OFFSET 0
#define STRAND_2_MINUTES_PERIOD 4
#define STRAND_2_HOURS_PERIOD 4
#define STRAND_3_PIXELS 96
#define STRAND_3_OFFSET 0
#define STRAND_3_MINUTES_PERIOD 8
#define STRAND_3_HOURS_PERIOD 8
#define STRAND_4_PIXELS 72
#define STRAND_4_OFFSET 0
#define STRAND_4_MINUTES_PERIOD 16
#define STRAND_4_HOURS_PERIOD 16
#define STRAND_5_PIXELS 48
#define STRAND_5_OFFSET 0
#define STRAND_5_MINUTES_PERIOD 32
#define STRAND_5_HOURS_PERIOD 32
#define STRAND_6_PIXELS 24
#define STRAND_6_OFFSET 0
#define STRAND_6_MINUTES_PERIOD 64
#define STRAND_6_HOURS_PERIOD 0

#define FRACTIONAL_BRIGHTNESS 256
#define SECOND_HAND_WIDTH 11
#define SECOND_HAND_PULSE_DURATION_MILLIS 500
#define MINUTE_HAND_WIDTH 1
#define HOUR_HAND_WIDTH 3
#define REFRESH_RATE_HZ 50

#define REFRESH_INTERVAL_MILLIS (1000 / REFRESH_RATE_HZ)

// GPS Settings
#define GPS_FIX_RATE PMTK_API_SET_FIX_CTL_100_MILLIHERTZ
#define GPS_OUTPUT_RATE PMTK_SET_NMEA_UPDATE_100_MILLIHERTZ
#define GPS_OUTPUT PMTK_SET_NMEA_OUTPUT_ZDA
#define GPS_SERIAL_BAUD_RATE 115200
#define GPS_SERIAL_BAUD_RATE_SENTENCE PMTK_SET_BAUD_115200

// Computed values from settings
// Offsets from the computed time.  Used to align the center of the hand with
// the center of the pixel.
#define SECOND_HAND_OFFSET (((1 - (SECOND_HAND_WIDTH)) * (FRACTIONAL_BRIGHTNESS)) / 2)
#define MINUTE_HAND_OFFSET (((1 - (MINUTE_HAND_WIDTH)) * (FRACTIONAL_BRIGHTNESS)) / 2)
#define HOUR_HAND_OFFSET (((1 - (HOUR_HAND_WIDTH)) * (FRACTIONAL_BRIGHTNESS)) / 2)

#define STRAND_1_FRACTIONAL_PIXELS ((STRAND_1_PIXELS) * (FRACTIONAL_BRIGHTNESS))
#define STRAND_2_FRACTIONAL_PIXELS ((STRAND_2_PIXELS) * (FRACTIONAL_BRIGHTNESS))
#define STRAND_3_FRACTIONAL_PIXELS ((STRAND_3_PIXELS) * (FRACTIONAL_BRIGHTNESS))
#define STRAND_4_FRACTIONAL_PIXELS ((STRAND_4_PIXELS) * (FRACTIONAL_BRIGHTNESS))
#define STRAND_5_FRACTIONAL_PIXELS ((STRAND_5_PIXELS) * (FRACTIONAL_BRIGHTNESS))
#define STRAND_6_FRACTIONAL_PIXELS ((STRAND_6_PIXELS) * (FRACTIONAL_BRIGHTNESS))

#define STRAND_1_SECOND_HAND_OFFSET ((SECOND_HAND_OFFSET) + (STRAND_1_OFFSET))
#define STRAND_1_MINUTE_HAND_OFFSET ((MINUTE_HAND_OFFSET) + (STRAND_1_OFFSET))
#define STRAND_1_HOUR_HAND_OFFSET ((HOUR_HAND_OFFSET) + (STRAND_1_OFFSET))
#define STRAND_2_SECOND_HAND_OFFSET ((SECOND_HAND_OFFSET) + (STRAND_2_OFFSET))
#define STRAND_2_MINUTE_HAND_OFFSET ((MINUTE_HAND_OFFSET) + (STRAND_2_OFFSET))
#define STRAND_2_HOUR_HAND_OFFSET ((HOUR_HAND_OFFSET) + (STRAND_2_OFFSET))
#define STRAND_3_SECOND_HAND_OFFSET ((SECOND_HAND_OFFSET) + (STRAND_3_OFFSET))
#define STRAND_3_MINUTE_HAND_OFFSET ((MINUTE_HAND_OFFSET) + (STRAND_3_OFFSET))
#define STRAND_3_HOUR_HAND_OFFSET ((HOUR_HAND_OFFSET) + (STRAND_3_OFFSET))
#define STRAND_4_SECOND_HAND_OFFSET ((SECOND_HAND_OFFSET) + (STRAND_4_OFFSET))
#define STRAND_4_MINUTE_HAND_OFFSET ((MINUTE_HAND_OFFSET) + (STRAND_4_OFFSET))
#define STRAND_4_HOUR_HAND_OFFSET ((HOUR_HAND_OFFSET) + (STRAND_4_OFFSET))
#define STRAND_5_SECOND_HAND_OFFSET ((SECOND_HAND_OFFSET) + (STRAND_5_OFFSET))
#define STRAND_5_MINUTE_HAND_OFFSET ((MINUTE_HAND_OFFSET) + (STRAND_5_OFFSET))
#define STRAND_5_HOUR_HAND_OFFSET ((HOUR_HAND_OFFSET) + (STRAND_5_OFFSET))
#define STRAND_6_SECOND_HAND_OFFSET ((SECOND_HAND_OFFSET) + (STRAND_6_OFFSET))
#define STRAND_6_MINUTE_HAND_OFFSET ((MINUTE_HAND_OFFSET) + (STRAND_6_OFFSET))
#define STRAND_6_HOUR_HAND_OFFSET ((HOUR_HAND_OFFSET) + (STRAND_6_OFFSET))

#define MOD(a, b) ((((a) % (b)) + (b)) % (b))
#define ST(offset, period, pixels, milli) (MOD((offset) + (milli) % (period) * (pixels) / (period), (pixels)))

// Application starts here

CRGB strand1[STRAND_1_PIXELS];  // Outermost
CRGB strand2[STRAND_2_PIXELS];
CRGB strand3[STRAND_3_PIXELS];
CRGB strand4[STRAND_4_PIXELS];
CRGB strand5[STRAND_5_PIXELS];
CRGB strand6[STRAND_6_PIXELS];  // Innermost

// Time Stuff
TimeChangeRule EDT = {"EDT", Second, Sun, Mar, 2, -240};
TimeChangeRule EST = {"EST", First, Sun, Nov, 2, -300};
Timezone myTZ(EDT, EST);

// GPS Stuff
TinyGPSPlus gps;

TinyGPSCustom zdaUtc(gps, "GNZDA", 1);
TinyGPSCustom zdaDay(gps, "GNZDA", 2);
TinyGPSCustom zdaMonth(gps, "GNZDA", 3);
TinyGPSCustom zdaYear(gps, "GNZDA", 4);

TinyGPSCustom pmtkResponseCode(gps, "PMTK001", 1);
TinyGPSCustom pmtk667ActionFlag(gps, "PMTK001", 2);
TinyGPSCustom pmtk667OldLeapSecondOffset(gps, "GNZDA", 5);
TinyGPSCustom pmtk667CurrentUtcTimeOfWeekSeconds(gps, "GNZDA", 6);
TinyGPSCustom pmtk667CurrentUtcWeekNumber(gps, "GNZDA", 7);
TinyGPSCustom pmtk667LeapSecondUtcWeekNumber(gps, "GNZDA", 8);
TinyGPSCustom pmtk667LeapSecondDayOfWeek(gps, "GNZDA", 9);
TinyGPSCustom pmtk667NewLeapSecondOffset(gps, "GNZDA", 10);

time_t currentMinute = 0;
time_t nextLeapMinute = 0;
int numberOfLeapSeconds = 0;
unsigned long ppsStartOfSecondMillis;  // Used for drift calculation
unsigned long ppsStartOfMinuteMillis;  // Used with currentMinute to get complete millisecond precision time, accounting
// for leap seconds

unsigned long secondsInMinute(time_t minute) {
  return 60 + ((minute % 60) == (nextLeapMinute % 60) ? numberOfLeapSeconds : 0);
}

unsigned long secondsInCurrentHour() {
  return 3600 + ((currentMinute % 3600) == (nextLeapMinute % 3600) ? numberOfLeapSeconds : 0);
}

void pps() {
  ppsStartOfSecondMillis = millis();
  unsigned long currentSecond = (ppsStartOfSecondMillis - ppsStartOfMinuteMillis + 500) / 1000;
  boolean minuteRollover = currentSecond == secondsInMinute(currentMinute);
  if (minuteRollover) {
    ppsStartOfMinuteMillis = ppsStartOfSecondMillis;
    currentMinute = ((currentMinute / 60) + 1) * 60;
  }
}

void updateStrand(CRGB* strand,
                  int width,
                  int offset,
                  int period,
                  int pixels,
                  unsigned long millis,
                  const CRGB& color) {
  int position = ST(offset, period, pixels, millis);
  int wholePosition = position / FRACTIONAL_BRIGHTNESS;
  int fractionalPosition = position % FRACTIONAL_BRIGHTNESS;
  CRGB handRender[width + 1];
  for (auto& i : handRender) {
    i = color;
  }
  handRender[0].fadeToBlackBy(fractionalPosition);
  strand[wholePosition].fadeToBlackBy(FRACTIONAL_BRIGHTNESS - fractionalPosition);
  handRender[width].fadeToBlackBy(FRACTIONAL_BRIGHTNESS - fractionalPosition);
  strand[wholePosition + width].fadeToBlackBy(fractionalPosition);
  for (int i = 0; i <= width; i++) {
    // if (i != 0 && i != width) {
    //    strand[position / FRACTIONAL_BRIGHTNESS + i] = CRGB::Black;
    // }
    strand[wholePosition + i] += handRender[i];
  }
}

void updateStrandSeconds(CRGB* strand,
                         time_t displayTime,
                         unsigned long millisSinceStartOfMinute,
                         unsigned long millisSinceStartOfSecond,
                         unsigned long millisSinceStartOfHour,
                         int offset,
                         int period,
                         int pixels,
                         const CHSV& color) {
  CHSV secondHandRender[SECOND_HAND_WIDTH + 1];
  for (auto& i : secondHandRender) {
    i = color;
  }
  unsigned long secondHandDiameter = (SECOND_HAND_WIDTH - 1) * FRACTIONAL_BRIGHTNESS * millisSinceStartOfSecond /
                                     (2 * SECOND_HAND_PULSE_DURATION_MILLIS);

  unsigned long lowerIndex = ((SECOND_HAND_WIDTH - 1) * FRACTIONAL_BRIGHTNESS / 2 - secondHandDiameter);
  unsigned long upperIndex = ((SECOND_HAND_WIDTH - 1) * FRACTIONAL_BRIGHTNESS / 2 + secondHandDiameter);
  secondHandRender[lowerIndex / FRACTIONAL_BRIGHTNESS].value +=
      FRACTIONAL_BRIGHTNESS - lowerIndex % FRACTIONAL_BRIGHTNESS;
  secondHandRender[lowerIndex / FRACTIONAL_BRIGHTNESS + 1].value += lowerIndex % FRACTIONAL_BRIGHTNESS;
  secondHandRender[upperIndex / FRACTIONAL_BRIGHTNESS].value +=
      FRACTIONAL_BRIGHTNESS - upperIndex % FRACTIONAL_BRIGHTNESS;
  secondHandRender[upperIndex / FRACTIONAL_BRIGHTNESS + 1].value += upperIndex % FRACTIONAL_BRIGHTNESS;
  if ((millisSinceStartOfMinute / 1000) % 2 > 0) {
    int secondHand = ST(offset, period * secondsInMinute(displayTime) * 1000, pixels, millisSinceStartOfHour);
    for (int i = 0; i < SECOND_HAND_WIDTH + 1; i++) {
      strand[secondHand + i] += secondHandRender[i];
    }
  }
}

void setClock() {
  // Get timestamp
  unsigned long displayStartOfMinuteMillis = ppsStartOfMinuteMillis;
  time_t displayTime = myTZ.toLocal(currentMinute);
  unsigned long displayMillis = millis();
  if (displayStartOfMinuteMillis != ppsStartOfMinuteMillis) {
    displayStartOfMinuteMillis = ppsStartOfMinuteMillis;
    displayTime = myTZ.toLocal(currentMinute);
    displayMillis = millis();
  }
  // Determine hand positions
  unsigned long millisSinceStartOfMinute = displayMillis - displayStartOfMinuteMillis;
  unsigned long millisSinceStartOfSecond = millisSinceStartOfMinute % 1000;
  unsigned long millisSinceStartOfHour = (displayTime % 3600) * 1000 + millisSinceStartOfMinute;

  // Strand 1
  // start point of hand (minute) = 1/2 of brightness - 1/2 of width of hand + (millisSinceStartOfHour % millisInPeriod)
  // / millisPerFractionalPixel

  // Set hours on strands
  unsigned long millisSinceStartOfDay = hour(displayTime) * secondsInCurrentHour() * 1000 + millisSinceStartOfHour;
  // Not accurate if the current day has leap seconds.  Uses secondsInCurrentHour to handle creating an interval that
  // matches the period of the current hour.
  updateStrand(strand1, HOUR_HAND_WIDTH, STRAND_1_HOUR_HAND_OFFSET,
               STRAND_1_HOURS_PERIOD * secondsInCurrentHour() * 1000, STRAND_1_FRACTIONAL_PIXELS, millisSinceStartOfDay,
               CRGB::Red);
  updateStrand(strand2, HOUR_HAND_WIDTH, STRAND_2_HOUR_HAND_OFFSET,
               STRAND_2_HOURS_PERIOD * secondsInCurrentHour() * 1000, STRAND_2_FRACTIONAL_PIXELS, millisSinceStartOfDay,
               CRGB::Red);
  updateStrand(strand3, HOUR_HAND_WIDTH, STRAND_3_HOUR_HAND_OFFSET,
               STRAND_3_HOURS_PERIOD * secondsInCurrentHour() * 1000, STRAND_3_FRACTIONAL_PIXELS, millisSinceStartOfDay,
               CRGB::Red);
  updateStrand(strand4, HOUR_HAND_WIDTH, STRAND_4_HOUR_HAND_OFFSET,
               STRAND_4_HOURS_PERIOD * secondsInCurrentHour() * 1000, STRAND_4_FRACTIONAL_PIXELS, millisSinceStartOfDay,
               CRGB::Red);
  updateStrand(strand5, HOUR_HAND_WIDTH, STRAND_5_HOUR_HAND_OFFSET,
               STRAND_5_HOURS_PERIOD * secondsInCurrentHour() * 1000, STRAND_5_FRACTIONAL_PIXELS, millisSinceStartOfDay,
               CRGB::Red);
  updateStrand(strand6, HOUR_HAND_WIDTH, STRAND_6_HOUR_HAND_OFFSET,
               STRAND_6_HOURS_PERIOD * secondsInCurrentHour() * 1000, STRAND_6_FRACTIONAL_PIXELS, millisSinceStartOfDay,
               CRGB::Red);

  // Set minutes on strands
  updateStrand(strand1, MINUTE_HAND_WIDTH, STRAND_1_MINUTE_HAND_OFFSET,
               STRAND_1_MINUTES_PERIOD * secondsInMinute(displayTime) * 1000, STRAND_1_FRACTIONAL_PIXELS,
               millisSinceStartOfHour, CRGB::Blue);
  updateStrand(strand2, MINUTE_HAND_WIDTH, STRAND_2_MINUTE_HAND_OFFSET,
               STRAND_2_MINUTES_PERIOD * secondsInMinute(displayTime) * 1000, STRAND_2_FRACTIONAL_PIXELS,
               millisSinceStartOfHour, CRGB::Blue);
  updateStrand(strand3, MINUTE_HAND_WIDTH, STRAND_3_MINUTE_HAND_OFFSET,
               STRAND_3_MINUTES_PERIOD * secondsInMinute(displayTime) * 1000, STRAND_3_FRACTIONAL_PIXELS,
               millisSinceStartOfHour, CRGB::Blue);
  updateStrand(strand4, MINUTE_HAND_WIDTH, STRAND_4_MINUTE_HAND_OFFSET,
               STRAND_4_MINUTES_PERIOD * secondsInMinute(displayTime) * 1000, STRAND_4_FRACTIONAL_PIXELS,
               millisSinceStartOfHour, CRGB::Blue);
  updateStrand(strand5, MINUTE_HAND_WIDTH, STRAND_5_MINUTE_HAND_OFFSET,
               STRAND_5_MINUTES_PERIOD * secondsInMinute(displayTime) * 1000, STRAND_5_FRACTIONAL_PIXELS,
               millisSinceStartOfHour, CRGB::Blue);
  updateStrand(strand6, MINUTE_HAND_WIDTH, STRAND_6_MINUTE_HAND_OFFSET,
               STRAND_6_MINUTES_PERIOD * secondsInMinute(displayTime) * 1000, STRAND_6_FRACTIONAL_PIXELS,
               millisSinceStartOfHour, CRGB::Blue);

  //   // Set seconds on strands
  //   if (millisSinceStartOfSecond < SECOND_HAND_PULSE_DURATION_MILLIS) {
  //     updateStrandSeconds(strand1, displayTime, millisSinceStartOfMinute, millisSinceStartOfSecond,
  //                         millisSinceStartOfHour, STRAND_1_SECOND_HAND_OFFSET, STRAND_1_MINUTES_PERIOD,
  //                         STRAND_1_FRACTIONAL_PIXELS, CHSV(85, 255, 0));
  //     updateStrandSeconds(strand2, displayTime, millisSinceStartOfMinute, millisSinceStartOfSecond,
  //                         millisSinceStartOfHour, STRAND_2_SECOND_HAND_OFFSET, STRAND_2_MINUTES_PERIOD,
  //                         STRAND_2_FRACTIONAL_PIXELS, CHSV(85, 255, 0));
  //     updateStrandSeconds(strand3, displayTime, millisSinceStartOfMinute, millisSinceStartOfSecond,
  //                         millisSinceStartOfHour, STRAND_3_SECOND_HAND_OFFSET, STRAND_3_MINUTES_PERIOD,
  //                         STRAND_3_FRACTIONAL_PIXELS, CHSV(85, 255, 0));
  //     updateStrandSeconds(strand4, displayTime, millisSinceStartOfMinute, millisSinceStartOfSecond,
  //                         millisSinceStartOfHour, STRAND_4_SECOND_HAND_OFFSET, STRAND_4_MINUTES_PERIOD,
  //                         STRAND_4_FRACTIONAL_PIXELS, CHSV(85, 255, 0));
  //   }
}

void set12oClock() {
  strand1[STRAND_1_PIXELS - 1] = strand1[1] = CRGB::Red;
  strand2[STRAND_2_PIXELS - 1] = strand2[1] = CRGB::Red;
  strand3[STRAND_3_PIXELS / 2 - 1] = strand3[STRAND_3_PIXELS / 2] = strand3[STRAND_3_PIXELS / 2 + 1] = CRGB::Red;
  strand4[STRAND_4_PIXELS / 2 - 1] = strand4[STRAND_4_PIXELS / 2] = strand4[STRAND_4_PIXELS / 2 + 1] = CRGB::Red;
  strand5[STRAND_5_PIXELS - 1] = strand5[1] = CRGB::Red;
  strand6[STRAND_6_PIXELS - 1] = strand6[1] = CRGB::Red;
  strand1[0] = strand2[0] = strand3[0] = strand4[0] = strand5[0] = strand6[0] = CRGB::Blue;
}

void setup() {
  CFastLED::addLeds<APA102, STRAND_1_DATA_PIN, STRAND_1_CLOCK_PIN, BGR>(strand1, STRAND_1_PIXELS);
  CFastLED::addLeds<APA102, STRAND_2_DATA_PIN, STRAND_2_CLOCK_PIN, BGR>(strand2, STRAND_2_PIXELS);
  CFastLED::addLeds<APA102, STRAND_3_DATA_PIN, STRAND_3_CLOCK_PIN, BGR>(strand3, STRAND_3_PIXELS);
  CFastLED::addLeds<APA102, STRAND_4_DATA_PIN, STRAND_4_CLOCK_PIN, BGR>(strand4, STRAND_4_PIXELS);
  CFastLED::addLeds<APA102, STRAND_5_DATA_PIN, STRAND_5_CLOCK_PIN, BGR>(strand5, STRAND_5_PIXELS);
  CFastLED::addLeds<APA102, STRAND_6_DATA_PIN, STRAND_6_CLOCK_PIN, BGR>(strand6, STRAND_6_PIXELS);
  GPS_SERIAL.begin(GPS_SERIAL_BAUD_RATE);
  GPS_SERIAL.println(GPS_SERIAL_BAUD_RATE_SENTENCE);
  GPS_SERIAL.println(GPS_FIX_RATE);
  GPS_SERIAL.println(GPS_OUTPUT_RATE);
  GPS_SERIAL.println(GPS_OUTPUT);
  GPS_SERIAL.println();
  attachInterrupt(digitalPinToInterrupt(GPS_PPS_PIN), pps, RISING);
#ifdef DEBUG
  Serial.begin(9600);
#endif
}

// Refresh rate timing
unsigned long startTimeMillis;
unsigned long lastRefreshMillis;

#ifdef DEBUG
unsigned long debugRefreshSum = 0;
unsigned long debugRefreshCount = 0;
#endif

void loop() {
  startTimeMillis = millis();
  if (startTimeMillis - lastRefreshMillis > REFRESH_INTERVAL_MILLIS) {
    if (zdaUtc.isValid()) {
      setClock();
    } else if (startTimeMillis % 1024 < 512) {
      set12oClock();
    }

    // Show
    FastLED.show();

    // Clear pixels
    for (auto& i : strand1) {
      i = CRGB::Black;
    }
    for (auto& i : strand2) {
      i = CRGB::Black;
    }
    for (auto& i : strand3) {
      i = CRGB::Black;
    }
    for (auto& i : strand4) {
      i = CRGB::Black;
    }
    for (auto& i : strand5) {
      i = CRGB::Black;
    }
    for (auto& i : strand6) {
      i = CRGB::Black;
    }

    // Set next refresh (startTimeMillis + 16.67 = 60Hz, startTimeMillis + 10 = 100Hz)
    lastRefreshMillis = startTimeMillis;

#ifdef DEBUG
    debugRefreshSum += millis() - startTimeMillis;
    debugRefreshCount++;
#endif
  }
  // Check for GPS updates
  while (GPS_SERIAL.available()) {
    gps.encode(GPS_SERIAL.read());
  }
  if (zdaUtc.isUpdated()) {
    tmElements_t tm;
    tm.Year = CalendarYrToTm(atoi(zdaYear.value()));
    tm.Month = atoi(zdaMonth.value());
    tm.Day = atoi(zdaDay.value());
    char hr[2], min[2], sec[2], ms[3];
    uint8_t seconds;
    uint32_t milliseconds;
    sscanf(zdaUtc.value(), "%2s%2s%2s.%3s", hr, min, sec, ms);
    tm.Hour = atoi(hr);
    tm.Minute = atoi(min);
    seconds = atoi(sec);
    milliseconds = atoi(ms) + zdaUtc.age() + (seconds * 1000);
    tm.Second = 0;

    time_t gpsMinute = makeTime(tm);
    if (gpsMinute != currentMinute) {
      currentMinute = gpsMinute;
      if (seconds != (ppsStartOfSecondMillis - ppsStartOfMinuteMillis + 200) / 1000) {
        ppsStartOfMinuteMillis = ppsStartOfSecondMillis - (seconds * 1000);
      }
    }
#ifdef DEBUG
    Serial.printf("Delay in ms: %d\n", millis() - ppsStartOfMinuteMillis - milliseconds);
    Serial.printf("Display time in ms: %d.%03d\n", (debugRefreshSum * 1000 / debugRefreshCount) / 1000,
                  (debugRefreshSum * 1000 / debugRefreshCount) % 1000);
    debugRefreshSum = 0;
    debugRefreshCount = 0;
#endif
  }
}
