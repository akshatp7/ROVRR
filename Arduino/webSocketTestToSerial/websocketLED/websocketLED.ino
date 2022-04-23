#include <WiFi.h>
#include <Arduino_JSON.h>
#include <ArduinoWebsockets.h>

const char* ssid = "";  // Enter SSID here
const char* password = "";  //Enter Password here

const int fwd_LED_pin = 16;
const int bck_LED_pin = 19;
const int lft_LED_pin = 13;
const int rht_LED_pin = 14;
const int freq = 5000; // LED PWM frequency
const int fwd_LED_channel = 0;
const int bck_LED_channel = 1;
const int lft_LED_channel = 2;
const int rht_LED_channel = 3;
const int resolution = 8; // LED PWM resolution

using namespace websockets;

WebsocketsServer server;
WebsocketsClient client;

typedef struct input_LED_duty_cycle{
  int fwd;
  int bck;
  int lft;
  int rht;
} input_LED_duty_cycle_t;

input_LED_duty_cycle_t input_LED_duty_cycle = {0};

int connectedClient = 1;

void setup() {
  Serial.begin(115200);

  // configure LED PWM functionalitites
  ledcSetup(fwd_LED_channel, freq, resolution);
  ledcSetup(bck_LED_channel, freq, resolution);
  ledcSetup(lft_LED_channel, freq, resolution);
  ledcSetup(rht_LED_channel, freq, resolution);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(fwd_LED_pin, fwd_LED_channel);
  ledcAttachPin(bck_LED_pin, bck_LED_channel);
  ledcAttachPin(lft_LED_pin, lft_LED_channel);
  ledcAttachPin(rht_LED_pin, rht_LED_channel);
  
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
    //Serial.print("Got Message: ");
    String body = msg.data();
    JSONVar json_body = JSON.parse(body);
    Serial.print((int) json_body["X"]);Serial.print(",");
    Serial.println((int) json_body["Y"]);
    input_LED_duty_cycle.fwd = -((int) json_body["Y"]);
    input_LED_duty_cycle.bck = (int) json_body["Y"];
    input_LED_duty_cycle.lft = -((int) json_body["X"]);
    input_LED_duty_cycle.rht = (int) json_body["X"];
    set_LEDs(&input_LED_duty_cycle);
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

  delay(20);
}

void set_LEDs(input_LED_duty_cycle_t* inp){
  (inp->fwd <= 0) ? ledcWrite(fwd_LED_channel,0) : ledcWrite(fwd_LED_channel,inp->fwd);
  (inp->bck <= 0) ? ledcWrite(bck_LED_channel,0) : ledcWrite(bck_LED_channel,inp->bck);
  (inp->lft <= 0) ? ledcWrite(lft_LED_channel,0) : ledcWrite(lft_LED_channel,inp->lft);
  (inp->rht <= 0) ? ledcWrite(rht_LED_channel,0) : ledcWrite(rht_LED_channel,inp->rht);
}
