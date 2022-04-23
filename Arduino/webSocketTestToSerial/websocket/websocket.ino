#include <WiFi.h>
#include <Arduino_JSON.h>
#include <ArduinoWebsockets.h>

const char* ssid = "";  // Enter SSID here
const char* password = "";  //Enter Password here

using namespace websockets;

WebsocketsServer server;
WebsocketsClient client;

int connectedClient = 1;

void setup() {
  Serial.begin(115200);
  // Connect to wifi
  WiFi.begin(ssid, password);

  // Wait some time to connect to wifi
  for(int i = 0; i < 15 && WiFi.status() != WL_CONNECTED; i++) {
      Serial.print(".");
      delay(1000);
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());   //You can get IP address assigned to ESP

  server.listen(80);
  Serial.print("Is server live? ");
  Serial.println(server.available());

  client = server.accept();
  while(!client.available()) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("Client Connected");
}

void loop() {
  WebsocketsMessage msg = client.readBlocking();

  if (connectedClient) {
    Serial.print("Got Message: ");
    Serial.println(msg.data());
  }
  // return echo
  //client.send("Echo: " + msg.data());

  // close the connection
  //client.close();

  if (msg.data() == "#") {
    client.close();
    connectedClient = 0;
    Serial.println("Client Disconnected. \nWaiting for new connection...");
    client = server.accept();
    while(!client.available()) {
      Serial.print(".");
      delay(1000);
    }
    connectedClient = 1;
  }

  delay(500);
}
