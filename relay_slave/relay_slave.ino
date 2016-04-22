
// Read here on how to connect Master/Slave:
// 
// https://www.arduino.cc/en/Tutorial/MasterWriter

#define wire

#ifdef wire
#include <Wire.h>
#endif

// Relays
int RELAY4 = 12;
int RELAY3 = 7;
int RELAY2 = 10;
int RELAY1 = 8;
int relayPause = 2000;
int relayState = HIGH;

// Are we debugging? Dont leave it enabled
void setup() {
  #ifdef wire
  Wire.begin(8);                // join i2c bus with address x
  Wire.onReceive(receiveEvent); // register event
  #endif

  Serial.begin(9600);

  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  pinMode(RELAY4, OUTPUT);

  turnLights(HIGH);
}

void turnLights(int value) {
  Serial.println("Turn lights change");

  digitalWrite(RELAY1, value);
  delay(relayPause);
  digitalWrite(RELAY2, value);
  delay(relayPause);
  digitalWrite(RELAY3, value);
  delay(relayPause);
  digitalWrite(RELAY4, value);
  delay(relayPause);

  Serial.println("Turn lights changed");

  relayState = value;
}

void loop() {
  #ifdef wire
  delay(100);
  #else
  delay(1000);
  if (relayState) {
    turnLights(LOW);
  } else {
    turnLights(HIGH);
  }
  #endif
}

#ifdef wire
// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
  Serial.println("receiveEvent");
  while (Wire.available() > 0) {  // loop through all but the last
    char c = Wire.read();         // receive byte as a character
    Serial.println("Received from wire: ");
    Serial.println(c);              // print the character
    if ('-' == c) {
      turnLights(LOW);
    } else if ('+' == c) {
      turnLights(HIGH);
    }
  }
}
#endif
