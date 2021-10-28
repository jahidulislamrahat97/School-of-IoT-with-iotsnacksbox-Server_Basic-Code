#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include "DHT.h"
#include "FS.h"
#include "SD.h"


const char* ssid = "House of R&D";
const char* password = "12345HouseofR&D";

const char* TemperaturePost_serverName = "https://api.iotsnacksbox.io/trigger/Temperature?snacksboxtoken=df4ea9286188ed53b96d8b434c80ccd9dde91e3be46b32601da5acf7807bd7a0" ;
const char* HumidityPost_serverName    = "https://api.iotsnacksbox.io/trigger/Humidity?snacksboxtoken=df4ea9286188ed53b96d8b434c80ccd9dde91e3be46b32601da5acf7807bd7a0" ;
const char* SmokePost_serverName       = "https://api.iotsnacksbox.io/trigger/Smoke?snacksboxtoken=df4ea9286188ed53b96d8b434c80ccd9dde91e3be46b32601da5acf7807bd7a0" ;
const char* LightPost_serverName       = "https://api.iotsnacksbox.io/trigger/Light?snacksboxtoken=df4ea9286188ed53b96d8b434c80ccd9dde91e3be46b32601da5acf7807bd7a0" ;


#define DHTPIN 15
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

int Smoke_Sensor = 39;
int Light_Sensor = 34;

int Temperature;
int Humidity;
int Smoke;
int Light;

#define SD_CS 5
RTC_DATA_ATTR int readingID = 0;
String dataMessage;

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


  SD.begin(SD_CS);
  if (!SD.begin(SD_CS)) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  Serial.println("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("ERROR - SD card initialization failed!");
    return;    // init failed
  }

  // If the data.txt file doesn't exist
  // Create a file on the SD card and write the data labels
  File file = SD.open("/data.txt");
  if (!file) {
    Serial.println("File doens't exist");
    Serial.println("Creating file...");
    writeFile(SD, "/data.txt", "Reading ID, Temperature, Humidity, Smoke, Light \r\n");
  }
  else {
    Serial.println("File already exists");
  }
  file.close();




}

void logSDCard() {
  dataMessage = String(readingID) + "," + String(Temperature) + "," + String(Humidity) + "," + String(Smoke) + "," +
                String(Light) + "\r\n";
  Serial.print("Save data: ");
  Serial.println(dataMessage);
  appendFile(SD, "/data.txt", dataMessage.c_str());
}

// Write to the SD card (DON'T MODIFY THIS FUNCTION)
void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

// Append data to the SD card (DON'T MODIFY THIS FUNCTION)
void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
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
    readingID++;
    Temperature_Read();
    Humidity_Read();
    Smoke_Read();
    Light_Read();
    logSDCard();
    Control_Server_From_Device();
  }
  else {
    Serial.println("WiFi Disconnected");
  }


}
