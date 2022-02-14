#include <SPI.h>
#include "xaar128.h"
#include "Arduino.h"

Xaar128::Xaar128() {}

void Xaar128::init() {
  pinMode(nSS1, OUTPUT);
  pinMode(nSS2, OUTPUT);
  pinMode(nCLK, OUTPUT);
  pinMode(SCK, OUTPUT);
  pinMode(nRESET, OUTPUT);
  pinMode(xVDD, OUTPUT);
  pinMode(nFIRE, OUTPUT);
  pinMode(SCK, OUTPUT);
  pinMode(MOSI, OUTPUT);

  pinMode(relayVHCH, OUTPUT);
  digitalWrite(relayVHCH, LOW);
  pinMode(relayVHCL, OUTPUT);
  digitalWrite(relayVHCL, LOW);

  pinMode(READY, INPUT);
  pinMode(MISO, INPUT);

  // Set initial state for nFIRE
  digitalWrite(nFIRE, HIGH);

  // Chip select disable
  digitalWrite(nSS2, HIGH);
  digitalWrite(nSS1, HIGH);

  //Reset is active low, set inactive
  digitalWrite(nRESET, HIGH);

  //xVDD LOW
  digitalWrite(xVDD, LOW);
  readyState = LOW;
  delay(10);
}

void Xaar128::powerUp() {
  digitalWrite(nRESET, LOW);
  delay(120);
  digitalWrite(xVDD, HIGH);
  delay(120);
  digitalWrite(relayVHCL, HIGH);
  delay(10);
  digitalWrite(relayVHCH, HIGH);
  delay(120);
  digitalWrite(nRESET, HIGH);
  delay(10);
}

void Xaar128::powerDown() {
  digitalWrite(nRESET, HIGH);
  delayMicroseconds(1);
  digitalWrite(nRESET, LOW);
  delay(100);
  digitalWrite(relayVHCH, LOW);
  delay(10);
  digitalWrite(relayVHCL, LOW);
  delay(100);
  digitalWrite(xVDD, LOW);
  delay(100);
  digitalWrite(nRESET, HIGH);
}


void Xaar128::loadBuffer64(byte *val) {
  byte B[64];
  memcpy(B, val, 64);
  for (int i = 0; i < 8; i++) {
    SPI.transfer(B[i]);
  }
}

bool Xaar128::fire() {
  // Wait for start of READY active cycle
  while (digitalRead(READY) == LOW) {}
  digitalWrite(nFIRE, LOW);
  // Check state. If READY does not go low, we failed to fire.
  delayMicroseconds(5);
  bool okay = (digitalRead(READY) == LOW);
  delayMicroseconds(115);
  digitalWrite(nFIRE, HIGH);
  delayMicroseconds(1);
  return okay;
}
