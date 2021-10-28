#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "DHT.h"

const char* ssid = "House of R&D";
const char* password = "12345HouseofR&D";

const char* ActionGet_serverName        = "https://api.iotsnacksbox.io/actions?snacksboxtoken=1434169c4e29be4672ffe6406a06fa68a99b15a95ff4b43ca2aaaa2ada3746b2" ;
const char* ActionPost_serverName       = "https://api.iotsnacksbox.io/actions?snacksboxtoken=1434169c4e29be4672ffe6406a06fa68a99b15a95ff4b43ca2aaaa2ada3746b2" ;
const char* TemperaturePost_serverName  = "https://api.iotsnacksbox.io/trigger/Temperature?snacksboxtoken=1434169c4e29be4672ffe6406a06fa68a99b15a95ff4b43ca2aaaa2ada3746b2" ;
const char* SmokePost_serverName        = "https://api.iotsnacksbox.io/trigger/Gas?snacksboxtoken=1434169c4e29be4672ffe6406a06fa68a99b15a95ff4b43ca2aaaa2ada3746b2" ;
const char* LightPost_serverName        = "https://api.iotsnacksbox.io/trigger/Light?snacksboxtoken=1434169c4e29be4672ffe6406a06fa68a99b15a95ff4b43ca2aaaa2ada3746b2" ;

String payload ;

#define DHTPIN 14
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

int AutoMode_value;
int AutoMode_state;

int Led = 26;
int Fan = 27;
int Buzzer = 25;
int Smoke_Sensor = 39;
int Light_Sensor = 34;

int Led_state;
int Buzzer_state;
int Fan_state;

int V_Led_value;
int V_Buzzer_value;
int V_Fan_value;

int Temperature;
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

  pinMode(Led, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  pinMode(Fan, OUTPUT);
  pinMode(Smoke_Sensor, INPUT);
  pinMode(Light_Sensor, INPUT);

  digitalWrite(Led, HIGH);
  digitalWrite(Buzzer, LOW);
  digitalWrite(Fan, HIGH);

}

void Json_Parsing() {
  // Parsing the JSON using https://arduinojson.org/v6/assistant/
  Serial.print("Payload/Json from Server:");
  Serial.println(payload);

  DynamicJsonDocument doc(1536);
  deserializeJson(doc, payload);

  JsonObject root_0 = doc[0];
  String root_0_name = root_0["name"]; // "Relay1"
  AutoMode_value = root_0["value"]; // "0"
  Serial.print("Component Name: ");
  Serial.print(root_0_name);
  Serial.print(" : ");
  Serial.println(AutoMode_value);




  JsonObject root_1 = doc[1];
  String root_1_name = root_1["name"]; // "Relay2"
  V_Led_value = root_1["value"]; // "0"
  Serial.print("Component Name: ");
  Serial.print(root_1_name);
  Serial.print(" : ");
  Serial.println(V_Led_value);

  JsonObject root_2 = doc[2];
  String root_2_name = root_2["name"]; // "Relay2"
  V_Fan_value = root_2["value"]; // "0"
  Serial.print("Component Name: ");
  Serial.print(root_2_name);
  Serial.print(" : ");
  Serial.println(V_Fan_value);

  JsonObject root_3 = doc[3];
  String root_3_name = root_3["name"]; // "Relay2"
  V_Buzzer_value = root_3["value"]; // "0"
  Serial.print("Component Name: ");
  Serial.print(root_3_name);
  Serial.print(" : ");
  Serial.println(V_Buzzer_value);


}
int Temperature_Read() {
  Temperature = dht.readTemperature();
  Serial.print("Room Temperature: ");
  Serial.print(Temperature);
  Serial.println("°C");
  return Temperature;
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


void Control_Device_From_Server() {
  HTTPClient http;
  // Your Domain name with URL path or IP address with path
  http.begin(ActionGet_serverName);
  // Send HTTP GET request
  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
    Serial.println(payload);
    Json_Parsing();
    if ((AutoMode_value == 1)) {
      Light_Read();
      Temperature_Read();
      Smoke_Read();


      if (Light >= 30) {
        digitalWrite(Led, HIGH );
        Led_state = 0;
        Serial.println("Led OFF");
      }
      else if (Light < 31) {
        digitalWrite(Led, LOW);
        Led_state = 1;
        Serial.println("Led ON");
      }

      if (Temperature <= 26) {
        digitalWrite(Fan, HIGH );
        Fan_state = 0;
        Serial.println("Fan OFF");
      }
      else if (Temperature >26) {
        digitalWrite(Fan, LOW);
        Fan_state = 1;
        Serial.println("Fan ON");
      }

      if (Smoke <= 18) {
        digitalWrite(Buzzer, LOW );
        Buzzer_state = 0;
        Serial.println("Buzzer OFF");
      }
      else if (Smoke >18) {
        digitalWrite(Buzzer, HIGH);
        Buzzer_state = 1;
        Serial.println("Buzzer ON");
      }

      
      AutoMode_state = 1;
      Serial.println("AutoMode ON");

    }
    else if ((AutoMode_value == 0)) {

      AutoMode_state = 0;

      Serial.println("AutoMode OFF");
      Serial.println("ManualMode ON");

      if (V_Led_value == 1) {
        digitalWrite(Led, LOW);
        Led_state = 1;
        Serial.println("Led ON");
      }
      else if (V_Led_value == 0) {
        digitalWrite(Led, HIGH);
        Led_state = 0;
        Serial.println("Led OFF");
      }


      if (V_Buzzer_value == 1) {
        digitalWrite(Buzzer, HIGH);
        Buzzer_state = 1;
        Serial.println("Buzzer ON");
      }
      else if (V_Buzzer_value == 0) {
        digitalWrite(Buzzer, LOW);
        Buzzer_state = 0;
        Serial.println("Buzzer OFF");
      }

      if (V_Fan_value == 1) {
        digitalWrite(Fan, LOW);
        Fan_state = 1;
        Serial.println("Fan ON");
      }
      else if (V_Fan_value == 0) {
        digitalWrite(Fan, HIGH);
        Fan_state = 0;
        Serial.println("Fan OFF");
      }

    }
    
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();
}

void Control_Server_From_Device() {
  HTTPClient http;
  http.begin(ActionPost_serverName);

  char buffer[256];
  DynamicJsonDocument doc(256);

  JsonArray actions = doc.createNestedArray("actions");
  JsonObject actions_0 = actions.createNestedObject();
  actions_0["name"] = "AutoMode";
  actions_0["value"] = AutoMode_state;

  JsonObject actions_1 = actions.createNestedObject();
  actions_1["name"] = "Fan";
  actions_1["value"] = Fan_state;

  JsonObject actions_2 = actions.createNestedObject();
  actions_2["name"] = "Led";
  actions_2["value"] = Led_state;


  serializeJson(doc, buffer);
  Serial.println(String(buffer));

  // Specify content-type header
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Content-Type", "application/json");
  String httpRequestData = buffer;
  int httpResponseCode = http.POST(httpRequestData);
  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);
  http.end();


  http.begin(TemperaturePost_serverName);

  // Specify content-type header
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Content-Type", "application/json");

  buffer[32];
  DynamicJsonDocument doc0(32);
  JsonObject data = doc0.createNestedObject("data");
  Temperature_Read();
  data["Temperature"] = Temperature;
  serializeJson(doc0, buffer);
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
    Control_Device_From_Server();
    Control_Server_From_Device();
  }
  else {
    Serial.println("WiFi Disconnected");
  }


}
