#ifndef BCM_H
#define BCM_H

class Bcm {
  public:
    Bcm() {}
    boolean isFLDoorOpen() {
      return this->FLDoorSensor->getState();
    }
    boolean isFRDoorOpen() {
      return this->FRDoorSensor->getState();
    }
    boolean isRLDoorOpen() {
      return this->RLDoorSensor->getState();
    }
    boolean isRRDoorOpen() {
      return this->RRDoorSensor->getState();
    }
    boolean isBackDoorOpen() {
      return this->backDoorSensor->getState();
    }
    boolean isAnyDoorOpen() {
      return this->isFLDoorOpen() || this->isFRDoorOpen() || this->isRLDoorOpen() || this->isRRDoorOpen() || this->isBackDoorOpen();
    }
    boolean areDoorsLocked() {
      return this->isLocked;
    }
    boolean areDoorsUnlocked() {
      return !this->isLocked;
    }
    void toggleRearFogLight() {
      this->RearFogRelay->toggle(!this->RearFogRelay->getState());
    }
    boolean isRearFogLightActive() {
      return this->RearFogRelay->getState() == HIGH;
    }
    void lockDoors() {
      this->lockRelay->set(HIGH, 100);
      this->isLocked = true;
    }
    void unlockDoors() {
      this->unlockRelay->set(HIGH, 1000);
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
      this->FLDoorSensor->update(canMessage);
      this->FRDoorSensor->update(canMessage);
      this->RLDoorSensor->update(canMessage);
      this->RRDoorSensor->update(canMessage);
           
    }
    void update(void (*bcmCallback)(Button *lockButton, Button *unlockButton, Button *headlightWasherButton,DigitalInput *blueSmirf , Bcm *bcm)) {
      this->lockButton->update();
      this->unlockButton->update();
      this->lockRelay->update();
      this->unlockRelay->update();
      this->blueSmirf->getState();

      this->headlightWasherButton->update();
      this->headlightWasherRelay->update();

      if (this->lockButton->wasPressedTimesOrMore(1)) {
        this->isLocked = true;
      } 
      if (this->unlockButton->wasPressedTimesOrMore(1)) {
        this->isLocked = false;
      }
      if (this->blueSmirf->getState()){
        this->isLocked = true;
      }else{
        this->isLocked = false;
      }

      bcmCallback(this->lockButton, this->unlockButton, this->headlightWasherButton, this->blueSmirf, this);
    }
  private:
    Button *lockButton        = new Button(new DigitalInput(23),700);
    Button *unlockButton      = new Button(new DigitalInput(22),700);
    boolean isLocked          = false;
    TimedOutput *lockRelay    = new TimedOutput(new DigitalOutput(27, HIGH));
    TimedOutput *unlockRelay  = new TimedOutput(new DigitalOutput(28, HIGH));

    Button *headlightWasherButton     = new Button(new DigitalInput(34, 20, LOW, INPUT));
    TimedOutput *headlightWasherRelay = new TimedOutput(new DigitalOutput(40));
    DigitalInput *blueSmirf           = new DigitalInput(48, 20, HIGH, INPUT); // Bluetooth for Keyless Entry
    // Bluesmirf Serial1 connection
  
    CanInput *FLDoorSensor            = new CanInput(0x060D, 0, B00001000);
    CanInput *FRDoorSensor            = new CanInput(0x060D, 0, B00010000);
    CanInput *RLDoorSensor            = new CanInput(0x060D, 0, B00100000);
    CanInput *RRDoorSensor            = new CanInput(0x060D, 0, B01000000);
    CanInput *backDoorSensor          = new CanInput(0x0358, 2, B00000001);
    DigitalOutput *RearFogRelay       = new DigitalOutput(52, HIGH);
};

#endif
