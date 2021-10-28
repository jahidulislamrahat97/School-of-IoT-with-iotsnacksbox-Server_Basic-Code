#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "Digital_Inquibator_5";
const char* password = "BL@DGAP5";

const char* serverName = "https://api.iotsnacksbox.io/actions?snacksboxtoken=790414b9aa226289834a2cd02e90ad36cb9489580409e9405f16b010af2402dd";

String payload ;

int Relay1 = 26;
int Relay2 = 27;
int Relay1_value;
int Relay2_value;


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

  pinMode(Relay1, OUTPUT);
  pinMode(Relay2, OUTPUT);

  digitalWrite(Relay1, HIGH);
  digitalWrite(Relay2, HIGH);

}


void Json_Parsing() {
  // Parsing the JSON using https://arduinojson.org/v6/assistant/
  Serial.print("Payload/Json from Server:");
  Serial.println(payload);

  DynamicJsonDocument doc(768);
  deserializeJson(doc, payload);

  JsonObject root_0 = doc[0];
  String root_0_name = root_0["name"]; // "Relay1"
  Relay1_value = root_0["value"]; // "0"
  Serial.print("Component Name: ");
  Serial.print(root_0_name);
  Serial.print(" : ");
  Serial.println(Relay1_value);

  JsonObject root_1 = doc[1];
  String root_1_name = root_1["name"]; // "Relay2"
  Relay2_value = root_1["value"]; // "0"
  Serial.print("Component Name: ");
  Serial.print(root_1_name);
  Serial.print(" : ");
  Serial.println(Relay2_value);

}


void loop() {

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    // Your Domain name with URL path or IP address with path
    http.begin(serverName);

    // Send HTTP GET request
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      payload = http.getString();
      Serial.println(payload);
      Json_Parsing();

      if (Relay1_value == 0) {
        digitalWrite(Relay1, HIGH);
      }
      else if (Relay1_value == 1) {
        digitalWrite(Relay1, LOW);
      }
      if (Relay2_value == 0) {
        digitalWrite(Relay2, HIGH);
      }
      else if (Relay2_value == 1) {
        digitalWrite(Relay2, LOW);
      }
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }


}
