#ifndef SLEEP_H
#define SLEEP_H

#include <Snooze.h>

class Sleep {
  public:
    Sleep(int interruptPin, long timeout) {
      this->interruptPin = interruptPin;
      this->timeout = timeout;
      this->digitalSnooze = new SnoozeDigital();
      this->snoozeConfig = new SnoozeBlock(*this->digitalSnooze);
    }
    void setup() {
      this->digitalSnooze->pinMode(this->interruptPin, INPUT, RISING);      
    }
    void update() {
      if (this->sleepRequestTime > 0 && millis() - this->sleepRequestTime >= this->timeout) {
        this->sleepRequestTime = 0;
        Serial.println("Going to sleep");
        Snooze.deepSleep(*this->snoozeConfig);
        Serial.println("Waking up!");
      }
    }
    void deepSleep() {
      this->sleepRequestTime = millis();
    }
    virtual ~Sleep() {
      delete this->snoozeConfig;
    };
  private:
    int interruptPin;
    unsigned long timeout;
    unsigned long sleepRequestTime = 0;
    SnoozeBlock *snoozeConfig;
    SnoozeDigital *digitalSnooze;
};

#endif
