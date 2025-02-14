#include <SoftwareSerial.h>

#define GSM_TX_PIN 19
#define GSM_RX_PIN 18

#define GPS_TX_PIN 23
#define GPS_RX_PIN 22

SoftwareSerial gsmSerial(GSM_TX_PIN, GSM_RX_PIN);
SoftwareSerial gpsSerial(GPS_TX_PIN, GPS_RX_PIN);

void setup() {
  Serial.begin(9600);
  gsmSerial.begin(9600);
  gpsSerial.begin(9600);
}

void loop() {
  if (Serial.available()) {
    gsmSerial.write(Serial.read());
  }
  if (gsmSerial.available()) {
    Serial.write(gsmSerial.read());
  }
//  if (gpsSerial.available()) {
//    Serial.write(gpsSerial.read());
//  }
}
