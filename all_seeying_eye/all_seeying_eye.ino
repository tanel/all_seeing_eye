/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Sweep
*/

// servo
#include <Servo.h>

Servo myservo;  // create servo object to control a servo
int pos = 0;    // variable to store the servo position
int servoPin = 9;
int maxServoDegrees = 150;
int minServoDegrees = 10;

// PIR 
int inputPin = 2;               // choose the input pin (for PIR sensor)
int pirState = LOW;             // we start, assuming no motion detected
int val = 0;                    // variable for reading the pin status
int startedAt = 0;
int finishedAt = 0;

// LED
int led1 = 6;
int led2 = 5;
int led3 = 3;
int led4 = 11;
int brightness = 0;    // how bright the LED is
int fadeAmount = 2;    // how many points to fade the LED by
int maxBrightness = 255;

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
 
  Serial.begin(9600);  

  myservo.write(maxServoDegrees);
  delay(1000);
}

void moveEyeDown() {
    for (pos = minServoDegrees; pos <= maxServoDegrees; pos += 1) { 
      // in steps of 1 degree
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15ms for the servo to reach the position
    }
}

void moveEyeUp() {
    // goes from 180 degrees to 0 degrees
    for (pos = maxServoDegrees; pos >= minServoDegrees; pos -= 1) { 
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15ms for the servo to reach the position
    }
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
  // if last show was more than n seconds ago (
  // or there hasnt been a show yet, check if we can
  // start one.
  // read PIR
  val = digitalRead(inputPin);  // read input value
  if (val == HIGH) {            // check if the input is HIGH
    if (pirState == LOW) {
      int secondsSince = (millis() - finishedAt) / 1000;
      if (secondsSince > 5 || !finishedAt) {
        Serial.println("Starting");
 
        moveEyeUp();

        fadeLedIn();
        
        // We only want to print on the output change, not state
        pirState = HIGH;
  
        startedAt = millis();
      }
    }
  }

  // if n seconds have passed since last motion detection,
  // turn it off now
  if (pirState == HIGH && startedAt) {
    int secondsSince = (millis() - startedAt) / 1000;
    if (secondsSince > 5) {
      Serial.println("Stopping");

      fadeLedOut();

      moveEyeDown();

      // We only want to print on the output change, not state
      pirState = LOW;

      finishedAt = millis();
    }
  }

}
