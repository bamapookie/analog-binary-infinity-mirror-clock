#include <Arduino.h>
#include <SPI.h>
#include <xa1110.h>

void setup() {
  while (!Serial)
    ;
  while (!Serial1)
    ;
  Serial.begin(9600);
  // fixBaudRate();
  Serial1.begin(115200);
  Serial1.println("$PMTK250,3,3,115200*1E");
  Serial1.println("$PMTK253,0*2A");
  Serial1.println(PMTK_SET_BAUD_115200);
}

void loop() {
  while (Serial.available())
    Serial1.write(Serial.read());
  while (Serial1.available())
    Serial.write(Serial1.read());
}

void fixBaudRate() {
  Serial.println("Setting 4800.");
  Serial1.begin(4800);  // 4800,9600,14400,19200,38400,57600,115200,230400,460800,921600
  Serial1.println("$PMTK250,3,3,115200*1E");
  Serial1.println("$PMTK253,0*2A");
  Serial1.println(PMTK_SET_BAUD_115200);
  delay(500);
  Serial1.end();
  delay(500);
  Serial.println("Setting 9600.");
  Serial1.begin(9600);  // 4800,9600,14400,19200,38400,57600,115200,230400,460800,921600
  Serial1.println("$PMTK250,3,3,115200*1E");
  Serial1.println("$PMTK253,0*2A");
  Serial1.println(PMTK_SET_BAUD_115200);
  delay(500);
  Serial1.end();
  delay(500);
  Serial.println("Setting 14400.");
  Serial1.begin(14400);  // 4800,9600,14400,19200,38400,57600,115200,230400,460800,921600
  Serial1.println("$PMTK250,3,3,115200*1E");
  Serial1.println("$PMTK253,0*2A");
  Serial1.println(PMTK_SET_BAUD_115200);
  delay(500);
  Serial1.end();
  delay(500);
  Serial.println("Setting 19200.");
  Serial1.begin(19200);  // 4800,9600,14400,19200,38400,57600,115200,230400,460800,921600
  Serial1.println("$PMTK250,3,3,115200*1E");
  Serial1.println("$PMTK253,0*2A");
  Serial1.println(PMTK_SET_BAUD_115200);
  delay(500);
  Serial1.end();
  delay(500);
  Serial.println("Setting 38400.");
  Serial1.begin(38400);  // 4800,9600,14400,19200,38400,57600,115200,230400,460800,921600
  Serial1.println("$PMTK250,3,3,115200*1E");
  Serial1.println("$PMTK253,0*2A");
  Serial1.println(PMTK_SET_BAUD_115200);
  delay(500);
  Serial1.end();
  delay(500);
  Serial.println("Setting 57600.");
  Serial1.begin(57600);  // 4800,9600,14400,19200,38400,57600,115200,230400,460800,921600
  Serial1.println("$PMTK250,3,3,115200*1E");
  Serial1.println("$PMTK253,0*2A");
  Serial1.println(PMTK_SET_BAUD_115200);
  delay(500);
  Serial1.end();
  delay(500);
  Serial.println("Setting 115200.");
  Serial1.begin(115200);  // 4800,9600,14400,19200,38400,57600,115200,230400,460800,921600
  Serial1.println("$PMTK250,3,3,115200*1E");
  Serial1.println("$PMTK253,0*2A");
  Serial1.println(PMTK_SET_BAUD_115200);
  delay(500);
  Serial1.end();
  delay(500);
  Serial.println("Setting 230400.");
  Serial1.begin(230400);  // 4800,9600,14400,19200,38400,57600,115200,230400,460800,921600
  Serial1.println("$PMTK250,3,3,115200*1E");
  Serial1.println("$PMTK253,0*2A");
  Serial1.println(PMTK_SET_BAUD_115200);
  delay(500);
  Serial1.end();
  delay(500);
  Serial.println("Setting 460800.");
  Serial1.begin(460800);  // 4800,9600,14400,19200,38400,57600,115200,230400,460800,921600
  Serial1.println("$PMTK250,3,3,115200*1E");
  Serial1.println("$PMTK253,0*2A");
  Serial1.println(PMTK_SET_BAUD_115200);
  delay(500);
  Serial1.end();
  delay(500);
  Serial.println("Setting 921600.");
  Serial1.begin(921600);  // 4800,9600,14400,19200,38400,57600,115200,230400,460800,921600
  Serial1.println("$PMTK250,3,3,115200*1E");
  Serial1.println("$PMTK253,0*2A");
  Serial1.println(PMTK_SET_BAUD_115200);
  delay(500);
  Serial1.end();
  delay(500);
}