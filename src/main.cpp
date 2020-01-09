#include <Arduino.h>
#include <FastLED.h>
#include <TimeLib.h>
#include <Timezone.h>
#include <abimc_pcb.h>
#include <xa1110.h>
#include <TinyGPS++.h>

#define DEBUG_REFRESH

// Settings
// LED Settings
#define STRAND_1_PIXELS 144
#define STRAND_1_OFFSET 0
#define STRAND_1_MINUTES_PERIOD 2
#define STRAND_1_HOURS_PERIOD 2
// #define STRAND_2_PIXELS 120
// #define STRAND_2_OFFSET 0
// #define STRAND_2_MINUTES_PERIOD 4
// #define STRAND_2_HOURS_PERIOD 4
// #define STRAND_3_PIXELS 96
// #define STRAND_3_OFFSET 0
// #define STRAND_3_MINUTES_PERIOD 8
// #define STRAND_3_HOURS_PERIOD 8
// #define STRAND_4_PIXELS 72
// #define STRAND_4_OFFSET 0
// #define STRAND_4_MINUTES_PERIOD 16
// #define STRAND_4_HOURS_PERIOD 16
// #define STRAND_5_PIXELS 48
// #define STRAND_5_OFFSET 0
// #define STRAND_5_MINUTES_PERIOD 32
// #define STRAND_5_HOURS_PERIOD 32
// #define STRAND_6_PIXELS 24
// #define STRAND_6_OFFSET 0
// #define STRAND_6_MINUTES_PERIOD 64
// #define STRAND_6_HOURS_PERIOD 0

#define FRACTIONAL_BRIGHTNESS 256
#define SECOND_HAND_WIDTH 11
#define SECOND_HAND_PULSE_DURATION_MILLIS 500
#define MINUTE_HAND_WIDTH 1
#define HOUR_HAND_WIDTH 3
#define REFRESH_RATE_HZ 50

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
// #define STRAND_2_FRACTIONAL_PIXELS ((STRAND_2_PIXELS) * (FRACTIONAL_BRIGHTNESS))
// #define STRAND_3_FRACTIONAL_PIXELS ((STRAND_3_PIXELS) * (FRACTIONAL_BRIGHTNESS))
// #define STRAND_4_FRACTIONAL_PIXELS ((STRAND_4_PIXELS) * (FRACTIONAL_BRIGHTNESS))
// #define STRAND_5_FRACTIONAL_PIXELS ((STRAND_5_PIXELS) * (FRACTIONAL_BRIGHTNESS))
// #define STRAND_6_FRACTIONAL_PIXELS ((STRAND_6_PIXELS) * (FRACTIONAL_BRIGHTNESS))

#define STRAND_1_SECOND_HAND_OFFSET ((SECOND_HAND_OFFSET) + (STRAND_1_OFFSET))
#define STRAND_1_MINUTE_HAND_OFFSET ((MINUTE_HAND_OFFSET) + (STRAND_1_OFFSET))
#define STRAND_1_HOUR_HAND_OFFSET ((HOUR_HAND_OFFSET) + (STRAND_1_OFFSET))
//#define STRAND_2_SECOND_HAND_OFFSET ((SECOND_HAND_OFFSET) + (STRAND_2_OFFSET))
//#define STRAND_2_MINUTE_HAND_OFFSET ((MINUTE_HAND_OFFSET) + (STRAND_2_OFFSET))
//#define STRAND_2_HOUR_HAND_OFFSET ((HOUR_HAND_OFFSET) + (STRAND_2_OFFSET))
//#define STRAND_3_SECOND_HAND_OFFSET ((SECOND_HAND_OFFSET) + (STRAND_3_OFFSET))
//#define STRAND_3_MINUTE_HAND_OFFSET ((MINUTE_HAND_OFFSET) + (STRAND_3_OFFSET))
//#define STRAND_3_HOUR_HAND_OFFSET ((HOUR_HAND_OFFSET) + (STRAND_3_OFFSET))
//#define STRAND_4_SECOND_HAND_OFFSET ((SECOND_HAND_OFFSET) + (STRAND_4_OFFSET))
//#define STRAND_4_MINUTE_HAND_OFFSET ((MINUTE_HAND_OFFSET) + (STRAND_4_OFFSET))
//#define STRAND_4_HOUR_HAND_OFFSET ((HOUR_HAND_OFFSET) + (STRAND_4_OFFSET))
//#define STRAND_5_SECOND_HAND_OFFSET ((SECOND_HAND_OFFSET) + (STRAND_5_OFFSET))
//#define STRAND_5_MINUTE_HAND_OFFSET ((MINUTE_HAND_OFFSET) + (STRAND_5_OFFSET))
//#define STRAND_5_HOUR_HAND_OFFSET ((HOUR_HAND_OFFSET) + (STRAND_5_OFFSET))
//#define STRAND_6_SECOND_HAND_OFFSET ((SECOND_HAND_OFFSET) + (STRAND_6_OFFSET))
//#define STRAND_6_MINUTE_HAND_OFFSET ((MINUTE_HAND_OFFSET) + (STRAND_6_OFFSET))
//#define STRAND_6_HOUR_HAND_OFFSET ((HOUR_HAND_OFFSET) + (STRAND_6_OFFSET))

