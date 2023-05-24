#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

#define RX_PIN 4
#define TX_PIN 5

const char* ssid = "JaxTeller";
const char* password = "Cbg2329@";

const char* url = "https://f1racing.onrender.com/write";

SoftwareSerial gpsSerial(RX_PIN, TX_PIN);

TinyGPSPlus gps;

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }
  if (gps.location.isValid() && gps.time.isValid()) {
    String payload = "{\"f1CarNo\": 121, \"latitude\": " + String(gps.location.lat(), 6) + ", \"longitude\": " + String(gps.altitude.meters()) + ", \"speed\": " + String(gps.speed.kmph()) + ", \"time\": \"" + String(gps.time.hour()) + ":
    " + String(gps.time.minute()) + ":" + String(gps.time.second()) + "\"}";

    Serial.println("GPS Data:");
    Serial.println("Latitude: " + String(gps.location.lat(), 6));
    Serial.println("Longitude: " + String(gps.location.lng(), 6));
    Serial.println("Speed: " + String(gps.speed.kmph()) + "km/h");
    Serial.println("Time: " + String(gps.time.hour()) + ":" + String(gps.time.minute()) + ":" + String(gps.time.second()));

    HTTPClient http;
    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
      Serial.printf("HTTP POST request successful, response code: %d\n", httpResponseCode);
      String response = http.getString();
      Serial.println("Response: " + response);
    } else {
      Serial.printf("HTTP POST request failed, error code: %d\n", httpResponseCode);
    }

    http.end();

    delay(10000);
  }
}
