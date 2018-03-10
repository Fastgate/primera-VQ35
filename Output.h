#ifndef OUTPUT_H
#define OUTPUT_H

class Output {
  public:
    Output(int pinNumber, boolean normally = LOW) {
      this->pinNumber = pinNumber;
      this->normally = normally;
      pinMode(this->pinNumber, OUTPUT);
      this->reset();
    }
    void set(boolean value) {
      digitalWrite(this->pinNumber, value);
    }
    void activate() {
      this->set(!this->normally);
    }
    void reset() {
      this->set(this->normally);
    }
    boolean getState() {
      return digitalRead(this->pinNumber);
    }
  private:
    int pinNumber;
    boolean normally;
};

class IntervalOutput {
  public:
    IntervalOutput(Output *output) {
      this->output = output;
    }
    void blink(unsigned int interval, unsigned int duration) {
      if (this->interval == interval && this->duration == duration && this->isActive) {
        return;
      }
      
      this->interval = interval;
      this->duration = duration;
      this->switchTime = 0;
      this->isActive = true;
    }
    void reset() {
      this->output->reset();
      this->switchTime = 0;
      this->isActive = false;
    }
    unsigned int getInterval() {
      return this->interval;
    }
    unsigned int getDuration() {
      return this->duration;
    }
    void update() {
      if (this->isActive) {
        unsigned int intervalTime = millis() - switchTime;
        if (!isOutputActive && (intervalTime < duration || intervalTime >= interval)) {
          this->output->activate();
          isOutputActive = true;
        }
        else if (isOutputActive && intervalTime >= duration) {
          this->output->reset();
          isOutputActive = false;
        }
  
        if (intervalTime >= interval) {
          this->switchTime = millis();
        }
      }
    }
  private:
    Output *output;
    
    unsigned long switchTime = 0;
    unsigned int interval    = 0;
    unsigned int duration    = 0;
    bool isOutputActive      = false;
    bool isActive            = false;
};

class TimedOutput {
  public:
    TimedOutput(Output *output) {
      this->output = output;
    }
    void set(boolean value, unsigned int duration) {
      this->switchTime = millis();
      this->switchDuration = duration;
      this->output->set(value);
    }
    boolean getState() {
      return this->output->getState();
    }
    void update() {
      if (millis() - switchTime >= this->switchDuration) {
        this->output->reset();
      }
    }
  private:
    Output *output;
    
    unsigned long switchTime = 0;
    unsigned int switchDuration = 0;
};

#endif