#define MOD(a, b) ((((a) % (b)) + (b)) % (b))
#define ST(offset, period, pixels, milli) (MOD((offset) + (milli) % (period) * (pixels) / (period), (pixels)))

CRGB strand1[STRAND_1_PIXELS];  // Outermost NOLINT(cert-err58-cpp)
// CRGB strand2[STRAND_2_PIXELS]; // NOLINT(cert-err58-cpp)
// CRGB strand3[STRAND_3_PIXELS]; // NOLINT(cert-err58-cpp)
// CRGB strand4[STRAND_4_PIXELS]; // NOLINT(cert-err58-cpp)
// CRGB strand5[STRAND_5_PIXELS]; // NOLINT(cert-err58-cpp)
// CRGB strand6[STRAND_6_PIXELS];  // Innermost NOLINT(cert-err58-cpp)

// Time Stuff
TimeChangeRule EDT = {"EDT", Second, Sun, Mar, 2, -240};
TimeChangeRule EST = {"EST", First, Sun, Nov, 2, -300};
Timezone myTZ(EDT, EST); // NOLINT(cert-err58-cpp)

// GPS Stuff
TinyGPSPlus gps; // NOLINT(cert-err58-cpp)

// Application starts here
// Refresh rate timing
unsigned long startTimeMillis;
unsigned long nextRefreshMillis;
#ifdef DEBUG_REFRESH
unsigned int debugRefresh = 0;
#endif

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

