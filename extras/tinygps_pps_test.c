#include <TinyGPS++.h>
#include <mtk33x9.h>

#define GpsSerial Serial1
#define PpsPin 4
#define GpsEcho true
#define SkipReadings 5

//#define PreviousBaudRate 9600
//#define PreviousBaudRate 57600
#define PreviousBaudRate 115200

//#define NewBaudRate 9600
//#define NewBaudRate 57600
#define NewBaudRate 115200

#define NmeaUpdateRate PMTK_SET_NMEA_UPDATE_10HZ
//#define NmeaUpdateRate PMTK_SET_NMEA_UPDATE_5HZ
//#define NmeaUpdateRate PMTK_SET_NMEA_UPDATE_1HZ

#define FixRate PMTK_API_SET_FIX_CTL_100_MILLIHERTZ
//#define FixRate PMTK_API_SET_FIX_CTL_200_MILLIHERTZ
//#define FixRate PMTK_API_SET_FIX_CTL_1HZ
//#define FixRate PMTK_API_SET_FIX_CTL_5HZ

bool displayPps = false, displayParseDelay = false;

uint32_t ppsStart = 0, ppsEnd = 0;
uint32_t msparsedelaymax = 0;
uint32_t msparsedelaymin = msparsedelaymax - 1;
int skipReadings = SkipReadings;

// Adafruit_GPS GPS(&GpsSerial);
TinyGPSPlus gps;

void catchPps() {
  if (digitalRead(PpsPin) == HIGH) {
    ppsStart = millis();
  } else {
    ppsEnd = millis();
    displayPps = true;
    if (skipReadings > 0)
      skipReadings--;
    else {
      msparsedelaymin++;
      msparsedelaymax--;
    }
  }
}

void setup() {
  while (!Serial)
    ;
  Serial.begin(115200);
  Serial.println("TinyGPS++ library NMEA message delay test!");

  GpsSerial.begin(PreviousBaudRate);
  GpsSerial.println(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  GpsSerial.println(FixRate);
  GpsSerial.println(NmeaUpdateRate);

  attachInterrupt(PpsPin, catchPps, CHANGE);
  delay(2000);

  if (PreviousBaudRate != NewBaudRate) {
    if (NewBaudRate == 9600) {
      GpsSerial.println(PMTK_SET_BAUD_9600);
    } else if (NewBaudRate == 57600) {
      GpsSerial.println(PMTK_SET_BAUD_57600);
    } else if (NewBaudRate == 115200) {
      GpsSerial.println("$PMTK251,115200*1F");
    }
    delay(500);
    GpsSerial.end();
    delay(500);
    GpsSerial.begin(NewBaudRate);
  }

  GpsSerial.println(PMTK_Q_RELEASE);
  GpsSerial.println("$PMTK667*35");
}

void loop() {
  while (GpsSerial.available()) {
    char c = GpsSerial.read();
    if (GpsEcho) {
      Serial.print(c);
    }
    if (gps.encode(c)) {
      if (GpsEcho) {
        Serial.println();
      }
      if (displayPps) {
        Serial.print("PPS Length:");
        Serial.println(ppsEnd - ppsStart);
        displayPps = false;
      }
      if (skipReadings == 0) {
        uint32_t msparsedelay = millis() - ppsStart - gps.time.millisecond();
        msparsedelaymin = min(msparsedelaymin, msparsedelay);
        msparsedelaymax = max(msparsedelaymax, msparsedelay) % 1000;
        Serial.print("Parse Delay:");
        Serial.print(msparsedelay);
        Serial.print(" Min:");
        Serial.print(msparsedelaymin);
        Serial.print(" Max:");
        Serial.println(msparsedelaymax);
        char sz[32];
        sprintf(sz, "%02d/%02d/%02d ", gps.date.month(), gps.date.day(),
                gps.date.year());
        Serial.print(sz);
        sprintf(sz, "%02d:%02d:%02d.%03d ", gps.time.hour(), gps.time.minute(),
                gps.time.second(), gps.time.millisecond());
        Serial.println(sz);
      }
    }
  }
}