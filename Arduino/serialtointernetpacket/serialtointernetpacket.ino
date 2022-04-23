#include <WiFi.h>
#include <DHT_U.h> // Adafruit Universal Sensor Library
#include <Arduino_JSON.h>
#include <ArduinoWebsockets.h>

const char* ssid = "";  // Enter SSID here
const char* password = "";  //Enter Password here

using namespace websockets;

// Initialize Global Variables
char incomingByte;

WebsocketsServer server;

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  server.listen(80);
}

void loop() {
  WebsocketsClient client = server.accept();
  if(client.available()){
    WebsocketsMessage msg = client.readBlocking();
    JSONVar doc;
    doc["value"]=0;
    if (Serial.available()) {
        /* Command Handler */
        incomingByte = Serial.read();
        switch (incomingByte) {
            case '0':
                doc["value"] = -1;
                break;
            case '1':
                doc["value"] = 1;
                break;
            case 'c':
                doc["value"] = 0;
                break;
            default:
                break;
        }
    }
        
    String JSON_string = JSON.stringify(doc);
    client.send(JSON.stringify(doc));
    Serial.println(JSON_string);
    client.close();
  }
}
