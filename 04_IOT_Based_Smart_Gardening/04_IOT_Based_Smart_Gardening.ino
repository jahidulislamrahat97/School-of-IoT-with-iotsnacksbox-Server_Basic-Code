#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "House of R&D";
const char* password = "12345HouseofR&D";

const char* ActionGet_serverName  = "https://api.iotsnacksbox.io/actions?snacksboxtoken=4ad1a93aa9cabdb95fb085a9a1a4ee2acb2b1317b10239f61269fd80a872ea2e" ;
const char* ActionPost_serverName = "https://api.iotsnacksbox.io/actions?snacksboxtoken=4ad1a93aa9cabdb95fb085a9a1a4ee2acb2b1317b10239f61269fd80a872ea2e" ;
const char* MoisturePost_serverName   = "https://api.iotsnacksbox.io/trigger/Moisture?snacksboxtoken=4ad1a93aa9cabdb95fb085a9a1a4ee2acb2b1317b10239f61269fd80a872ea2e" ;

String payload ;

int AutoMode_value;
int ManualMode_value;
int AutoMode_state;
int ManualMode_state;
int Pump = 27;
int Pump_state;
int V_Pump_value;
int Moisture_sensor = 36;
int Moisture;

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

  pinMode(Pump, OUTPUT);
  pinMode(Moisture_sensor, INPUT);
  digitalWrite(Pump, HIGH);



}

void Json_Parsing() {
  // Parsing the JSON using https://arduinojson.org/v6/assistant/
  Serial.print("Payload/Json from Server:");
  Serial.println(payload);

  DynamicJsonDocument doc(1024);
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
  ManualMode_value = root_1["value"]; // "0"
  Serial.print("Component Name: ");
  Serial.print(root_1_name);
  Serial.print(" : ");
  Serial.println(ManualMode_value);


  JsonObject root_2 = doc[2];
  String root_2_name = root_2["name"]; // "Relay2"
  V_Pump_value = root_2["value"]; // "0"
  Serial.print("Component Name: ");
  Serial.print(root_2_name);
  Serial.print(" : ");
  Serial.println(V_Pump_value);


}
void Moisture_Read() {
  Moisture = analogRead(Moisture_sensor) / 45;
  Serial.print("Moisture: ");
  Serial.print(Moisture);
  Serial.println("%");
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
    if ((AutoMode_value == 1) && (ManualMode_value == 0)) {
      Moisture_Read();
      if (Moisture <= 30) {
        digitalWrite(Pump, HIGH);
        Pump_state = 0;
        Serial.println("Pump OFF");

      }
     
      else if (Moisture >= 31) {
        digitalWrite(Pump, LOW);
        Pump_state = 1;
        Serial.println("Pump ON");
      }
      AutoMode_state = 1;
      ManualMode_state = 0;
      Serial.println("AutoMode ON");
      Serial.println("ManualMode OFF");
    }
    else if ((AutoMode_value == 0) && (ManualMode_value == 1)) {

      AutoMode_state = 0;
      ManualMode_state = 1;
      Serial.println("AutoMode OFF");
      Serial.println("ManualMode ON");

      if (V_Pump_value == 1) {
        digitalWrite(Pump, LOW);
        Pump_state = 1;
        Serial.println("Pump ON");
      }
      else if (V_Pump_value == 0) {
        digitalWrite(Pump, HIGH);
        Pump_state = 0;
        Serial.println("Pump OFF");
      }

    }
    else if ((AutoMode_value == 0) && (ManualMode_value == 0)) {
      digitalWrite(Pump, HIGH);
      Pump_state = 0;
      AutoMode_state = 0;
      ManualMode_state = 0;
      Serial.println("Pump OFF");
      Serial.println("AutoMode OFF");
      Serial.println("ManualMode OFF");
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
  actions_1["name"] = "ManualMode";
  actions_1["value"] = ManualMode_state;

  JsonObject actions_2 = actions.createNestedObject();
  actions_2["name"] = "Pump";
  actions_2["value"] = Pump_state;


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
  http.begin(MoisturePost_serverName);

  // Specify content-type header
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Content-Type", "application/json");
  buffer[32];
  DynamicJsonDocument doc1(32);
  JsonObject data = doc1.createNestedObject("data");
  Moisture_Read();
  data["Moisture"] = Moisture;

  serializeJson(doc1, buffer);
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
