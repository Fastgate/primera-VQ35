

#include <Arduino.h>
#include "DigitalButton.h"

DigitalButton::DigitalButton(int pinNumber, int mode, boolean compareMode, unsigned int debounceDurationLimit, unsigned int releaseDurationLimit) {
  pinMode(pinNumber, mode);
  this->pinNumber = pinNumber;
  this->compareMode = compareMode;
  this->debounceDurationLimit = debounceDurationLimit;
  this->releaseDurationLimit = releaseDurationLimit; 
}

void DigitalButton::update() {
  unsigned long updateTime = millis();
  int newState = this->getState(digitalRead(this->pinNumber), updateTime);

  if (newState != this->state) {
    this->state = newState;
    this->stateTime = updateTime;
    if (newState == DigitalButton::STATE_PRESSED) {
      this->pressCounter++;
    }
  } 
  else {
    if (this->state == DigitalButton::STATE_IDLE) {
      pressCounter = 0;
    }
  }
}

int DigitalButton::getState(boolean newPinValue, unsigned long updateTime) { 
  if (newPinValue != this->pinValue) {
    this->debounceTime = millis();
    this->pinValue = newPinValue;
  }

  if (millis() - this->debounceTime > this->debounceDurationLimit) {
    if (newPinValue == this->compareMode) {
      return DigitalButton::STATE_PRESSED;
    } 
    else {
      if (this->state == DigitalButton::STATE_PRESSED) {
        return DigitalButton::STATE_RELEASED;
      } else if (this->state == DigitalButton::STATE_RELEASED) {
        return updateTime - this->stateTime < this->releaseDurationLimit ? DigitalButton::STATE_RELEASED : DigitalButton::STATE_IDLE;
      }
    }
  } 

  return this->state;
}

boolean DigitalButton::wasPressedFor(int duration) {
  return this->state == DigitalButton::STATE_RELEASED &&
    this->stateTime >= duration;
}

boolean DigitalButton::wasPressedTimes(int count) {
  return this->state == DigitalButton::STATE_IDLE &&
    this->pressCounter == count;
}

boolean DigitalButton::wasPressedTimesOrMore(int count) {
  return this->state == DigitalButton::STATE_IDLE &&
    this->pressCounter >= count;
}


