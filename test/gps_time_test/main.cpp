#include <Arduino.h>
#include <FastLED.h>
#include <SPI.h>
#include <TimeLib.h>
#include <Timezone.h>
#include <TinyGPS++.h>

#define ABIMC_V_1_3

#include <abimc_pcb.h>
#include <xa1110.h>

#define REFRESH_RATE_HZ 50
#define REFRESH_INTERVAL_MILLIS (1000 / REFRESH_RATE_HZ)

// GPS Settings
#define GPS_FIX_RATE PMTK_API_SET_FIX_CTL_100_MILLIHERTZ
#define GPS_OUTPUT_RATE PMTK_SET_NMEA_UPDATE_100_MILLIHERTZ
#define GPS_OUTPUT PMTK_SET_NMEA_OUTPUT_ZDA
#define GPS_SERIAL_BAUD_RATE 115200
#define GPS_SERIAL_BAUD_RATE_SENTENCE PMTK_SET_BAUD_115200

// GPS Stuff
TinyGPSPlus gps;

TinyGPSCustom zdaUtc(gps, "GNZDA", 1);
TinyGPSCustom zdaDay(gps, "GNZDA", 2);
TinyGPSCustom zdaMonth(gps, "GNZDA", 3);
TinyGPSCustom zdaYear(gps, "GNZDA", 4);

time_t currentMinute = 0;
unsigned long ppsStartOfSecondMillis;  // Used for drift calculation
unsigned long ppsStartOfMinuteMillis;  // Used with currentMinute to get complete millisecond precision time, accounting
// for leap seconds

void pps() {
  ppsStartOfSecondMillis = millis();
  unsigned long currentSecond = (ppsStartOfSecondMillis - ppsStartOfMinuteMillis + 500) / 1000;
  boolean minuteRollover = currentSecond == 60; // Not handling leap seconds yet.
  if (minuteRollover) {
    ppsStartOfMinuteMillis = ppsStartOfSecondMillis;
    currentMinute = ((currentMinute / 60) + 1) * 60;
  }
}

void setup() {
  attachInterrupt(digitalPinToInterrupt(GPS_PPS_PIN), pps, RISING);
  GPS_SERIAL.begin(GPS_SERIAL_BAUD_RATE);
  GPS_SERIAL.println(GPS_SERIAL_BAUD_RATE_SENTENCE);
  GPS_SERIAL.println(GPS_FIX_RATE);
  GPS_SERIAL.println(GPS_OUTPUT_RATE);
  GPS_SERIAL.println(GPS_OUTPUT);
}

// Refresh rate timing
unsigned long startTimeMillis;
unsigned long lastRefreshMillis;

void loop() {
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
    Serial.printf("%04d-%02d-%02dT%02d:%02d:%02d.%03d", tmYearToCalendar(tm.Year), tm.Month, tm.Day, tm.Hour, tm.Minute,
                  seconds, milliseconds);
    Serial.printf("Delay in ms: %d\n", millis() - ppsStartOfMinuteMillis - milliseconds);
  }
}