void
updateStrand(CRGB *strand, int width, int offset, int period, int pixels, unsigned long millis, const CRGB &color) {
    int position = ST(offset, period, pixels, millis);
    CRGB handRender[width + 1];
    for (int i = 0; i < width; i++) {
        handRender[1] = color;
    }
    handRender[0].fadeToBlackBy(position % FRACTIONAL_BRIGHTNESS);
    strand[position / FRACTIONAL_BRIGHTNESS].fadeToBlackBy(FRACTIONAL_BRIGHTNESS - (position % FRACTIONAL_BRIGHTNESS));
    handRender[width].fadeToBlackBy(FRACTIONAL_BRIGHTNESS - (position % FRACTIONAL_BRIGHTNESS));
    strand[position / FRACTIONAL_BRIGHTNESS + width].fadeToBlackBy(position % FRACTIONAL_BRIGHTNESS);
    for (int i = 0; i <= width; i++) {
//        if (i != 0 && i != width) {
//            strand[position / FRACTIONAL_BRIGHTNESS + i] = CRGB::Black;
//        }
        strand[position / FRACTIONAL_BRIGHTNESS + i] += handRender[i];
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
    updateStrand(strand1, HOUR_HAND_WIDTH, STRAND_1_HOUR_HAND_OFFSET,
                 STRAND_1_HOURS_PERIOD * secondsInCurrentHour() * 1000,
                 STRAND_1_FRACTIONAL_PIXELS,
                 hour(displayTime) * secondsInCurrentHour() * 1000 + millisSinceStartOfHour, CRGB::Red);

    // Set minutes on strands
    updateStrand(strand1, MINUTE_HAND_WIDTH, STRAND_1_MINUTE_HAND_OFFSET,
                 STRAND_1_MINUTES_PERIOD * secondsInMinute(displayTime) * 1000,
                 STRAND_1_FRACTIONAL_PIXELS, millisSinceStartOfHour, CRGB::Blue);

    // Set seconds on strands
    if (millisSinceStartOfSecond < SECOND_HAND_PULSE_DURATION_MILLIS) {
        CHSV color = CHSV(85, 255, 0); // Green
        CHSV secondHandRender[SECOND_HAND_WIDTH + 1];
        for (auto &i : secondHandRender) {
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
            int strand1SecondHand = ST(STRAND_1_SECOND_HAND_OFFSET,
                                       STRAND_1_MINUTES_PERIOD * secondsInMinute(displayTime) * 1000,
                                       STRAND_1_FRACTIONAL_PIXELS, millisSinceStartOfHour);
            for (int i = 0; i < SECOND_HAND_WIDTH + 1; i++) {
                strand1[strand1SecondHand + i] += secondHandRender[i];
            }
        }
    }

}

void set12oClock() {
    if (startTimeMillis % 1024 < 512) {
        strand1[STRAND_1_PIXELS - 1] = strand1[1] = CRGB::Red;
        // strand2[STRAND_2_PIXELS - 1] = strand2[1] = CRGB::Red;
        // strand3[STRAND_3_PIXELS / 2 - 1] = strand3[STRAND_3_PIXELS / 2] = strand3[STRAND_3_PIXELS / 2 + 1] = CRGB::Red;
        // strand4[STRAND_4_PIXELS / 2 - 1] = strand4[STRAND_4_PIXELS / 2] = strand4[STRAND_4_PIXELS / 2 + 1] = CRGB::Red;
        // strand5[STRAND_5_PIXELS - 1] = strand5[1] = CRGB::Red;
        // strand6[STRAND_6_PIXELS - 1] = strand6[1] = CRGB::Red;
        strand1[0] = CRGB::Blue;
        // strand1[0] = strand2[0] = strand3[0] = strand4[0] = strand5[0] = strand6[0] = CRGB::Blue;
    }
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

void setup() {
    CFastLED::addLeds<APA102, STRAND_1_DATA_PIN, STRAND_1_CLOCK_PIN, RGB>(strand1, STRAND_1_PIXELS);
    // FastLED.addLeds<APA102, STRAND_2_DATA_PIN, STRAND_2_CLOCK_PIN, RGB>(strand2, STRAND_2_PIXELS);
    // FastLED.addLeds<APA102, STRAND_3_DATA_PIN, STRAND_3_CLOCK_PIN, RGB>(strand3, STRAND_3_PIXELS);
    // FastLED.addLeds<APA102, STRAND_4_DATA_PIN, STRAND_4_CLOCK_PIN, RGB>(strand4, STRAND_4_PIXELS);
    // FastLED.addLeds<APA102, STRAND_5_DATA_PIN, STRAND_5_CLOCK_PIN, RGB>(strand5, STRAND_5_PIXELS);
    // FastLED.addLeds<APA102, STRAND_6_DATA_PIN, STRAND_6_CLOCK_PIN, RGB>(strand6, STRAND_6_PIXELS);
    attachInterrupt(digitalPinToInterrupt(GPS_PPS_PIN), pps, RISING);
    GPS_SERIAL.begin(GPS_SERIAL_BAUD_RATE);
    GPS_SERIAL.println(GPS_SERIAL_BAUD_RATE_SENTENCE);
    GPS_SERIAL.println(GPS_FIX_RATE);
    GPS_SERIAL.println(GPS_OUTPUT_RATE);
    GPS_SERIAL.println(GPS_OUTPUT);
}

void loop() {
    startTimeMillis = millis();
    if (nextRefreshMillis - startTimeMillis <= 0) {
        if (gps.time.isValid()) {
            setClock();
        } else {
            set12oClock();
        }

#ifdef DEBUG_REFRESH
        strand1[debugRefresh] = CRGB(32, 32, 32);
#endif

        // Show
        FastLED.show();

        // Clear pixels
        for (auto &i : strand1) {
            i = CRGB::Black;
        }
        // for (int i = 0; i < STRAND_2_PIXELS; i++) {
        //   strand2[i] = CRGB::Black;
        // }
        // for (int i = 0; i < STRAND_3_PIXELS; i++) {
        //   strand3[i] = CRGB::Black;
        // }
        // for (int i = 0; i < STRAND_4_PIXELS; i++) {
        //   strand4[i] = CRGB::Black;
        // }
        // for (int i = 0; i < STRAND_5_PIXELS; i++) {
        //   strand5[i] = CRGB::Black;
        // }
        // for (int i = 0; i < STRAND_6_PIXELS; i++) {
        //   strand6[i] = CRGB::Black;
        // }

        // Set next refresh (startTimeMillis + 16.67 = 60Hz, startTimeMillis + 10 = 100Hz)
        nextRefreshMillis = startTimeMillis + (1000 / REFRESH_RATE_HZ);

#ifdef DEBUG_REFRESH
        debugRefresh = millis() - startTimeMillis;
#endif
    }
    // Check for GPS updates
    while (GPS_SERIAL.available() > 0) {
        gps.encode(GPS_SERIAL.read());
    }
    if (gps.date.isUpdated() && gps.time.isUpdated()) {
        tmElements_t tm;
        tm.Year = CalendarYrToTm(gps.date.year());
        tm.Month = gps.date.month();
        tm.Day = gps.date.day();
        tm.Hour = gps.time.hour();
        tm.Minute = gps.time.minute();
        tm.Second = 0;

        time_t gpsMinute = makeTime(tm);
        if (gpsMinute != currentMinute) {
            currentMinute = gpsMinute;
            if (gps.time.second() != (ppsStartOfSecondMillis - ppsStartOfMinuteMillis + 200) / 1000) {
                ppsStartOfMinuteMillis = ppsStartOfSecondMillis - (gps.time.second() * 1000);
            }
        }

    }
}

#pragma clang diagnostic pop
