#include <IRremote.h>      //including the remote library

#define fwd_button 6375  // code received from next button
#define rev_button 19125  // code received from previous button
#define left_button 4335  // code received from left button
#define right_button 23205 // code received from right button
#define Stop_button 14535   // code received from stop button

int receiver_pin = 2;      //output pin of IR receiver to pin 2 of arduino
//initializing the pins for leds
int left_motor1 = 11;      //pin 6 of arduino to pin 7 of l293d
int left_motor2 =  12;      //pin 7 of arduino to pin 2 of l293d
int right_motor1  = 6;     //pin 5 of arduino to pin 10 of l293d
int right_motor2 = 5;      //pin 4 of arduino to pin 15 of l293d

IRrecv receiver(receiver_pin); //Arduino will take output of IR receiver from pin 2
decode_results output;

void setup() {
  Serial.begin(9600);
  receiver.enableIRIn(); // Start to take the output from IR receiver
  //initializing all the pins where we have connected the led's as output pins
  pinMode(left_motor1, OUTPUT);
  pinMode(left_motor2, OUTPUT);
  pinMode(right_motor1, OUTPUT);
  pinMode(right_motor2, OUTPUT);
}

void loop() {
  if (receiver.decode(&output)) {
    unsigned int value = output.value;
    Serial.println(value);
    switch (value) {
      case fwd_button:
        digitalWrite(left_motor1, LOW);
        digitalWrite(left_motor2, HIGH);
        digitalWrite(right_motor1, HIGH);
        digitalWrite(right_motor2, LOW);
        break;
      case rev_button:
        digitalWrite(left_motor1, HIGH);
        digitalWrite(left_motor2, LOW);
        digitalWrite(right_motor1, LOW);
        digitalWrite(right_motor2, HIGH);
        break;
      case left_button:
        digitalWrite(left_motor1, LOW);
        digitalWrite(left_motor2, HIGH);
        digitalWrite(right_motor1, HIGH);
        digitalWrite(right_motor2, HIGH);
        break;
      case right_button:
        digitalWrite(left_motor1, HIGH);
        digitalWrite(left_motor2, HIGH);
        digitalWrite(right_motor1, HIGH);
        digitalWrite(right_motor2, LOW);
        break;
      case Stop_button:
        digitalWrite(left_motor1, LOW);
        digitalWrite(left_motor2, LOW);
        digitalWrite(right_motor1, LOW);
        digitalWrite(right_motor2, LOW);
        break;
    }
//    delay(1000)-;
    receiver.resume();
  }
}
