#ifndef ECM_H
#define ECM_H

#include <arduinoIO.h>

class IgnitionButton : public Button {
  public:
    static const uint8_t STATE_OFF     = 0;
    static const uint8_t STATE_RED     = 1;
    static const uint8_t STATE_GREEN   = 2;
    static const uint8_t STATE_YELLOW  = 3;
  
    IgnitionButton(Input *input): Button(input, 0) {}
    void flashStatusLed(uint8_t newState, unsigned int newInterval, unsigned int newDuration) {
      if (newState == this->statusLedState && newInterval == this->statusLedInterval && newDuration == this->statusLedDuration) {
        return;
      }
      
      switch (newState) {
        case IgnitionButton::STATE_OFF:
          this->statusLedRed->deactivate();
          this->statusLedGreen->deactivate();
          break;
        case IgnitionButton::STATE_RED:
          this->statusLedRed->blink(newInterval, newDuration);
          this->statusLedGreen->deactivate();
          break;
        case IgnitionButton::STATE_GREEN:
          this->statusLedRed->deactivate();
          this->statusLedGreen->blink(newInterval, newDuration);
          break;
        case IgnitionButton::STATE_YELLOW:
          this->statusLedRed->blink(newInterval, newDuration);
          this->statusLedGreen->blink(newInterval, newDuration);
          break;
      }

      this->statusLedState    = newState;
      this->statusLedInterval = newInterval;
      this->statusLedDuration = newDuration;
    }
    void setStatusLed(uint8_t newState) {
      this->flashStatusLed(newState, 0, 0);
    }
    void setAccLed(boolean newState) {
      this->accLed->toggle(newState);
    }
    void setOnLed(boolean newState) {
      this->onLed->toggle(newState);
    }
    void update() {
      Button::update();
      this->statusLedRed->update();
      this->statusLedGreen->update();
    }
  private:
    uint8_t statusLedState         = 0;
    unsigned int statusLedInterval = 0;
    unsigned int statusLedDuration = 0;
    IntervalOutput *statusLedGreen = new IntervalOutput(new DigitalOutput(20, LOW));
    IntervalOutput *statusLedRed   = new IntervalOutput(new DigitalOutput(24, LOW));
    DigitalOutput *accLed = new DigitalOutput(5, LOW);
    DigitalOutput *onLed  = new DigitalOutput(2, LOW);
};

class Ecm {
  public:
    Ecm(DigitalInput *clutchSensor, CanInput *brakeSensor, DigitalInput *neutralSensor, DigitalInput *keySensor, Bcm *bcm) {
      this->clutchSensor  = clutchSensor;
      this->brakeSensor   = brakeSensor;
      this->neutralSensor = neutralSensor;
      this->keySensor     = keySensor;
      this->bcm           = bcm;
    }
    boolean isEngineRunning() {
      return this->engineRunning;
    }
    boolean isEngineDefrosting() {
      return this->engineDefrostTime > 0;
    }
    void startEngine() {
      if (!this->engineRunning && (this->clutchSensor->getState() || this->neutralSensor->getState())) {
        this->engineDefrostTime = 0;
        this->setIgnition(IGNITION_ON);
        this->engineRunning = true;
        this->acc->deactivate();
        this->crank->set(HIGH, CRANK_DURATION);
      }
    }

     void startEngineRemote() {
      if (!this->engineRunning) {
        this->engineDefrostTime = 0;
        this->setIgnition(IGNITION_ON);
        this->engineRunning = true;
        this->acc->deactivate();
        this->crank->set(HIGH, CRANK_DURATION);
      }
    }
    void stopEngine() {
      if (this->engineRunning) {
        this->setIgnition(IGNITION_OFF);
        this->engineRunning = false;
      }
    }
    void startEngineDefrost() {
      this->startEngineRemote();
      this->engineDefrostTime = millis();
    }
    void stopEngineDefrost() {
      this->engineDefrostTime = 0;
    }
    void setIgnition(uint8_t newState) {
      if (this->ignitionState != newState) {   
        this->acc->toggle(newState >= IGNITION_ACC && !this->crank->isActive());
        this->ignitionButton->setAccLed(newState == IGNITION_ACC);
        this->natsLed->toggle(newState >= IGNITION_ACC);
    
        this->nats->toggle(newState >= IGNITION_ON);
        this->on->toggle(newState >= IGNITION_ON);
        this->ignitionButton->setOnLed(newState >= IGNITION_ON);

        this->ignitionState = newState;
      }
    }

     void updateCan(CAN_message_t canMessage) {
      this->brakeSensor->update(canMessage);
      }

