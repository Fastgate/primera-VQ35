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
      return unlockRelay->getState() == HIGH;
    }
    boolean areDoorsUnlocked() {
      return !this->areDoorsLocked();
    }
    void lockDoors() {
      this->lockRelay->set(HIGH, 100);
    }
    void unlockDoors() {
      this->unlockRelay->set(HIGH, 100);
    }
    void openWindows() {
      this->unlockRelay->set(HIGH, 4000);
    }
    void closeWindows() {
      this->lockRelay->set(HIGH, 6000);
    }
    void update(void (*bcmCallback)(Button *lockButton, Button *unlockButton, Bcm *bcm)) {
      this->lockButton->update();
      this->unlockButton->update();
      this->lockRelay->update();
      this->unlockRelay->update();

      bcmCallback(this->lockButton, this->unlockButton, this);
    }
  private:
    Button *lockButton = new Button(new DigitalInput(22),700);
    Button *unlockButton = new Button(new DigitalInput(23),700);
    TimedOutput *lockRelay = new TimedOutput(new DigitalOutput(27, HIGH));
    TimedOutput *unlockRelay = new TimedOutput(new DigitalOutput(28, HIGH));
  
    DigitalInput *driverDoorSensor = new DigitalInput(49, 20, LOW, INPUT_PULLUP);
    DigitalInput *passengerDoorSensor = new DigitalInput(51, 20, LOW, INPUT_PULLUP);
    DigitalInput *backDoorSensor = new DigitalInput(56, 20, LOW, INPUT_PULLUP);
};

#endif
