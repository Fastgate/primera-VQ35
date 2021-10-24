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
    void update() {
      Button::update();
      this->statusLedRed->update();
      this->statusLedGreen->update();
    }
  private:
    uint8_t statusLedState         = 0;
    unsigned int statusLedInterval = 0;
    unsigned int statusLedDuration = 0;
    IntervalOutput *statusLedGreen = new IntervalOutput(new DigitalOutput(3, HIGH));
    IntervalOutput *statusLedRed   = new IntervalOutput(new DigitalOutput(4, HIGH));
    
};

class Ecm {
  public:
    Ecm(DigitalInput *ClutchSwitchButton, CanInput *brakeSensor, DigitalInput *pnpSwitch, CanInput *keySensor,Button *bluetoothConnection, Bcm *bcm) {
      this->ClutchSwitchButton  = ClutchSwitchButton;
      this->brakeSensor         = brakeSensor;
      this->pnpSwitch           = pnpSwitch;
      this->bluetoothConnection = bluetoothConnection;
      this->keySensor           = keySensor;
      this->bcm                 = bcm;
    }
    boolean isEngineRunning() {
      return this->engineRunning;
    }
    boolean isbuttonStart() {
      return this->buttonStart;
    }
    
    void startEngine() {
      if (!this->engineRunning && bcm->isNatsRlyActive()) {
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
    
    void setIgnition(uint8_t newState) {
      if (this->ignitionState != newState) { 
        this->bcm->setESD(true);   
        this->acc->toggle(newState >= IGNITION_ACC && !this->crank->isActive());
        this->on->toggle(newState >= IGNITION_ON);
        this->ignitionState = newState;
      }
    }

    void updateCan(CAN_message_t canMessage) {
      this->brakeSensor->updateCan(canMessage);
      this->keySensor->updateCan(canMessage);
    }

    void update() {
      //this->pnpSwitch->getState();
      //this->bluetoothConnection->getState();
      boolean isClutchPressed = this->ClutchSwitchButton->getState();
      boolean isBrakePressed  = this->brakeSensor->getState();
      boolean canStartEngineLocal  = isClutchPressed && this->bcm->isNatsRlyActive();
      boolean canStartEngineRemote = (this->pnpSwitch->getState() || isClutchPressed) && this->bcm->isNatsRlyActive();
      //boolean bluetoothConnection = this->bluetoothConnection->getState();
      
      this->ignitionButton->update();
      this->bluetoothConnection->update();
      this->crankSensor->update();
      this->crank->update();
      this->BtACC->update();
      this->BtIGN->update();
      this->BtStart->update();

      // ignition logic
      if (!this->engineRunning) {

        // Bluetooth ignition button
        if (this->BtStart->isPressed() && canStartEngineRemote) {
          this->startEngine();
          this->buttonStart = false;
        } 
        else if (this->BtIGN->isPressed()) {
          this->setIgnition(IGNITION_ON);
        } 
        else if (this->BtACC->isPressed()) {
          this->setIgnition(IGNITION_ACC);
        } 
        else if (this->BtACC->wasPressedFor(100)) {
          this->setIgnition(IGNITION_OFF);
        }

        // Car ignition button
        if (this->ignitionButton->isPressed()&& this->bcm->areDoorsUnlocked()) {
          // switch engine on
          if (canStartEngineLocal) {
            this->startEngine();
            this->buttonStart = true;
          }
          // toggle through ignition states
          else {
            this->setIgnition(this->ignitionState >= IGNITION_ON ? IGNITION_OFF : this->ignitionState + 1);
          }
        }
      } 
      else {
        // Turning off the Engine
        if (
            (!this->buttonStart  && this->BtIGN->wasPressedFor(100)) || // Bluetooth ignition button turned off
            (this->ignitionButton->isPressed() && isBrakePressed) ||                  // Car ignition button pressed while braking
            this->ignitionButton->wasHeldFor(Ecm::ENGINE_BUTTON_STOP_DURATION)        // Car ignition button held down long
          ) {
            this->bcm->setESD(false);
            stopEngine();
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
      else if (isClutchPressed &&this->bcm->areDoorsUnlocked()) {
        if (isClutchPressed && this->bcm->isNatsRlyActive()) {
          this->ignitionButton->flashStatusLed(IgnitionButton::STATE_GREEN, 1000, 100);
        }
        else {
          if (this->ignitionState >= IGNITION_ACC) {
            this->ignitionButton->setStatusLed(IgnitionButton::STATE_RED);
          }
          else {
            this->ignitionButton->flashStatusLed(IgnitionButton::STATE_RED, 500, 100);
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

      
    }
    uint8_t IGNITION_OFF  = 0;
    uint8_t IGNITION_ACC  = 1;
    uint8_t IGNITION_ON   = 2;

    static const unsigned int DEFROST_DURATION            = 10 * 60 * 1000;
    static const unsigned int CRANK_DURATION              = 700;
    static const unsigned int ENGINE_BUTTON_STOP_DURATION = 3 * 1000;
  private:
    uint8_t ignitionState           = IGNITION_OFF;
    boolean engineRunning           = false;
    boolean buttonStart           = false;
    unsigned long engineDefrostTime = 0;

    DigitalInput *pnpSwitch;
    DigitalInput *ClutchSwitchButton;
    Button *bluetoothConnection;
    CanInput *brakeSensor;
    CanInput *keySensor;
    Bcm *bcm;
        
    IgnitionButton *ignitionButton        = new IgnitionButton(new DigitalInput(5, 20, HIGH, INPUT));
    DigitalOutput *acc                    = new DigitalOutput(31);
    DigitalOutput *on                     = new DigitalOutput(30);
    TimedOutput *crank                    = new TimedOutput(new DigitalOutput(32));
    Button *crankSensor                   = new Button(new DigitalInput(32, 20, HIGH, OUTPUT));

    DigitalOutput *ButtonIllu             = new DigitalOutput(2, LOW);
    DigitalInput  *oilPressureSwitch      = new DigitalInput(25, 20, HIGH, INPUT);
    
    Button *BtStart                       = new Button(new DigitalInput(37, 20, HIGH, INPUT));
    Button *BtIGN                         = new Button(new DigitalInput(36, 20, HIGH, INPUT));
    Button *BtACC                         = new Button(new DigitalInput(35, 20, HIGH, INPUT));
};

#endif
