#ifndef BCM_H
#define BCM_H

#include <Arduino.h>
#include <arduinoIO.h>

class Bcm {
  public:
    Bcm() {}
    
  //////////////////////////////////////////////
 ///////////////// DOOR SWITCH ////////////////
//////////////////////////////////////////////    
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
  //////////////////////////////////////////////////////  
 ////////////////// REAR FOG //////////////////////////
////////////////////////////////////////////////////// 
    
    void toggleRearFogLight() {
      this->RearFogRelay->toggle(!this->RearFogRelay->getState());
    }
    boolean isRearFogLightActive() {
      return this->RearFogRelay->getState() == HIGH;
      //message0358.buf[4] = B10000000; // Rear Fog Lamp
      //can.write(message0358);
    }

    boolean isNatsRlyActive() {
      return this->NatsRly->getState() == HIGH;
    }

    void setNatsRly(boolean newState) {
      this->NatsRly->toggle(newState);
    }
    boolean isRunningLightSensorActive() {
      return this->ButtonIllumination->getState() == HIGH;
    }

    void setButtonIllumination(boolean newState) {
      this->ButtonIllumination->toggle(newState);
    }

    boolean isEsdActive() {
      return this->ESD->getState() == HIGH;
    }

    void setESD(boolean newState) {
      this->ESD->toggle(newState);
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
  //////////////////////////////////////////////////   
 ////////// KEYLESS ENTRY ///////////////////////// 
/////////////////////////////////////////////////

       
    void washHeadlights(unsigned int duration) {
      this->headlightWasherRelay->set(HIGH, duration);
    }

    void updateCan(CAN_message_t canMessage) {
      this->FLDoorSensor->updateCan(canMessage);
      this->FRDoorSensor->updateCan(canMessage);
      this->RLDoorSensor->updateCan(canMessage);
      this->RRDoorSensor->updateCan(canMessage);
           
    }
    
    void update(void (*bcmCallback)(Button *BtLockButton, Button *BtUnlockButton, Button *lockButton, Button *unlockButton, Button *headlightWasherButton, Bcm *bcm)) {
      this->lockButton->update();
      this->unlockButton->update();
      this->BtLockButton->update();
      this->BtUnlockButton->update();
      this->lockRelay->update();
      this->unlockRelay->update();
           
      this->headlightWasherButton->update();
      this->headlightWasherRelay->update();
     
      if (this->lockButton->wasPressedTimesOrMore(1) || this->BtLockButton->wasPressedTimesOrMore(1)) {
        this->isLocked = true;
      } 
      if (this->unlockButton->wasPressedTimesOrMore(1) || this->BtUnlockButton->wasPressedTimesOrMore(1)) {
        this->isLocked = false;
      }
       

      bcmCallback(this->BtLockButton, this->BtUnlockButton, this->lockButton, this->unlockButton, this->headlightWasherButton, this);
    }

  private:
    Button *lockButton                    = new Button(new DigitalInput(34),700);
    Button *unlockButton                  = new Button(new DigitalInput(29),700);
    boolean isLocked                      = false;
    TimedOutput *lockRelay                = new TimedOutput(new DigitalOutput(7, HIGH));
    TimedOutput *unlockRelay              = new TimedOutput(new DigitalOutput(8, HIGH));
    
    CanInput *FLDoorSensor                = new CanInput(0x060D, 0, B00001000);
    CanInput *FRDoorSensor                = new CanInput(0x060D, 0, B00010000);
    CanInput *RLDoorSensor                = new CanInput(0x060D, 0, B00100000);
    CanInput *RRDoorSensor                = new CanInput(0x060D, 0, B01000000);
    CanInput *backDoorSensor              = new CanInput(0x0358, 2, B00000001);
    Button        *headlightWasherButton  = new Button(new DigitalInput(12, 20, HIGH, INPUT_PULLUP));
    TimedOutput   *headlightWasherRelay   = new TimedOutput(new DigitalOutput(9));
    DigitalOutput *RearFogRelay           = new DigitalOutput(6, HIGH);
    DigitalOutput *ESD                    = new DigitalOutput(15, LOW);
    DigitalOutput *NatsRly                = new DigitalOutput(14, HIGH);
    DigitalOutput *ButtonIllumination     = new DigitalOutput(2, HIGH);
    
// **************************** Bluetooth Inputs ***************************************

    Button *BtLockButton                  = new Button(new DigitalInput(40),2000);
    Button *BtUnlockButton                = new Button(new DigitalInput(41),3000);
    DigitalInput  *BtTrunk                = new DigitalInput(21, 20, HIGH, INPUT);
    DigitalInput  *BtESD                  = new DigitalInput(38, 20, HIGH, INPUT); // EngineStartDeactivation Signal from BT Modul
    DigitalInput  *BtNatsInput            = new DigitalInput(39, 20, HIGH, INPUT); // Input Signal from BT Modul
        
};

#endif
