#ifndef BCM_H
#define BCM_H

class Bcm {
  public:
    Bcm() {}
    boolean isDriverDoorOpen() {
      return this->driverDoorSensor->getState();
    }
    boolean isPassengerDoorOpen() {
      return this->passengerDoorSensor->getState();
    }
    boolean isBackDoorOpen() {
      return this->backDoorSensor->getState();
    }
    boolean isAnyDoorOpen() {
      return this->isDriverDoorOpen() || this->isPassengerDoorOpen() || this->isBackDoorOpen();
    }
    boolean areDoorsLocked() {
      return this->isLocked;
    }
    boolean areDoorsUnlocked() {
      return !this->isLocked;
    }

    boolean isRearFogLightActive() {
      return this->RearFogRelay->getState() == HIGH;
    }
    void lockDoors() {
      this->lockRelay->set(HIGH, 100);
      this->isLocked = true;
    }
    void unlockDoors() {
      this->unlockRelay->set(HIGH, 100);
      this->isLocked = false;
    }
    void openWindows() {
      this->unlockRelay->set(HIGH, 4000);
    }
    void closeWindows() {
      this->lockRelay->set(HIGH, 6000);
    }
    void washHeadlights(unsigned int duration) {
      this->headlightWasherRelay->set(HIGH, duration);
    }
    void updateCan(CAN_message_t canMessage) {
      this->driverDoorSensor->update(canMessage);
      this->passengerDoorSensor->update(canMessage);
    }

    void setRearFogLight(boolean newState) {
      this->RearFogRelay->toggle(newState);
    }

    void update(void (*bcmCallback)(Button *lockButton, Button *unlockButton, Button *headlightWasherButton, Bcm *bcm)) {
      this->lockButton->update();
      this->unlockButton->update();
      
      this->unlockRelay->update();

      this->headlightWasherButton->update();
      this->headlightWasherRelay->update();
      

       if (this->lockButton->wasPressedTimesOrMore(1)) {
         this->isLocked = true;
       } 
       if (this->unlockButton->wasPressedTimesOrMore(1)) {
         this->isLocked = false;
       }

      bcmCallback(this->lockButton, this->unlockButton, this->headlightWasherButton, this);
    }
  private:
    Button *lockButton        = new Button(new DigitalInput(22),700);
    Button *unlockButton      = new Button(new DigitalInput(23),700);
    boolean isLocked          = false;
    TimedOutput *lockRelay    = new TimedOutput(new DigitalOutput(27, HIGH));
    TimedOutput *unlockRelay  = new TimedOutput(new DigitalOutput(28, HIGH));

    Button *headlightWasherButton     = new Button(new DigitalInput(34, 20, LOW, INPUT));
    TimedOutput *headlightWasherRelay = new TimedOutput(new DigitalOutput(40));
  
    CanInput *driverDoorSensor        = new CanInput(0x060D, 0, B00010000);
    CanInput *passengerDoorSensor     = new CanInput(0x060D, 0, B00010000);
    DigitalInput *backDoorSensor      = new DigitalInput(56, 20, LOW, INPUT);
    DigitalOutput *RearFogRelay       = new DigitalOutput(52, HIGH);
};

#endif
