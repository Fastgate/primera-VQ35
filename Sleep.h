#ifndef SLEEP_H
#define SLEEP_H

#include <Snooze.h>

SnoozeDigital digitalSnooze;
SnoozeBlock snoozeConfig(digitalSnooze);

class Sleep {
  public:
    Sleep(int interruptPin, long timeout) {
      this->interruptPin = interruptPin;
      this->timeout = timeout;

      pinMode(this->interruptPin, INPUT);
      digitalSnooze.pinMode(this->interruptPin, INPUT, FALLING);
    }
    void update() {
      if (this->sleepRequestTime > 0 && millis() - this->sleepRequestTime >= this->timeout) {
        this->sleepRequestTime = 0;
        //Serial.println("Going to sleep");
        Snooze.deepSleep(snoozeConfig);
        //Serial.println("Waking up!");
      }
    }
    void cancelSleepRequest() {
      this->sleepRequestTime = 0;
    }
    void deepSleep() {
      this->sleepRequestTime = millis();
    }
    ~Sleep() {
      //delete this->snoozeConfig; LIBRARY IS SHIT WE CANNOT DO THIS
    };
  private:
    int interruptPin;
    unsigned long timeout;
    unsigned long sleepRequestTime = 0;
};

#endif
