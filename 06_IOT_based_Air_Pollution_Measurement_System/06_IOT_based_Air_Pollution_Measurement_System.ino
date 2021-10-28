/*********************************************************************************
   If this code works, it was written by Jahidul Islam Rahat.
   If not, I don't know who wrote it.
   :) xD

   Author: Jahidul Islam Rahat.
   Date: 7 Jan 2021.
*********************************************************************************/
/*
   Example code for IoT Snacksbox and School of IoT.
   IoT Dashboard: https://iotsnacksbox.io/ - (IoT Snacksbox is an IoT server.
   where you can use it for factory monitoring, Appliances Control,
   Data Visualization, Smart Production Planning, Data Backup, Access Control,
   Security monitoring and Inter Machine Communication.
   School of IoT:
   ***You can use it free for your school project.***
*/

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Replace with your network credentials
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

/*
   Read api&json documantation here for IoT Snacksbox:
   Replace with your Token.
   const char* SmokePost_serverName  = "https://api.iotsnacksbox.io/trigger/Smoke?snacksboxtoken=" ;
*/
const char* SmokePost_serverName  = "https://api.iotsnacksbox.io/trigger/Smoke?snacksboxtoken=f3b0d5a73b6a68dbf0199c7d10f103e8d52f0ffcb65573af478650ad34093be0" ;

int Buzzer = 25;
int Smoke_sensor = 39;
int Smoke;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  pinMode(Buzzer, OUTPUT);
  pinMode(Smoke_sensor, INPUT);
  digitalWrite(Buzzer, LOW);
}


void Smoke_Read() {
  Smoke = analogRead(Smoke_sensor) / 45;
  Serial.print("Smoke: ");
  Serial.print(Smoke);
  Serial.println("%");
}


void loop() {
  Smoke_Read();
  if (Smoke <= 30) {
    digitalWrite(Buzzer, LOW);
    Serial.println("Buzzer OFF");
  }
  else {
    digitalWrite(Buzzer, HIGH);
    Serial.println("Buzzer ON");
  }

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(SmokePost_serverName);

    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.addHeader("Content-Type", "application/json");
    char buffer[32];
    DynamicJsonDocument doc1(32);
    JsonObject data = doc1.createNestedObject("data");

    data["Smoke"] = Smoke;

    serializeJson(doc1, buffer);
    Serial.print(buffer);

    String httpRequestData = buffer;
    int httpResponseCode = http.POST(httpRequestData);
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    http.end();
    /*
       If you find httpResponseCode: 200, it means your request was successful.
       And if you find any error code, then search on google and solve it.
       If you don't know how to solve it, then give a knock to the support.
    */
  }
  else {
    Serial.println("WiFi Disconnected");
  }
}
