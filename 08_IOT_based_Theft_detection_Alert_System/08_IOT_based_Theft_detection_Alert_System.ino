#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "House of R&D";
const char* password = "12345HouseofR&D";

const char* ActionGet_serverName  = "https://api.iotsnacksbox.io/actions?snacksboxtoken=101ab2c68a8315ea7d2af758fbb550821ec3e732af63773d8d26b76162504fd2" ;
const char* ActionPost_serverName = "https://api.iotsnacksbox.io/actions?snacksboxtoken=101ab2c68a8315ea7d2af758fbb550821ec3e732af63773d8d26b76162504fd2" ;
const char* PIRPost_serverName  = "https://api.iotsnacksbox.io/trigger/PIRvalue?snacksboxtoken=101ab2c68a8315ea7d2af758fbb550821ec3e732af63773d8d26b76162504fd2" ;

String payload ;

int AutoMode_value;
int AutoMode_state;
int Buzzer = 25;
int Buzzer_state;
int V_Buzzer_value;
int PIR_sensor = 13;
int PIR_value;

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
  pinMode(PIR_sensor, INPUT);
  digitalWrite(Buzzer, LOW);



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
  V_Buzzer_value = root_1["value"]; // "0"
  Serial.print("Component Name: ");
  Serial.print(root_1_name);
  Serial.print(" : ");
  Serial.println(V_Buzzer_value);



}
void PIR_Read() {
  PIR_value = digitalRead(PIR_sensor);
  Serial.print("PIR_value: ");
  Serial.println(PIR_value);
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
    if (AutoMode_value == 1) {
      PIR_Read();
      if (PIR_value == 0) {
        digitalWrite(Buzzer, LOW);
        Buzzer_state = 0;
        Serial.println("Buzzer OFF");
      }
      else if (PIR_value == 1) {
        digitalWrite(Buzzer, HIGH);
        Buzzer_state = 1;
        Serial.println("Buzzer ON");
      }
      AutoMode_state = 1;
      Serial.println("AutoMode ON");
    }
    else if (AutoMode_value == 0)  {

      AutoMode_state = 0;
      Serial.println("AutoMode OFF");

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

  char buffer[192];
  DynamicJsonDocument doc(192);

  JsonArray actions = doc.createNestedArray("actions");

  JsonObject actions_0 = actions.createNestedObject();
  actions_0["name"] = "AutoMode";
  actions_0["value"] = AutoMode_state;

  JsonObject actions_1 = actions.createNestedObject();
  actions_1["name"] = "Buzzer";
  actions_1["value"] = Buzzer_state;



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

  
  http.begin(PIRPost_serverName);
  // Specify content-type header
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Content-Type", "application/json");
  
  buffer[32];
  DynamicJsonDocument doc1(32);
  JsonObject data = doc1.createNestedObject("data");
  PIR_Read();
  data["PIR"] = PIR_value;
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
