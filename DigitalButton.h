#ifndef DIGITAL_BUTTON_H
#define DIGITAL_BUTTON_H

#include <Arduino.h>

class DigitalButton {
  public:
    DigitalButton(int pinNumber, int mode, boolean compareMode = HIGH, unsigned int debounceDurationLimit = 20, unsigned int releaseDurationLimit = 700);

    boolean wasPressedFor(int duration);
    boolean wasPressedTimes(int count);
    boolean wasPressedTimesOrMore(int count);

    void update();

  private:
    static const int STATE_IDLE     = 0;
    static const int STATE_PRESSED  = 1;
    static const int STATE_RELEASED = 2;
  
    int pinNumber;
    int pinValue;
    unsigned int debounceDurationLimit;
    unsigned long debounceTime = 0;
    boolean compareMode;
    
    unsigned long stateTime = 0;
    int state = DigitalButton::STATE_IDLE;
    unsigned int pressCounter = 0;
    unsigned int releaseDurationLimit = 0;

    int getState(boolean pinValue, unsigned long updateTime);
};

#endif

