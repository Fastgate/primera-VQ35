#ifndef BUTTON_H
#define BUTTON_H

class Sensor {
  public: 
    virtual boolean getState() = 0;
    virtual ~Sensor() {}
};

class DigitalSensor : public Sensor {
  public:
    DigitalSensor(int pinNumber, unsigned int debounceDuration = 20, boolean compareMode = HIGH, int mode = INPUT) {
      pinMode(pinNumber, mode);
      this->pinNumber = pinNumber;
      this->compareMode = compareMode;
      this->debounceDuration = debounceDuration;
    }
    virtual boolean getState() { 
      boolean newPinValue = digitalRead(this->pinNumber);
      if (newPinValue != this->desiredPinValue) {
        this->debounceTime = millis();
        this->desiredPinValue = newPinValue;
      }
    
      if (millis() - this->debounceTime > this->debounceDuration) {
        this->pinValue = this->desiredPinValue;
      }
    
      return this->pinValue == this->compareMode;
    }
  private:
    int pinNumber;
    int pinValue;
    int desiredPinValue;
    boolean compareMode;
    unsigned int debounceDuration;
    unsigned long debounceTime = 0;
};

class Button {
  public:
    Button(Sensor *sensor, unsigned int pushDurationLimit = 300) {
      _sensor = sensor;
      _pushDurationLimit = pushDurationLimit;
    }
    virtual ~Button() {}

    boolean isPressed() {
      return _buttonState == Button::STATE_PRESSED;
    }
    boolean wasPressedFor(unsigned int duration) {
      return _buttonState == Button::STATE_RELEASED &&
        millis() - _pressTime >= duration;
    }
    boolean wasPressedTimes(unsigned int count) {
      return _buttonState == Button::STATE_RELEASED &&
        _pressCounter == count;
    }
    boolean wasPressedTimesOrMore(unsigned int count) {
      return _buttonState == Button::STATE_RELEASED &&
        _pressCounter >= count;
    }
    /* FIXME: not working
    boolean wasHeldFor(unsigned int duration, unsigned int repeatTime = 0) {
      unsigned long currentTime = millis();
      int a = (int(currentTime - _stateTime) - duration + int(repeatTime * 0.5)) / repeatTime;
      if(a < 0) {
        a = 0;
      }
      unsigned long delayedPressTime = duration + _stateTime + repeatTime * a;
  
      return _buttonState == Button::STATE_HELD &&
        (_stateTime >= duration && (repeatTime == 0 || currentTime >= delayedPressTime));
    }
    */

    void update() {
      unsigned long updateTime = millis();
      boolean newSensorState = _sensor->getState();
      
      if (_sensorState != newSensorState) {
        int idleState = updateTime - _pressTime < _pushDurationLimit ? Button::STATE_WAITING : Button::STATE_RELEASED;
        _buttonState = newSensorState ? Button::STATE_PRESSED : idleState;
        _sensorState = newSensorState;
        _stateTime = updateTime;
        if (_buttonState == Button::STATE_PRESSED) {
          _pressTime = updateTime;
          _pressCounter++;
        }
      }
      else {
        if (_buttonState == Button::STATE_IDLE) {
          _pressCounter = 0;
        }
        
        if (_buttonState == Button::STATE_WAITING) {
          _buttonState = updateTime - _pressTime < _pushDurationLimit ? Button::STATE_WAITING : Button::STATE_RELEASED;
        }
        else {
          _buttonState = newSensorState ? Button::STATE_HELD : STATE_IDLE;  
        }
      }
    }

  private:
    static const int STATE_IDLE     = 0;
    static const int STATE_PRESSED  = 1;
    static const int STATE_HELD     = 2;
    static const int STATE_RELEASED = 3;
    static const int STATE_WAITING  = 4;
  
    Sensor *_sensor;
    boolean _sensorState = false;

    int _buttonState = STATE_IDLE;
    
    unsigned long _stateTime = 0;
    unsigned int _pressCounter = 0;
    unsigned int _pushDurationLimit = 0;
    unsigned int _pressTime = 0;

    int getState(boolean pinValue, unsigned long updateTime);
};

#endif
