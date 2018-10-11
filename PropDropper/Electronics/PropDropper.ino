#include <Servo.h>

// Servo definitions
Servo dropServo;
Servo drumServo;
int dropServoRaisedPosition = 0;
int dropServoDroppedPosition = 180;
int drumServoStoppedPosition = 90;
int drumServoWindPosition = 180;

// Ping sensor definitions
const int triggerPin = 11;
const int echoPin = 12;
long distance;

// Timings
int dropPin = 3;
int drumPin = 6;
int timeHang = 3000;
int timeWind = 3000;
int timeRaise = 500;
int timeCooldown = 3000;
int armed = 0;

void setup() {
  // Arduino board setup  
  pinMode(LED_BUILTIN, OUTPUT);

  // Serial port setup.
  Serial.begin(9600);

  // Ping sensor setup
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // Servos setup
  dropServo.attach(dropPin);
  drumServo.attach(drumPin);

  // Initialize.
  resetAll();
}

void loop() {
  waitForTrigger();
  Serial.println("DROPPING THE SPIDER!!!");
  setArmed(0);
  dropDrum();
  delay(timeHang);
  raiseDrum();
  windDrum();
  waitForCooldown();
  resetTrigger();
  setArmed(1);
}

void resetAll() {
  Serial.println("resetAll()");
  for (int j = 1; j <= 5; j++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  }
  
  setArmed(0);
  dropDrum();
  raiseDrum();
  windDrum();
  resetTrigger();
  setArmed(1);
}

void resetTrigger() {
  // Wait for target area to clear.
  // We will frequently get a 0 false positive, so we must watch for that too.
  int distance = readDistance();
  showDistance(distance);
  while ((distance < 60) && (distance > 3)) {
    delay(100);
    distance = readDistance();
    showDistance(distance);
  }
}

void waitForTrigger() {
  // Wait for intrusion in the target area.  
  // We will frequently get a 0 false positive, so we must watch for that too.
  int distance = readDistance();
  showDistance(distance);
  while ((distance > 40) || (distance < 3)) {
    delay(100);
    distance = readDistance();
    showDistance(distance);
  }
}

void dropDrum() {
  Serial.println("dropDrum()");
  dropServo.write(dropServoDroppedPosition);
}

void raiseDrum() {
  Serial.println("raiseDrum()");
  dropServo.write(dropServoRaisedPosition);
  delay(timeRaise);
}

void windDrum() {
  Serial.println("windDrum()");
  drumServo.write(drumServoWindPosition);
  delay(timeWind);
  drumServo.write(drumServoStoppedPosition);
}

void waitForCooldown() {
  Serial.println("waitForCooldown()");
  delay(timeCooldown);
}

void setArmed(int value) {
  armed = value;
  if (armed == 0) {
    Serial.println("Disarmed.");
    digitalWrite(LED_BUILTIN, LOW);
  }
  else {
    Serial.println("Armed!");
    digitalWrite(LED_BUILTIN, HIGH);
  }
}

void showDistance(long d) {
  Serial.print("Distance (cm): ");
  Serial.println(d);
}

long readDistance()
{
  long duration;
  long averagecm = 0;
  long averageinches = 0;
  int readCount = 10;
  long cm;
  long inches;

  for(int j = 0; j < readCount; j++) {
    // Reset before a read.
    digitalWrite(triggerPin, LOW);
    delayMicroseconds(5);
  
    // Trigger the sensor to do a read.
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(triggerPin, LOW);
  
    // Read the output signal from the sensor.
    duration = pulseIn(echoPin, HIGH);
  
    // Calculate the distance.
    cm = (duration / 2) / 29.1;
    inches = (duration / 2) / 74;
    averagecm = averagecm + cm;
    averageinches = averageinches + inches;
  }
  averagecm = averagecm  / readCount;
  averageinches = averageinches / readCount;

  return averagecm;
}
