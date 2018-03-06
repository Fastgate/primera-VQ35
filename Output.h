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

class TimedOutput {
  public:
    TimedOutput(Output *output) {
      this->output = output;
      this->output->reset();
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


