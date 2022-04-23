#include <WiFi.h>
#include <Arduino_JSON.h>
#include <ArduinoWebsockets.h>

const char* ssid = "";  // Enter SSID here
const char* password = "";  //Enter Password here

// Set your Static IP address
IPAddress local_IP(192, 168, 0, 18);
// Set your Gateway IP address
IPAddress gateway(192, 168, 0, 1);

IPAddress subnet(255, 255, 255, 0);

// Define constants

const int left_motor1 = 15;      //pin 6 of arduino to pin 7 of l293d
const int left_motor2 =  2;      //pin 7 of arduino to pin 2 of l293d
const int right_motor1  = 13;     //pin 5 of arduino to pin 10 of l293d
const int right_motor2 = 12;

const int freq = 5000; // LED PWM frequency
const int left_ch1 = 0; // Channel
const int left_ch2 = 1; // Channel
const int right_ch1 = 2; // Channel
const int right_ch2 = 3; //
const int resolution = 8; // LED PWM resolution

using namespace websockets;

WebsocketsServer server;
WebsocketsClient client;

bool connectedClient = 0;


// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

void setup() {
  Serial.begin(115200);

  // configure LED PWM functionalitites
  ledcSetup(left_ch1, freq, resolution);
  ledcSetup(left_ch2, freq, resolution);
  ledcSetup(right_ch1, freq, resolution);
  ledcSetup(right_ch2, freq, resolution);
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(left_motor1, left_ch1);
  ledcAttachPin(left_motor2, left_ch2);
  ledcAttachPin(right_motor1, right_ch1);
  ledcAttachPin(right_motor2, right_ch2);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  server.listen(80);
  Serial.print("Is server live? ");
  Serial.println(server.available());

  client = server.accept();
  while (!client.available()) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Client Connected");
  connectedClient = 1;
}

void loop() {
  WebsocketsMessage msg = client.readBlocking();

  if (connectedClient) {
    Serial.print("Got Message: ");
    Serial.println(msg.data());
  }
  JSONVar incoming_payload = JSON.parse(msg.data());
  int x_input = (int)incoming_payload["X"];
  int y_input = (int)incoming_payload["Y"];
  if (x_input > 255 || y_input > 255 || x_input < -255 || y_input < -255) {
    Serial.println("Wrong input from controller");
  }
  else {
    if (y_input >= 200 && abs(x_input) < 150) { // full fwd, spd3
      ledcWrite(left_ch1, 255);
      ledcWrite(left_ch2, 0);
      ledcWrite(right_ch1, 255);
      ledcWrite(right_ch2, 0);
    }
    else if (y_input >= 150 && y_input < 200 && abs(x_input) < 150) { // full fwd, spd2
      ledcWrite(left_ch1, 200);
      ledcWrite(left_ch2, 0);
      ledcWrite(right_ch1, 200);
      ledcWrite(right_ch2, 0);
    }
    else if (y_input > 100 && y_input < 150 && abs(x_input) < 150) { // full fwd, spd1
      ledcWrite(left_ch1, 150);
      ledcWrite(left_ch2, 0);
      ledcWrite(right_ch1, 150);
      ledcWrite(right_ch2, 0);
    }
    else if (y_input <= -200 && abs(x_input) < 150) { // full rev, spd3
      ledcWrite(left_ch1, 0);
      ledcWrite(left_ch2, 255);
      ledcWrite(right_ch1, 0);
      ledcWrite(right_ch2, 255);
    }
    else if (y_input <= -150 && y_input > -200 && abs(x_input) < 150) { // full rev, spd2
      ledcWrite(left_ch1, 0);
      ledcWrite(left_ch2, 200);
      ledcWrite(right_ch1, 0);
      ledcWrite(right_ch2, 200);
    }
    else if (y_input < -100 && y_input > -150 && abs(x_input) < 150) { // full rev, spd1
      ledcWrite(left_ch1, 0);
      ledcWrite(left_ch2, 150);
      ledcWrite(right_ch1, 0);
      ledcWrite(right_ch2, 150);
    }
    else if (x_input == 0 && y_input == 0) { // full stop
      ledcWrite(left_ch1, 0);
      ledcWrite(left_ch2, 0);
      ledcWrite(right_ch1, 0);
      ledcWrite(right_ch2, 0);
    }
    else if (x_input < 255 && x_input >= 200 && abs(y_input) < 100) { // right turn, spd2
      ledcWrite(left_ch1, 0);
      ledcWrite(left_ch2, 255);
      ledcWrite(right_ch1, 255);
      ledcWrite(right_ch2, 0);
    }
    else if (x_input < 200 && x_input > 150 && abs(y_input) < 100) { // right turn, spd1
      ledcWrite(left_ch1, 0);
      ledcWrite(left_ch2, 0);
      ledcWrite(right_ch1, 255);
      ledcWrite(right_ch2, 0);
    }
    else if (x_input > -255 && x_input <= -200 && abs(y_input) < 100) { // left turn, spd2
      ledcWrite(left_ch1, 255);
      ledcWrite(left_ch2, 0);
      ledcWrite(right_ch1, 0);
      ledcWrite(right_ch2, 255);
    }
    else if (x_input > -200 && x_input < -150 && abs(y_input) < 100) { // left turn, spd1
      ledcWrite(left_ch1, 255);
      ledcWrite(left_ch2, 0);
      ledcWrite(right_ch1, 0);
      ledcWrite(right_ch2, 0);
    }
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
    while (!client.available()) {
      Serial.print(".");
      delay(1000);
    }
    connectedClient = 1;
  }
}