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
    
    void update(void (*bcmCallback)(Button *lockButton, Button *unlockButton, Button *headlightWasherButton, Bcm *bcm)) {
      this->lockButton->update();
      this->unlockButton->update();
      this->lockRelay->update();
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
    //DigitalInput  *ClutchSwitch         = new DigitalInput(17, 20, HIGH, INPUT);
    //DigitalOutput *WindowDown           = new DigitalOutput(6, HIGH);
    //DigitalOutput *WindowUp             = new DigitalOutput(5, HIGH);
    Button        *headlightWasherButton  = new Button(new DigitalInput(12, 20, HIGH, INPUT_PULLUP));
    TimedOutput   *headlightWasherRelay   = new TimedOutput(new DigitalOutput(9));
    DigitalOutput *RearFogRelay           = new DigitalOutput(6, HIGH);
    
// **************************** Bluetooth Inputs ***************************************

    DigitalInput  *BtUnlock               = new DigitalInput(41, 20, HIGH, INPUT);
    DigitalInput  *BtLock                 = new DigitalInput(40, 20, HIGH, INPUT);
    DigitalInput  *BtTrunk                = new DigitalInput(21, 20, HIGH, INPUT);
    
    
};

#endif
