
// servo
#include <Servo.h>
Servo myservo;
int pos = 0;
int servoPin = 9;
int maxServoDegrees = 180;
int minServoDegrees = 10;
int stepBack = 20;

// PIR 
int PIR1 = 2;
int PIR2 = 4;
int eyeState = LOW;
unsigned long lastMotionAt = 0;
unsigned long finishedAt = 0;

// LED
int led1 = 6;
int led2 = 5;
int led3 = 3;
int led4 = 11;
int brightness = 0;
int fadeAmount = 2;
int maxBrightness = 255;

// Relays
int RELAY4 = 12;
int RELAY3 = 7;
int RELAY2 = 10;
int RELAY1 = 8;
int relayPause = 500;

// Are we debugging? Dont leave it enabled
int debugging = 1;

void setup() {
  // Turn lights via relay on
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  pinMode(RELAY4, OUTPUT);
  digitalWrite(RELAY1, LOW);
  digitalWrite(RELAY2, LOW);
  digitalWrite(RELAY3, LOW);
  digitalWrite(RELAY4, LOW);
  
  // servo
  myservo.attach(servoPin);

  // PIR
  pinMode(PIR1, INPUT);
  pinMode(PIR2, INPUT);

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
      myservo.write(pos);
      delay(15);
    }
    myservo.write(maxServoDegrees - stepBack);
    delay(300);

    eyeState = LOW;

    // Turn lights via relay on
    digitalWrite(RELAY1, LOW);
    delay(relayPause);
    digitalWrite(RELAY2, LOW);
    delay(relayPause);
    digitalWrite(RELAY3, LOW);
    delay(relayPause);
    digitalWrite(RELAY4, LOW);
}

void moveEyeUp() {
    // Turn lights via relay off
    digitalWrite(RELAY1, HIGH);
    delay(relayPause);
    digitalWrite(RELAY2, HIGH);
    delay(relayPause);
    digitalWrite(RELAY3, HIGH);
    delay(relayPause);
    digitalWrite(RELAY4, HIGH);
  
    for (pos = maxServoDegrees; pos >= minServoDegrees; pos -= 1) { 
      myservo.write(pos);
      delay(15);
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
      finishedAt = millis();
      char buf[100];
      sprintf(buf, "finished at %lu", finishedAt);
      debug(buf);
    }
  }
}
