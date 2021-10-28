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
#include "DHT.h"

// Replace with your network credentials
const char* ssid = "House of R&D"; //"REPLACE_WITH_YOUR_SSID";
const char* password = "12345HouseofR&D";  //"REPLACE_WITH_YOUR_PASSWORD";

/*
   Read api&json documantation here for IoT Snacksbox:
   Replace with your Token.
   const char* ActionGet_serverName  = "https://api.iotsnacksbox.io/actions?snacksboxtoken=abcdefghijklmnopqrstuvwxyz1234567890" ;
   const char* ActionPost_serverName = "https://api.iotsnacksbox.io/actions?snacksboxtoken=abcdefghijklmnopqrstuvwxyz1234567890" ;
   const char* TempPost_serverName   = "https://api.iotsnacksbox.io/trigger/Temperature?snacksboxtoken=abcdefghijklmnopqrstuvwxyz1234567890" ;
*/

const char* ActionGet_serverName  = "https://api.iotsnacksbox.io/actions?snacksboxtoken=cef733ed717306da81656a4d90b1922f1c402f3a281a08e733818aa9f1a7145d" ;
const char* ActionPost_serverName = "https://api.iotsnacksbox.io/actions?snacksboxtoken=cef733ed717306da81656a4d90b1922f1c402f3a281a08e733818aa9f1a7145d" ;
const char* TempPost_serverName   = "https://api.iotsnacksbox.io/trigger/Temperature?snacksboxtoken=cef733ed717306da81656a4d90b1922f1c402f3a281a08e733818aa9f1a7145d" ;



String payload ;

int AutoMode_value;
int ManualMode_value;
int AutoMode_state;
int ManualMode_state;
int Fan = 26;
int Fan_state;
int V_Fan_value;

#define DHTPIN 15
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
int Temperature;

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
  pinMode(Fan, OUTPUT);
  digitalWrite(Fan, HIGH);

}

void Json_Parsing() {
  /*
     Read api&json documantation here for IoT Snacksbox:
     Parsing your json using arduino json assistant: https://arduinojson.org/v6/assistant/
     Here we just receive json from get request to the server.
     And extracting and prepare them for our personal use.
  */
  Serial.print("Payload/Json from Server:");
  Serial.println(payload);

  //Here doc(768)is json size(how much memory need for extracting this json).
  DynamicJsonDocument doc(768);
  deserializeJson(doc, payload);

  /* you will find this type of json for 1 Switch/Action:
     {
     "values":[0,1],
     "_id":"5ff5878f2d14472596c94a10",
     "name":"AutoMode",
     "type":"Boolean",
     "value":0,
     "createdAt":"2021-01-06T09:49:03.947Z",
     "updatedAt":"2021-01-06T09:59:37.450Z"
     }
     You don't need all of these values.
     you just need the value of the switch which you are using for IoT Server.
     I just kept ***String root_0_name = root_0["name"];*** this line for your better understanding.

  */

  JsonObject root_0 = doc[0];
  String root_0_name = root_0["name"]; // "AutoMode"
  AutoMode_value = root_0["value"]; // "0"
  Serial.print("Component Name: ");
  Serial.print(root_0_name);
  Serial.print(" : ");
  Serial.println(AutoMode_value);
  /*
     If you don't understand this Conversion, then goto Api & json documantation page.
  */


  JsonObject root_1 = doc[1];
  String root_1_name = root_1["name"]; // "Relay2"
  ManualMode_value = root_1["value"]; // "0"
  Serial.print("Component Name: ");
  Serial.print(root_1_name);
  Serial.print(" : ");
  Serial.println(ManualMode_value);

  JsonObject root_2 = doc[2];
  String root_2_name = root_2["name"]; // "Fan"
  V_Fan_value = root_2["value"]; // "0"
  Serial.print("Component Name: ");
  Serial.print(root_2_name);
  Serial.print(" : ");
  Serial.println(V_Fan_value);

}
//Reading Temperature from DHT11 sensor.
void Temperature_Read() {
  Temperature = dht.readTemperature();
  Serial.print("Room Temperature: ");
  Serial.print(Temperature);
  Serial.println("°C");
}

/*
   In this Function, Control your device from server.
   When you will send a GET Request to the server, the server will send back a json to you.
   Then you will extract them and use those value as you want.
*/

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
      Temperature_Read();
      if (Temperature > 24) {
        digitalWrite(Fan, LOW);
        Fan_state = 1;
        Serial.println("Fan ON");
        Serial.println("AutoMode ON");
        Serial.println("ManualMode OFF");
      }
      else {
        digitalWrite(Fan, HIGH);
        Fan_state = 0;
        Serial.println("Fan OFF");
        Serial.println("AutoMode ON");
        Serial.println("ManualMode OFF");
      }
      AutoMode_state = 1;
      ManualMode_state = 0;
    }
    else if ((AutoMode_value == 0) && (ManualMode_value == 1)) {

      AutoMode_state = 0;
      ManualMode_state = 1;
      if (V_Fan_value == 1) {
        digitalWrite(Fan, LOW);
        Fan_state = 1;
        Serial.println("Fan ON");
        Serial.println("AutoMode OFF");
        Serial.println("ManualMode ON");
      }
      else if (V_Fan_value == 0) {
        digitalWrite(Fan, HIGH);
        Fan_state = 0;
        Serial.println("Fan OFF");
        Serial.println("AutoMode OFF");
        Serial.println("ManualMode ON");
      }


    }
    else if ((AutoMode_value == 0) && (ManualMode_value == 0)) {
      digitalWrite(Fan, HIGH);
      Fan_state = 0;
      AutoMode_state = 0;
      ManualMode_state = 0;
      Serial.println("Fan OFF");
      Serial.println("AutoMode OFF");
      Serial.println("ManualMode OFF");
    }


  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
    /*
       If you find httpResponseCode: 200, it means your request was successful.
       And if you find any error code, then search on google and solve it.
       If you don't know how to solve it, then give a knock to the support.
    */
  }
  // Free resources
  http.end();
}




void Control_Server_From_Device() {
  /*In this Function, Control your Server from Device.
    When you send a POST Request to the server, server will update your command on dashbaord.
    When you want to send a POST Request to the server, you will need to make a Json.
    Read api&json documantation here for IoT Snacksbox:
    Serialize your json using arduino json assistant: https://arduinojson.org/v6/assistant/
    Here, we are just posting json from our device to Server.
  */

  HTTPClient http;
  // Your Domain name with URL path or IP address with path
  http.begin(ActionPost_serverName);
  //Here doc(192)is json size(how much memory need for serializing this json).
  char buffer[192];
  DynamicJsonDocument doc(192);

  JsonArray actions = doc.createNestedArray("actions");

  JsonObject actions_0 = actions.createNestedObject();
  actions_0["name"] = "AutoMode";
  actions_0["value"] = AutoMode_state;

  JsonObject actions_1 = actions.createNestedObject();
  actions_1["name"] = "ManualMode";
  actions_1["value"] = ManualMode_state;

  JsonObject actions_2 = actions.createNestedObject();
  actions_2["name"] = "Fan";
  actions_2["value"] = Fan_state;


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


  // Your Domain name with URL path or IP address with path
  http.begin(TempPost_serverName);
  // Specify content-type header
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Content-Type", "application/json");
  buffer[32];
  DynamicJsonDocument doc1(32);
  JsonObject data = doc1.createNestedObject("data");
  Temperature_Read();
  data["Temperature"] = Temperature;

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
