#include <Arduino.h>
#include <processor.h>

#define LED_R 20
#define LED_G 21
#define LED_Y 22

char nmeaBuffer[100];
MicroNMEA nmea(nmeaBuffer, sizeof(nmeaBuffer));
Processor processor(10);

void flash(int led) {
  digitalWrite(led, 1);
  delay(100);
  digitalWrite(led, 0);
}

void blink(int led) {
  digitalWrite(led, 0);
  delay(100);
  digitalWrite(led, 1);
}

void setup() {
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_Y, OUTPUT);

  digitalWrite(LED_R, 0);
  digitalWrite(LED_Y, 0);

  Serial.begin(9600);
  Serial2.begin(9600);

  digitalWrite(LED_G, 1);
}

void loop() {
  if (Serial2.available()) {
    char c = Serial2.read();
    nmea.process(c);

    if (nmea.isValid()) {
      processor.process(nmea) ? flash(LED_Y) : flash(LED_R);
    } else {
      blink(LED_G);
    }
  }
}
