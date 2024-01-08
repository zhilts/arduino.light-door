#include "Arduino.h"
#include <Servo.h>
#include "Settings.h"

Servo myservo;
const int numReadings = 10;
int readings[numReadings];
int readIndex = 0;
int total = 0;
int lastLight = 0;

void Serial_print(const String &str) {
    if (USE_SERIAL) {
        Serial.print(str);
    }
}

void Serial_println(const String &str) {
    if (USE_SERIAL) {
        Serial.println(str);
    }
}

void servoSet(int value, bool tornOff = true) {
    Serial_println("Servo set: " + String(value));
    digitalWrite(MOSFET_PIN, 1);
    delay(100);
    myservo.write(value);
    delay(1500);
    if (tornOff) {
        digitalWrite(MOSFET_PIN, 0);
    }
}

void setup() {
    if (USE_SERIAL) {
        Serial.begin(9600);
    }
    Serial_println("setup");
    pinMode(LIGHT_PIN, INPUT);
    pinMode(MOSFET_PIN, OUTPUT);
    digitalWrite(MOSFET_PIN, 0);
    pinMode(SERVO_PIN, OUTPUT);
    myservo.attach(SERVO_PIN);
    servoSet(SERVO_MIN);
    for (int &reading: readings) {
        reading = 0;
    }
}

int getLight() {
    int sensor = analogRead(LIGHT_PIN);
    if (DEBUG_LED) {
        if (sensor > LIGHT_LIMIT) {
            digitalWrite(LED_PIN, HIGH);
        } else {
            digitalWrite(LED_PIN, LOW);
        }
    }

    Serial_print(String(sensor) + " ");
    if (USE_APPROXIMATION) {
        // subtract the last reading:
        total = total - readings[readIndex];
        readings[readIndex] = sensor;
        // add the reading to the total:
        total = total + readings[readIndex];
        // advance to the next position in the array:
        readIndex = readIndex + 1;

        // if we're at the end of the array...
        if (readIndex >= numReadings) {
            // ...wrap around to the beginning:
            readIndex = 0;
        }

        return total / numReadings;
    } else {
        return sensor;
    }
}

void closeDoor() {
    Serial_println("Closing");
//    digitalWrite(LED_PIN, HIGH);
    servoSet(SERVO_MAX, false);
    servoSet(SERVO_MIN);
//    digitalWrite(LED_PIN, LOW);
}

void loop() {
    delay(1000);
    int newLight = getLight();

    Serial_println(String(newLight));
    if (newLight < LIGHT_LIMIT && lastLight >= LIGHT_LIMIT) {
        closeDoor();
    }
    lastLight = newLight;
}
