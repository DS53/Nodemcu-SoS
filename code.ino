#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include "ESP8266WiFi.h"

int button = 16;
int temp = 0;

char myssid[] = "Akatsuki";        
char mypass[] = "pgn03g5axq9c71";          


const char* host = "maker.ifttt.com";
const int httpsPort = 443;
const int API_TIEOUT = 10000;


const char* Host = "maps.googleapis.com";
String thisPage = "/maps/api/js?key=";
String key = "AIzaSyD6D6FdPa4A1h4vB9HDxT06s0scqkkuJMs";

int status = WL_IDLE_STATUS;
String jsonString = "{\n";

double latitude    = 0.0;
double longitude   = 0.0;
double accuracy    = 0.0;
int more_text = 1;
void setup() {
 Serial.begin(115200);
  pinMode(button, INPUT);

  Serial.println("Start");
  
  WiFi.disconnect();
  delay(100);
  Serial.println("Setup done");
 
  Serial.print("Connecting to ");
  Serial.println(myssid);
  WiFi.begin(myssid, mypass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(".");


}

void loop() {
 char bssid[6];

  WifiLocation location(Key);
  
  DynamicJsonBuffer jsonBuffer;
  Serial.println("scan start");
  
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(n);
    Serial.println(" networks found...");

    if (more_text) {
      
      Serial.println("{");
      Serial.println("\"homeMobileCountryCode\": 91,");  
      Serial.println("\"homeMobileNetworkCode\": 0,");   
      Serial.println("\"radioType\": \"gsm\",");        
      Serial.println("\"carrier\": \"Jio\",");       
      //Serial.println("\"cellTowers\": [");                
      //Serial.println("],");
      Serial.println("\"wifiAccessPoints\": [");
      for (int i = 0; i < n; ++i)
      {
        Serial.println("{");
        Serial.print("\"macAddress\" : \"");
        Serial.print(WiFi.BSSIDstr(i));
        Serial.println("\",");
        Serial.print("\"signalStrength\": ");
        Serial.println(WiFi.RSSI(i));
        if (i < n - 1)
        {
          Serial.println("},");
        }
        else
        {
          Serial.println("}");
        }
      }
      Serial.println("]");
      Serial.println("}");
    }
    Serial.println(" ");
  }

  jsonString = "{\n";
  jsonString += "\"homeMobileCountryCode\": 91,\n"; 
  jsonString += "\"homeMobileNetworkCode\": 0,\n";  
  jsonString += "\"radioType\": \"gsm\",\n";         
  jsonString += "\"carrier\": \"Jio\",\n";      
  jsonString += "\"wifiAccessPoints\": [\n";
  for (int j = 0; j < n; ++j)
  {
    jsonString += "{\n";
    jsonString += "\"macAddress\" : \"";
    jsonString += (WiFi.BSSIDstr(j));
    jsonString += "\",\n";
    jsonString += "\"signalStrength\": ";
    jsonString += WiFi.RSSI(j);
    jsonString += "\n";
    if (j < n - 1)
    {
      jsonString += "},\n";
    }
    else
    {
      jsonString += "}\n";
    }
  }
  jsonString += ("]\n");
  jsonString += ("}\n");
 

  Serial.println("");
   WiFiClientSecure client;
    Serial.print("Requesting URL: ");
  Serial.println("https://" + (String)Host + thisPage + key);
  Serial.println(" ");
  if (client.connect(Host, 443)) {
    Serial.println("Connected");
    client.println("POST " + thisPage + key + " HTTP/1.1");
    client.println("Host: " + (String)Host);
    client.println("Connection: close");
    client.println("Content-Type: application/json");
    client.println("User-Agent: Arduino/1.0");
    client.print("Content-Length: ");
    client.println(jsonString.length());
    client.println();
    client.print(jsonString);
    delay(500);
  }
  location_t loc = location.getGeoFromWiFi();

    Serial.println("Location request data");
    Serial.println(location.getSurroundingWiFiJson());
    Serial.println("Latitude: " + String(loc.lat, 7));
    Serial.println("Longitude: " + String(loc.lon, 7));
    Serial.println("Accuracy: " + String(loc.accuracy));

 
  while (client.available()) {
    String line = client.readStringUntil('\r');
    if (more_text) {
      Serial.print(line);
    }
    JsonObject& root = jsonBuffer.parseObject(line);
    if (root.success()) {
      latitude    = root["location"]["lat"];
      longitude   = root["location"]["lng"];
      accuracy   = root["accuracy"];
    }
  }

  Serial.println("closing connection");
  Serial.println();
  client.stop();

  Serial.print("Latitude = ");
  Serial.println(latitude, 6);
  Serial.print("Longitude = ");
  Serial.println(longitude, 6);
  Serial.print("Accuracy = ");
  Serial.println(accuracy);

  delay(3000);
  temp = digitalRead(button);

  if (temp == HIGH) {
    Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  BearSSL::WiFiClientSecure client;
  client.setInsecure();
  client.setTimeout(API_TIEOUT);
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  String url = "/trigger/msg/with/key/dY0cWJc7L3RgghDml9XeYg";
  url += "?value1="
  url += longitude
  url += "&value2="
  url += latitude
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');

  Serial.println("reply was:");
  Serial.println("==========");
  Serial.println(line);
  Serial.println("==========");
  Serial.println("closing connection");

  }
  else {

  }
  
  delay(50000);


}
