#ifndef TIMED_OUTPUT_H
#define TIMED_OUTPUT_H

#include <Arduino.h>

class TimedOutput {
  public:
    TimedOutput(int pinNumber, boolean normally = LOW);

    void write(boolean value, unsigned int time);
    boolean read();
    void update();
  private:
    int pinNumber;
    boolean normally;
    
    unsigned long switchTime = 0;
    unsigned int switchDuration = 0;
};

#endif


