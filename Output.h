#ifndef OUTPUT_H
#define OUTPUT_H

class Output {
  public:
    Output(int pinNumber, int normally = LOW) {
      this->pinNumber = pinNumber;
      this->normally = normally;
      pinMode(this->pinNumber, OUTPUT);
    }
    void deactivate() {
      this->set(this->normally);
    }
    boolean isActive() {
      return this->getState() != this->normally;
    }
    virtual int getState() = 0;
    virtual void set(int value) = 0;
    virtual ~Output() {};
  protected:
    int pinNumber;
    int normally;
};

class DigitalOutput : public Output {
  public:
    DigitalOutput(int pinNumber, int active = HIGH) : Output(pinNumber, !active) {
      // cannot call this in the base class :-(
      this->deactivate();
    }
    int getState() {
      return digitalRead(this->pinNumber);
    }
    void toggle(boolean state) {
      if (state && !this->isActive()) {
        this->activate();
      } 
      else if (!state && this->isActive()) {
        this->deactivate();
      }
    }
    void set(int value) {
      digitalWrite(this->pinNumber, value);
    }
    void activate() {
      this->set(!this->normally);
    }
};

class AnalogOutput : public Output {
  public:
    AnalogOutput(int pinNumber, int normally = LOW) : Output(pinNumber, normally) {
      // cannot call this in the base class :-(
      this->deactivate();
    }
    int getState() {
      return analogRead(this->pinNumber);
    }
    void set(int value) {
      analogWrite(this->pinNumber, value);
    }
};

class IntervalOutput {
  public:
    IntervalOutput(DigitalOutput *output) {
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
    void deactivate() {
      this->output->deactivate();
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
          this->output->deactivate();
          isOutputActive = false;
        }
  
        if (intervalTime >= interval) {
          this->switchTime = millis();
        }
      }
    }
  private:
    DigitalOutput *output;
    
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
    void set(int value, unsigned int duration) {
      this->switchTime = millis();
      this->switchDuration = duration;
      this->output->set(value);
    }
    boolean isActive() {
      return this->output->isActive();
    }
    boolean getState() {
      return this->output->getState();
    }
    void update() {
      if (millis() - switchTime >= this->switchDuration) {
        this->output->deactivate();
      }
    }
  private:
    Output *output;
    
    unsigned long switchTime = 0;
    unsigned int switchDuration = 0;
};

#endif


