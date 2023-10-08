#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>
#include <WiFi.h>

const char* url = "https://bokee.vercel.app/mqtt";  // URL ของ API

const char* ssid = "ใส่ชื่อไวไฟ";
const char* password = "ใส่รหัสไวไฟ";

void setup() {
  pinMode(13, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(14, INPUT);
  pinMode(27, INPUT);
  pinMode(26, INPUT);
  pinMode(25, INPUT);
  pinMode(33, INPUT);
  pinMode(32, INPUT);
  digitalWrite(13, 1);
  digitalWrite(2, 0);
  Serial.begin(115200);
  while (!Serial) { delay(100); }
  WiFiManager wm;
  Serial.println();
  Serial.println("******************************************************");
  Serial.print("Connecting to ");
  Serial.println(ssid);

  bool res;
  res = wm.autoConnect(ssid, "password");

  if (!res) {
    Serial.println("Failed to connect");
    // ESP.restart();
  } else {
    digitalWrite(2, 1);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(2, 1);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Fetching data...");
    HTTPClient http;

    http.begin(url);

    int httpCode = http.GET();

    if (httpCode == 200) {
      String payload = http.getString();
      DynamicJsonDocument doc(1024);  // ประมาณขนาดของ JSON

      DeserializationError error = deserializeJson(doc, payload);

      if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
      }

      const char* data = doc["data"];

      if (strcmp(data, "on") == 0) {
        Serial.println("on");
        on();
      } else if (strcmp(data, "off") == 0) {
        Serial.println("off");
        off();
      } else if (strcmp(data, "auto") == 0) {
        Serial.println("auto");
        autoMode();
      } else {
        Serial.println("Invalid data received");
      }
    } else {
      Serial.print("HTTP request failed with error code: ");
      Serial.println(httpCode);
    }

    http.end();
  } else {
    ESP.restart();
  }

  delay(1000);  // รอ 1 วินาที ก่อนที่จะทำการ request ใหม่
}

void on() {
  digitalWrite(13, 0);
}

void off() {
  digitalWrite(13, 1);
}

void autoMode() {
  if (digitalRead(14) == 0 && digitalRead(27) == 0 && digitalRead(26) == 0 && digitalRead(25) == 0 && digitalRead(33) == 0 && digitalRead(32) == 0) {
    off();
  } else {
    on();
  }
}
