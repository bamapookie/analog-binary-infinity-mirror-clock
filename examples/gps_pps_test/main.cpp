#include <Arduino.h>
#include <SPI.h>
#include <TimeLib.h>
#include <TinyGPS++.h>
#include <abimc_pcb.h>
#include <xa1110.h>

#define REFRESH_RATE_HZ 2
#define REFRESH_INTERVAL_MILLIS (1000 / REFRESH_RATE_HZ)

// GPS Settings
#define GPS_FIX_RATE PMTK_API_SET_FIX_CTL_100_MILLIHERTZ
#define GPS_OUTPUT_RATE PMTK_SET_NMEA_UPDATE_100_MILLIHERTZ
#define GPS_OUTPUT PMTK_SET_NMEA_OUTPUT_ZDA
#define GPS_SERIAL_BAUD_RATE 115200
#define GPS_SERIAL_BAUD_RATE_SENTENCE PMTK_SET_BAUD_115200

// GPS Stuff
TinyGPSPlus gps;

void pps() {
  Serial.write("PPS!\n");
}

void setup() {
  attachInterrupt(digitalPinToInterrupt(GPS_PPS_PIN), pps, RISING);
  GPS_SERIAL.begin(GPS_SERIAL_BAUD_RATE);
  GPS_SERIAL.println(GPS_SERIAL_BAUD_RATE_SENTENCE);
  GPS_SERIAL.println(GPS_FIX_RATE);
  GPS_SERIAL.println(GPS_OUTPUT_RATE);
  GPS_SERIAL.println(GPS_OUTPUT);
}

unsigned long startTimeMillis;
unsigned long lastRefreshMillis;

void loop() {
  startTimeMillis = millis();
  if (startTimeMillis - lastRefreshMillis > REFRESH_INTERVAL_MILLIS) {
    if (gps.time.isValid()) {
      Serial.write("Fix!\n");
    } else {
      Serial.write("No fix.\n");
    }
   lastRefreshMillis = startTimeMillis;
  }
  // Check for GPS updates
  while (GPS_SERIAL.available()) {
    gps.encode(GPS_SERIAL.read());
  }
}
