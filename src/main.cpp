#include <Arduino.h>
#include <processor.h>
#include <led.h>

#define LED_R 2
#define LED_G 1
#define LED_Y 0

char nmeaBuffer[100];
MicroNMEA nmea(nmeaBuffer, sizeof(nmeaBuffer));
Processor processor(7);

#define GPS Serial2

void setup() {
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_Y, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  off(LED_Y);

  Serial.begin(115200);
  GPS.begin(9600);

  on(LED_R);
  on(LED_G);

  delay(5000);

  MicroNMEA::sendSentence(GPS, "$PORZB");
  MicroNMEA::sendSentence(GPS, "$PORZB,RMC,1,GGA,1");
  MicroNMEA::sendSentence(GPS, "$PNVGNME,2,9,1");

  off(LED_R);
  processor.begin();
}

void loop() {
  while (GPS.available()) {
    char c = GPS.read();
    Serial.print(c);
    nmea.process(c);
  }

  if (nmea.isValid() && nmea.getYear() != 0) {
    processor.process(nmea) ? flash(LED_Y) : flash(LED_R);
    nmea.clear();
    flash(LED_BUILTIN);
  }
}
