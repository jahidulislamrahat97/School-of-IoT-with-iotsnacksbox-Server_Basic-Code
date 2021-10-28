#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "DHT.h"

const char* ssid = "Offline";
const char* password = "09876543210";

const char* TemperaturePost_serverName   = "https://api.iotsnacksbox.io/trigger/Temperature?snacksboxtoken=cb8ed06679990d43f70fa805130ddefbcfd30a054d525192207c6d04b91b406d" ;
const char* HumidityPost_serverName      = "https://api.iotsnacksbox.io/trigger/Humidity?snacksboxtoken=cb8ed06679990d43f70fa805130ddefbcfd30a054d525192207c6d04b91b406d" ;
const char* SmokePost_serverName         = "https://api.iotsnacksbox.io/trigger/Smoke?snacksboxtoken=cb8ed06679990d43f70fa805130ddefbcfd30a054d525192207c6d04b91b406d" ;
const char* LightPost_serverName         = "https://api.iotsnacksbox.io/trigger/Light?snacksboxtoken=cb8ed06679990d43f70fa805130ddefbcfd30a054d525192207c6d04b91b406d" ;


#define DHTPIN 15
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

int Smoke_Sensor = 39;
int Light_Sensor = 34;

int Temperature;
int Humidity;
int Smoke;
int Light;


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
  dht.begin();
  pinMode(Smoke_Sensor, INPUT);
  pinMode(Light_Sensor, INPUT);



}
int Temperature_Read() {
  Temperature = dht.readTemperature();
  Serial.print("Room Temperature: ");
  Serial.print(Temperature);
  Serial.println("°C");
  return Temperature;
}
int Humidity_Read() {
  Humidity = dht.readHumidity();
  Serial.print("Room Humidity: ");
  Serial.print(Humidity);
  Serial.println("%");
  return Humidity;
}

int Smoke_Read() {
  Smoke = analogRead(Smoke_Sensor) / 45;
  Serial.print("Smoke: ");
  Serial.print(Smoke);
  Serial.println("%");
  return Smoke;
}
int Light_Read() {
  Light = analogRead(Light_Sensor) / 45;
  Serial.print("Light: ");
  Serial.print(Light);
  Serial.println("%");
  return Light;
}

void Control_Server_From_Device() {
  HTTPClient http;
  http.begin(TemperaturePost_serverName);

  // Specify content-type header
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Content-Type", "application/json");

  char buffer[32];
  DynamicJsonDocument doc0(32);
  JsonObject data = doc0.createNestedObject("data");
  Temperature_Read();
  data["Temperature"] = Temperature;
  serializeJson(doc0, buffer);
  Serial.print(buffer);

  String httpRequestData = buffer;
  int httpResponseCode = http.POST(httpRequestData);

  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);
  http.end();



  http.begin(HumidityPost_serverName);

  // Specify content-type header
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Content-Type", "application/json");

  buffer[32];
  DynamicJsonDocument doc1(32);
  data = doc1.createNestedObject("data");
  Humidity_Read();
  data["Humidity"] = Humidity;
  serializeJson(doc1, buffer);
  Serial.print(buffer);

  httpRequestData = buffer;
  httpResponseCode = http.POST(httpRequestData);

  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);
  http.end();

  http.begin(SmokePost_serverName);

  // Specify content-type header
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Content-Type", "application/json");

  buffer[32];
  DynamicJsonDocument doc2(32);
  data = doc2.createNestedObject("data");
  Smoke_Read();
  data["Smoke"] = Smoke;
  serializeJson(doc2, buffer);
  Serial.print(buffer);

  httpRequestData = buffer;
  httpResponseCode = http.POST(httpRequestData);

  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);
  http.end();



  http.begin(LightPost_serverName);

  // Specify content-type header
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Content-Type", "application/json");

  buffer[32];
  DynamicJsonDocument doc3(32);
  data = doc3.createNestedObject("data");
  Light_Read();
  data["Light"] = Light;
  serializeJson(doc3, buffer);
  Serial.print(buffer);

  httpRequestData = buffer;
  httpResponseCode = http.POST(httpRequestData);

  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);
  http.end();






}



void loop() {

  if (WiFi.status() == WL_CONNECTED) {
    Control_Server_From_Device();
  }
  else {
    Serial.println("WiFi Disconnected");
  }


}
