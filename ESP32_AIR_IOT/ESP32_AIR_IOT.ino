#include <DHT.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

// Conversion factors (adjust these based on sensor calibration)
const float NH3_CONVERSION_FACTOR = 0.65; // Adjust as needed
const float CO2_CONVERSION_FACTOR = 0.65; // Adjust as needed
const float CO_CONVERSION_FACTOR = 1.0;  // Adjust as needed

#define DHTPIN 27
#define DHTTYPE DHT11

#define MQ7_PIN 39
#define MQ135_NH3_PIN 32
#define MQ135_CO2_PIN 34

#define DUST_PIN 36
#define LED_PIN 13

#define GSM_TX_PIN 19
#define GSM_RX_PIN 18

#define GPS_TX_PIN 23
#define GPS_RX_PIN 22

DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial gsmSerial(GSM_TX_PIN, GSM_RX_PIN);
SoftwareSerial gpsSerial(GPS_TX_PIN, GPS_RX_PIN);
TinyGPSPlus gps;

unsigned long previousReadingTime = 0;
unsigned long previousGsmTime = 0;

const unsigned long readingInterval = 1000;
const unsigned long gsmInterval = 30000;

float temperature, humidity;
float latitude, longitude;
float coValue, co2Value, nh3Value;
float pm25Value;

void setup() {
  Serial.begin(115200);
  gsmSerial.begin(9600);
  gpsSerial.begin(9600);

  pinMode(LED_PIN, OUTPUT);
  analogReadResolution(10);

  initGPRS();
}

void loop() {
  checkGPS();
  unsigned long currentMillis = millis();

  if (currentMillis - previousReadingTime >= readingInterval) {
    previousReadingTime = currentMillis;

    float t = dht.readTemperature();
    if (!isnan(t)) temperature = t;
    float h = dht.readHumidity();
    if (!isnan(h)) humidity = h;

    if (temperature < 20) {
      temperature = abs(temperature * 2);
      humidity = abs(humidity / 2);
    }

    int mq7Value = analogRead(MQ7_PIN);
    int mq135Value1 = analogRead(MQ135_NH3_PIN);
    int mq135Value2 = analogRead(MQ135_CO2_PIN);

    float voltageNH3 = (float)mq135Value1 * (3.3 / 1023.0);
    nh3Value = voltageNH3 * NH3_CONVERSION_FACTOR;

    float voltageCO2 = (float)mq135Value2 * (3.3 / 1023.0);
    co2Value = voltageCO2 * CO2_CONVERSION_FACTOR;

    float voltageCO = (float)mq7Value * (3.3 / 1023.0);
    coValue = voltageCO * CO_CONVERSION_FACTOR;

    // read PM2.5 reading
    digitalWrite(LED_PIN, LOW); // power on the LED
    delayMicroseconds(280); // Wait 0.28ms according to DS
    int pmValue = analogRead(DUST_PIN); // take analog reading
    delay(1);
    digitalWrite(LED_PIN, HIGH); // turn the LED off
    float pmVolt = pmValue * (3.3 / 1023.0);
    pm25Value = abs(0.18 * pmVolt);

    printLog();
  }

  if (currentMillis - previousGsmTime >= gsmInterval) {
    previousGsmTime = currentMillis;
    sendServer();
  }
}

void checkGPS() {
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      if (gps.location.isValid()) {
        latitude = gps.location.lat();
        longitude = gps.location.lng();
      }
    }
  }
}

void printLog() {
  Serial.print((String)temperature + " | ");
  Serial.print((String)humidity + " | ");
  Serial.print((String)nh3Value + " | ");
  Serial.print((String)co2Value + " | ");
  Serial.print((String)coValue + " | ");
  Serial.print((String)pm25Value + " | ");
  Serial.print((String)latitude + " | ");
  Serial.println((String)longitude + " | ");
}

void initGPRS() {
  // Wait 15sec for SIM connected
  for (int i = 0; i < 150; i++) {
    delay(100);
  }
  gsmSerial.print(F("AT\r\n"));
  delay(1000);
  gsmSerial.print(F("ATE0\r\n"));
  delay(1000);
  gsmSerial.println(F("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\""));
  delay(1000);
  gsmSerial.println(F("AT+SAPBR=3,1,\"APN\",\"wap\""));
  delay(1000);
  gsmSerial.println(F("AT+CGATT=1"));
  delay(1000);
  gsmSerial.println(F("AT+SAPBR=1,1"));
  // Wait 6sec for GPRS connected
  for (int i = 0; i < 60; i++) {
    delay(100);
  }
  gsmSerial.println(F("AT+HTTPINIT"));
  delay(1000);
}

void sendServer() {
  // Construct your HTTP GET request here using the sensor values
  String httpRequest = "AT+HTTPPARA=\"URL\",\"http://esinebd.com/projects/AirQuality/update_machine.php?";
  httpRequest += "v1=" + String(temperature);
  httpRequest += "&v2=" + String(humidity);
  httpRequest += "&v3=" + String(nh3Value);
  httpRequest += "&v4=" + String(co2Value);
  httpRequest += "&v5=" + String(coValue);
  httpRequest += "&v6=" + String(pm25Value);
  httpRequest += "&v7=" + String(latitude, 6);
  httpRequest += "&v8=" + String(longitude, 7);
  httpRequest += "&map\"";
  Serial.println(httpRequest);
  gsmSerial.println(httpRequest);
  delay(1000);
  gsmSerial.println(F("AT+HTTPACTION=0"));
  // Wait 6sec for GET push
  for (int i = 0; i < 30; i++) {
    delay(100);
  }
}
