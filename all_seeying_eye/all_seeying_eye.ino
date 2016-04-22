
// Read here on how to connect Master/Slave:
// 
// https://www.arduino.cc/en/Tutorial/MasterWriter

#define wire
#define wiretest

#ifdef wire
#include <Wire.h>
#endif

// servo
#include <Servo.h>
Servo myservo;
int pos = 0;
int servoPin = 9;
const int maxServoDegrees = 180;
const int minServoDegrees = 0;
const int stepBack = 0;

// PIR 
const int PIR1 = 2;
const int PIR2 = 4;
int eyeState = LOW;
unsigned long lastMotionAt = 0;
unsigned long finishedAt = 0;

// LED
const int led1 = 6;
const int led2 = 5;
const int led3 = 3;
const int led4 = 11;
int brightness = 0;
const int fadeAmount = 2;
const int maxBrightness = 255;

#ifndef wire
// Relays
const int RELAY4 = 12;
const int RELAY3 = 7;
const int RELAY2 = 10;
const int RELAY1 = 8;
const int relayPause = 2000;
#endif

// Are we debugging? Dont leave it enabled
const int debugging = 1;

void setup() {
  if (debugging) {
    Serial.begin(9600);
  }

#ifdef wire
  Wire.begin();
#endif

  // PIR
  pinMode(PIR1, INPUT);
  pinMode(PIR2, INPUT);

  // LED
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);

  // Set up servo
  myservo.attach(servoPin);
  myservo.write(maxServoDegrees);
  delay(1000);
  myservo.write(maxServoDegrees - stepBack);
  delay(300);
  myservo.detach();

  Serial.println("Setup done");

  turnLights(LOW);
}

void debug(String text) { 
  if (debugging) {
    Serial.println(text);
  }
}

void moveEyeDown() {
    myservo.attach(servoPin);

    for (pos = minServoDegrees; pos <= maxServoDegrees; pos += 1) { 
      // in steps of 1 degree
      myservo.write(pos);
      delay(15);
    }
    myservo.write(maxServoDegrees - stepBack);
    delay(300);

    myservo.detach();

    eyeState = LOW;
}

void turnLights(int value) {
    if (value) {
      debug("turning lights on");
    } else {
      debug("turning lights off");
    }

    // Turn lights via relay on/off
    #ifdef wire
    Wire.beginTransmission(8); // transmit to device #8
    byte written = 0;
    if (value) {
      written = Wire.write('+');
    } else {
      written = Wire.write('-');
    }
    {
      char buf[30];
      sprintf(buf, "%d bytes written to wire", written);
      debug(buf);
    }
    delay(100);
    byte status = Wire.endTransmission();
    switch (status) {
      case 0:
        debug("success");
        break;
      case 1:
        debug("data too long to fit in transmit buffer");
        break;
      case 2:
        debug("received NACK on transmit of address");
        break;
      case 3:
        debug("received NACK on transmit of data");
        break;
      case 4:
        debug("other error");
        break;
      default:
        debug("unknown error");
    }
    #else
    digitalWrite(RELAY1, value);
    delay(relayPause);
    digitalWrite(RELAY2, value);
    delay(relayPause);
    digitalWrite(RELAY3, value);
    delay(relayPause);
    digitalWrite(RELAY4, value);
    #endif

    if (value) {
      debug("turned lights on");
    } else {
      debug("turned lights off");
    }
}

void moveEyeUp() {
    myservo.attach(servoPin);
  
    for (pos = maxServoDegrees; pos >= minServoDegrees; pos -= 1) { 
      myservo.write(pos);
      delay(15);
    }
    myservo.write(minServoDegrees + stepBack);
    delay(300);

    myservo.detach();

    eyeState = HIGH;
}

void fadeLedIn() {
  while (brightness < maxBrightness) {
    analogWrite(led1, brightness);
    analogWrite(led2, brightness);
    analogWrite(led3, brightness);
    analogWrite(led4, brightness);
    
    brightness = brightness + fadeAmount;

    delay(30);
  }
}

void fadeLedOut() {
  // Fade LED out
  if (brightness > maxBrightness) {
    brightness = maxBrightness;
  }

  while (brightness > 0) {
    analogWrite(led1, brightness);
    analogWrite(led2, brightness);
    analogWrite(led3, brightness);
    analogWrite(led4, brightness);
    
    brightness = brightness - fadeAmount;

    delay(30);
  }

  analogWrite(led1, 0);
  analogWrite(led2, 0);
  analogWrite(led3, 0);
  analogWrite(led4, 0);
}

void loop() {
  delay(100);

  #ifdef wiretest
  turnLights(LOW);
  delay(5000);
  turnLights(HIGH);
  delay(5000);
  return;
  #endif

  int val1 = digitalRead(PIR1);
  int val2 = digitalRead(PIR2);
  int val = LOW;
  if (HIGH == val1 || HIGH == val2) {
    val = HIGH;
  }

  if (HIGH == val) {
    // If last show was less than N seconds ago,
    // Ignore the movement for now. We want the
    // visitor to be able to leave the premise,
    // before the show starts again.
    int secondsSince = (millis() - finishedAt) / 1000;
    if (finishedAt > 0 && secondsSince < 60) {
      return;
    }
    lastMotionAt = millis();
    if (LOW == eyeState) {
      char buf[30];
      sprintf(buf, "%d seconds since last finish", secondsSince);
      debug(buf);
      debug("Starting");
      turnLights(HIGH);
      moveEyeUp();
      fadeLedIn();
    }
    return;
  }
    
  if (LOW == val && HIGH == eyeState) {
    int secondsSince = (millis() - lastMotionAt) / 1000;
    if (secondsSince > 5) {
      debug("Stopping");
      fadeLedOut();
      moveEyeDown();
      turnLights(LOW);

      finishedAt = millis();
      char buf[100];
      sprintf(buf, "finished at %lu", finishedAt);
      debug(buf);
    }
  }
}
