#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "House of R&D";
const char* password = "12345HouseofR&D";

const char* ActionGet_serverName  = "https://api.iotsnacksbox.io/actions?snacksboxtoken=e481b9d4f2d40524be6bd71f5a92f84b578eade2fd2115388427f023b14e1404" ;
const char* ActionPost_serverName = "https://api.iotsnacksbox.io/actions?snacksboxtoken=e481b9d4f2d40524be6bd71f5a92f84b578eade2fd2115388427f023b14e1404" ;
const char* LightPost_serverName  = "https://api.iotsnacksbox.io/trigger/Light?snacksboxtoken=e481b9d4f2d40524be6bd71f5a92f84b578eade2fd2115388427f023b14e1404" ;

String payload ;

int AutoMode_value;
int AutoMode_state;
int ManualMode_state;
int ManualMode_value;
int Led = 26;
int Led_state;
int V_Led_value;
int Light_sensor = 39;
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

  pinMode(Led, OUTPUT);
  pinMode(Light_sensor, INPUT);
  digitalWrite(Led, HIGH);



}

void Json_Parsing() {
  // Parsing the JSON using https://arduinojson.org/v6/assistant/
  Serial.print("Payload/Json from Server:");
  Serial.println(payload);

  DynamicJsonDocument doc(768);
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
  V_Led_value = root_2["value"]; // "0"
  Serial.print("Component Name: ");
  Serial.print(root_2_name);
  Serial.print(" : ");
  Serial.println(V_Led_value);


}
void Light_Read() {
  Light = analogRead(Light_sensor) / 45;
  Serial.print("Light: ");
  Serial.print(Light);
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
      Light_Read();
      if (Light >= 30) {
        digitalWrite(Led,HIGH );
        Led_state = 0;
        Serial.println("Led OFF");
      }
      else if (Light <= 31) {
        digitalWrite(Led, LOW);
        Led_state = 1;
        Serial.println("Led ON");
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

      if (V_Led_value == 1) {
        digitalWrite(Led, HIGH);
        Led_state = 1;
        Serial.println("Led ON");
      }
      else if (V_Led_value == 0) {
        digitalWrite(Led, LOW);
        Led_state = 0;
        Serial.println("Led OFF");
      }

    }
    else if ((AutoMode_value == 0) && (ManualMode_value == 0)) {
      digitalWrite(Led, LOW);
      Led_state = 0;
      AutoMode_state = 0;
      ManualMode_state = 0;
      Serial.println("Led OFF");
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
  http.begin(LightPost_serverName);

  // Specify content-type header
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Content-Type", "application/json");
  buffer[32];
  DynamicJsonDocument doc1(32);
  JsonObject data = doc1.createNestedObject("data");
  Light_Read();
  data["Light"] = Light;

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
