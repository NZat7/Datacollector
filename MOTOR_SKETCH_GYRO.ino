#include <Servo.h>
#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

Servo servo1;
int servoPin = 11;

int motor1pin1 = 2;
int motor1pin2 = 3;

int motor2pin1 = 4;
int motor2pin2 = 5;

long timePrev;
float yaw = 0;
float gyroZOffset = 0;
bool isTurning = false;

void setup() {
  wire.begin();

  servo1.attach(servoPin);
  servo1.write(0);

  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(motor2pin1, OUTPUT);
  pinMode(motor2pin2, OUTPUT);

  // Enable pins for motor power
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  digitalWrite(9, HIGH);
  digitalWrite(10, HIGH);

  mpu.initialize();

  long gyroZSum = 0;
  for (int i = 0; i < 100; i++) {
    gyroZSum += mpu.getRotationZ();
    delay(10);
  }
  gyroZOffset = gyroZSum / 100.0;

  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0) {
    char incomingChar = Serial.read();

    if (incomingChar == '0') { // Go forward
      Serial.println("Both Motors ON");
      digitalWrite(motor1pin1, HIGH);
      digitalWrite(motor1pin2, LOW);
      digitalWrite(motor2pin1, HIGH);
      digitalWrite(motor2pin2, LOW);
      delay(2000);
    } 
    else if (incomingChar == '1') { // Go right
      Serial.println("Turning Right");
      yaw = 0;
      timePrev = millis();
      digitalWrite(motor1pin1, HIGH);
      digitalWrite(motor1pin2, LOW);
      digitalWrite(motor2pin1, LOW);
      digitalWrite(motor2pin2, HIGH);

      while (yaw < 90.0) {
        long timeCur = millis();
        float dt = (timeCur - timePrev) / 1000.0; 
        timePrev = timeCur;

        float gyroZ = (mpu.getRotationZ() - gyroZOffset) / 131.0;
        yaw += abs(gyroZ * dt); 
        
        delay(10); 
      }
      digitalWrite(motor1pin1, LOW);
      digitalWrite(motor1pin2, LOW);
      digitalWrite(motor2pin1, LOW);
      digitalWrite(motor2pin2, LOW);
    } 
    else if (incomingChar == '2') { // Go left
      Serial.println("Turning Left");
      yaw = 0;
      timePrev = millis();
      digitalWrite(motor1pin1, LOW);
      digitalWrite(motor1pin2, HIGH);
      digitalWrite(motor2pin1, HIGH);
      digitalWrite(motor2pin2, LOW);

      while (yaw < 90.0) {
        long timeCur = millis();
        float dt = (timeCur - timePrev) / 1000.0; 
        timePrev = timeCur;

        float gyroZ = (mpu.getRotationZ() - gyroZOffset) / 131.0;
        yaw += abs(gyroZ * dt); 
        
        delay(10); 
      }
      digitalWrite(motor1pin1, LOW);
      digitalWrite(motor1pin2, LOW);
      digitalWrite(motor2pin1, LOW);
      digitalWrite(motor2pin2, LOW);
    }
    else if (incomingChar == '3') { // Go backwards 
      Serial.println("Both Motors ON backwards");
      digitalWrite(motor1pin1, LOW);
      digitalWrite(motor1pin2, HIGH);
      digitalWrite(motor2pin1, LOW);
      digitalWrite(motor2pin2, HIGH);
      delay(2000);
    }
    else if (incomingChar == '4') { // Use sensor
      Serial.println("Sensor in Use");
      delay(2000);
      for (int angle = 0; angle <= 180; angle++) {
        servo1.write(angle);
        delay(20);
    }
    delay(2000);

    for (int angle = 180; angle >= 0; angle--) {
      servo1.write(angle);
      delay(20);
    }
 delay(2000);
}

    // Stop both motors after the command has been executed 
    digitalWrite(motor1pin1, LOW);
    digitalWrite(motor1pin2, LOW);
    digitalWrite(motor2pin1, LOW);
    digitalWrite(motor2pin2, LOW);
  }
}
