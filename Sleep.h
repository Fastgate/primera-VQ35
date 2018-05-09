#ifndef SLEEP_H
#define SLEEP_H

#include <Snooze.h>

class Sleep {
  public:
    Sleep(int interruptPin, long timeout) {
      this->interruptPin = interruptPin;
      this->timeout = timeout;
      SnoozeDigital digitalSnooze;
      digitalSnooze.pinMode(this->interruptPin, INPUT_PULLUP, RISING);
      this->snoozeConfig = new SnoozeBlock(digitalSnooze);
    }
    void update() {
      if (this->sleepRequestTime > 0 && millis() - this->sleepRequestTime >= this->timeout) {
        this->sleepRequestTime = 0;
        Snooze.deepSleep(*this->snoozeConfig);
      }
    }
    void deepSleep() {
      this->sleepRequestTime = millis();
    }
    virtual ~Sleep() {
      delete this->snoozeConfig;
    };
  protected:
    int interruptPin;
    unsigned long timeout;
    unsigned long sleepRequestTime = 0;
    SnoozeBlock *snoozeConfig;
};

#endif
