
#include "ApplicationMonitor.h"

// servo
#include <Servo.h>
Servo myservo;
int pos = 0;
int servoPin = 9;
int maxServoDegrees = 180;
int minServoDegrees = 10;
int stepBack = 20;

// PIR 
int inputPin = 2;
int eyeState = LOW;
int val = 0;
int lastMotionAt = 0;
int finishedAt = 0;

// LED
int led1 = 6;
int led2 = 5;
int led3 = 3;
int led4 = 11;
int brightness = 0;
int fadeAmount = 2;
int maxBrightness = 255;

// Are we debugging? Dont leave it enabled
int debugging = 0;

void setup() {
  // servo
  myservo.attach(servoPin);  // attaches the servo on pin 9 to the servo object

  // PIR
  pinMode(inputPin, INPUT);     // declare sensor as input

  // LED
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);

  if (debugging) {
    Serial.begin(9600);
  }

  myservo.write(maxServoDegrees);
  delay(1000);
  myservo.write(maxServoDegrees - stepBack);
  delay(300);
}

void debug(String text) { 
  if (debugging) {
    Serial.println(text);
  }
}

void moveEyeDown() {
    for (pos = minServoDegrees; pos <= maxServoDegrees; pos += 1) { 
      // in steps of 1 degree
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15ms for the servo to reach the position
    }
    myservo.write(maxServoDegrees - stepBack);
    delay(300);

    eyeState = LOW;
}

void moveEyeUp() {
    for (pos = maxServoDegrees; pos >= minServoDegrees; pos -= 1) { 
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15ms for the servo to reach the position
    }
    myservo.write(minServoDegrees + stepBack);
    delay(300);

    eyeState = HIGH;
}

void fadeLedIn() {
  while (brightness < maxBrightness) {
    analogWrite(led1, brightness);
    analogWrite(led2, brightness);
    analogWrite(led3, brightness);
    analogWrite(led4, brightness);
    
    // change the brightness for next time through the loop:
    brightness = brightness + fadeAmount;

    // wait for 30 milliseconds to see the dimming effect
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
    
    // change the brightness for next time through the loop:
    brightness = brightness - fadeAmount;

    // wait for 30 milliseconds to see the dimming effect
    delay(30);
  }
}

void loop() {
  val = digitalRead(inputPin);
  if (HIGH == val) {
    lastMotionAt = millis();
    if (LOW == eyeState) {
      debug("Starting");
      moveEyeUp();
      fadeLedIn();
    }
  } else if (LOW == val && HIGH == eyeState) {
    int secondsSince = (millis() - lastMotionAt) / 1000;
    if (secondsSince > 5) {
      debug("Stopping");
      fadeLedOut();
      moveEyeDown();
      finishedAt = millis();
    }
  }
}