    void update() {
      this->neutralSensor->getState();
      boolean isKeyInserted   = this->keySensor->getState();
      boolean isClutchPressed = this->clutchSensor->getState();
      boolean isBrakePressed  = this->brakeSensor->getState();
      
      this->ignitionButton->update();
      this->crankSensor->update();
      this->crank->update();

      // key signal
      this->key->toggle(isKeyInserted || this->ignitionState >= IGNITION_ON);

      // ignition button
      if (!this->engineRunning) {
        if (this->ignitionButton->isPressed() && isKeyInserted) {
          // switch engine on
          if (isClutchPressed) {
            this->startEngine();
          }
          // toggle through ignition states
          else {
            this->setIgnition(this->ignitionState >= IGNITION_ON ? IGNITION_OFF : this->ignitionState + 1);
          }
        }
      }
      else {
        if ((this->ignitionButton->isPressed() && isBrakePressed) || this->ignitionButton->wasHeldFor(Ecm::ENGINE_BUTTON_STOP_DURATION)) {
          stopEngine();
        }
      }

      if (this->ignitionButton->isPressed() && isKeyInserted) {
         //Keyboard.press(KEY_SYSTEM_WAKE_UP);  
         //Keyboard.release(KEY_SYSTEM_WAKE_UP);
        }else {
          if(!isKeyInserted){
            //Keyboard.press(KEY_SYSTEM_POWER_DOWN);   // ENGINE_BUTTON
            //Keyboard.release(KEY_SYSTEM_POWER_DOWN);
            }
          }

      // ignition light
      if (this->engineRunning) {
        if (this->crankSensor->isHeld()) {
          this->ignitionButton->flashStatusLed(IgnitionButton::STATE_YELLOW, 100, 50);
        }
        else {
          this->ignitionButton->setStatusLed(IgnitionButton::STATE_GREEN);
        }
      }
      else if (isKeyInserted) {
        if (isClutchPressed) {
          this->ignitionButton->flashStatusLed(IgnitionButton::STATE_GREEN, 1000, 100);
        }
        else {
          if (this->ignitionState >= IGNITION_ACC) {
            this->ignitionButton->setStatusLed(IgnitionButton::STATE_RED);
          }
          else {
            this->ignitionButton->flashStatusLed(IgnitionButton::STATE_RED, 2000, 100);
          }
        }
      }
      else if (this->bcm->areDoorsUnlocked()) {
        this->ignitionButton->setStatusLed(IgnitionButton::STATE_RED);
      }
      else {
        this->ignitionButton->setStatusLed(IgnitionButton::STATE_OFF);
      }
    
      // reenable ACC after crank has stopped
      if (this->crankSensor->wasPressedTimes(1)) {
        this->acc->toggle(this->ignitionState >= IGNITION_ACC);
      }

      // defrost feature
      if (isKeyInserted && this->isEngineDefrosting()) {
        this->stopEngineDefrost();
      }

      
      if (this->engineDefrostTime > 0 && millis() - this->engineDefrostTime >= DEFROST_DURATION) {
        this->stopEngineDefrost();
        if (!isKeyInserted) {
          this->stopEngine();
        }
      }
    }
    uint8_t IGNITION_OFF  = 0;
    uint8_t IGNITION_ACC  = 1;
    uint8_t IGNITION_ON   = 2;

    static const unsigned int DEFROST_DURATION            = 10 * 60 * 1000;
    static const unsigned int CRANK_DURATION              = 700;
    static const unsigned int ENGINE_BUTTON_STOP_DURATION = 3 * 1000;
  private:
    uint8_t ignitionState           = IGNITION_OFF;
    boolean engineRunning         = false;
    unsigned long engineDefrostTime = 0;

    DigitalInput *clutchSensor;
    CanInput *brakeSensor;
    DigitalInput *neutralSensor;
    DigitalInput *keySensor;
    Bcm *bcm;
        
    IgnitionButton *ignitionButton = new IgnitionButton(new DigitalInput(39, 20, HIGH, INPUT));
    DigitalOutput *acc  = new DigitalOutput(42);
    DigitalOutput *on   = new DigitalOutput(44);
    DigitalOutput *nats = new DigitalOutput(53);
    DigitalOutput *key  = new DigitalOutput(43);
    TimedOutput *crank  = new TimedOutput(new DigitalOutput(41));
    Button *crankSensor = new Button(new DigitalInput(41, 20, HIGH, OUTPUT));
        

    DigitalOutput *natsLed  = new DigitalOutput(57, LOW);
};

#endif
