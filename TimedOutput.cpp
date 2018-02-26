#include <Arduino.h>
#include "TimedOutput.h"

TimedOutput::TimedOutput(int pinNumber, boolean normally) {
  pinMode(pinNumber, OUTPUT);
  digitalWrite(pinNumber, normally);
  this->pinNumber = pinNumber;
  this->normally = normally;
}

void TimedOutput::write(boolean value, unsigned int duration) {
  this->switchTime = millis();
  this->switchDuration = duration;
  digitalWrite(this->pinNumber, value);
}

boolean TimedOutput::read() {
  return digitalRead(this->pinNumber);
}

void TimedOutput::update() {
  if (millis() - switchTime >= this->switchDuration) {
    digitalWrite(this->pinNumber, this->normally);
  }
}


