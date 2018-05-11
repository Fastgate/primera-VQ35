#include <SPI.h>
#include "Button.h"
#include "Output.h"
#include "Binary.h"
#include "Serial.h"
#include "Mmi.h"
#include "Hvac.h"
#include "Sleep.h"

#include "Acm.h"
#include "Bcm.h"

  /////////////
 // HELPERS // 
/////////////

#define min(X, Y)  ((X) < (Y) ? (X) : (Y))
#define max(X, Y)  ((X) > (Y) ? (X) : (Y))


  /////////////////////
 // MMI DEFINITIONS // 
/////////////////////

// Buttons
Mmi mmi(&Serial3, SERIAL_8N2_RXINV, 16, 17, 2);
MmiButton *mmiBigWheelButton    = mmi.createButton(0x01);
MmiButton *mmiMediaButton       = mmi.createButton(0x02);
MmiButton *mmiNameButton        = mmi.createButton(0x03);
MmiButton *mmiTelButton         = mmi.createButton(0x04);
MmiButton *mmiNavButton         = mmi.createButton(0x05);
MmiButton *mmiInfoButton        = mmi.createButton(0x06);
MmiButton *mmiCarButton         = mmi.createButton(0x07);
MmiButton *mmiSetupButton       = mmi.createButton(0x08);
MmiButton *mmiTopLeftButton     = mmi.createButton(0x0A);
MmiButton *mmiBottomLeftButton  = mmi.createButton(0x0B);
MmiButton *mmiPreviousButton    = mmi.createButton(0x0C);
MmiButton *mmiTopRightButton    = mmi.createButton(0x0D);
MmiButton *mmiBottomRightButton = mmi.createButton(0x0E);
MmiButton *mmiReturnButton      = mmi.createButton(0x0F);
MmiButton *mmiNextButton        = mmi.createButton(0x10);
MmiButton *mmiRadioButton       = mmi.createButton(0x18);
MmiButton *mmiSmallWheelButton  = mmi.createButton(0x38);

// Wheels
MmiWheel *mmiSmallWheel = mmi.createWheel(0x40);
MmiWheel *mmiBigWheel   = mmi.createWheel(0x50);

// Lights
MmiLight mmiMediaLight(0x02, &mmi);
MmiLight mmiNameLight(0x03, &mmi);
MmiLight mmiTelLight(0x04, &mmi);
MmiLight mmiNavLight(0x05, &mmi);
MmiLight mmiInfoLight(0x06, &mmi);
MmiLight mmiCarLight(0x07, &mmi);
MmiLight mmiSetupLight(0x08, &mmi);
MmiLight mmiTopLeftLight(0x0A, &mmi);
MmiLight mmiBottomLeftLight(0x0B, &mmi);
MmiLight mmiTopRightLight(0x0D, &mmi);
MmiLight mmiBottomRightLight(0x0E, &mmi);
MmiLight mmiRadioLight(0x18, &mmi);


  //////////////////////////////
 // ILLUMINATION DEFINITIONS // 
//////////////////////////////

DigitalSensor illuminationSensor(37, 20, LOW, INPUT_PULLUP);
Button illuminationDimUpButton(new DigitalSensor(45, 20, LOW, INPUT_PULLUP), 0);
Button illuminationDimDownButton(new DigitalSensor(46, 20, LOW, INPUT_PULLUP), 0);
AnalogOutput illuminationOutput(21);

uint8_t desiredIlluminationLevel = 0xFF / 2;
uint8_t illuminationLevel = 0x00;
bool illuminationState = false;


  //////////////////////////
 // IGNITION DEFINITIONS // 
//////////////////////////

#define IGNITION_OFF  0
#define IGNITION_ACC  1
#define IGNITION_ON   2

#define CRANK_DURATION 600
#define ENGINE_BUTTON_STOP_DURATION 3000

Button ignitionButton(new DigitalSensor(39, 20, HIGH, INPUT), 0);
Button crankSensor(new DigitalSensor(41, 20, HIGH, OUTPUT));
DigitalSensor clutchSensor(16, 20, HIGH, INPUT);
DigitalSensor brakeSensor(17, 20, HIGH, INPUT);
DigitalSensor neutralSensor(36, 20, HIGH, INPUT);
DigitalSensor keySensor(6, 20, HIGH, INPUT);

struct {
  uint8_t state = IGNITION_OFF;
  bool engine   = false;
  bool crank    = false;
} ignition;

struct {
  uint8_t state         = 0;
  IntervalOutput *green = new IntervalOutput(new DigitalOutput(20, LOW));
  IntervalOutput *red   = new IntervalOutput(new DigitalOutput(24, LOW));
  DigitalOutput *acc    = new DigitalOutput(5, LOW);
  DigitalOutput *on     = new DigitalOutput(2, LOW);
  DigitalOutput *nats   = new DigitalOutput(57, LOW);
} ignitionLights;

struct {
  DigitalOutput *acc  = new DigitalOutput(42);
  DigitalOutput *on   = new DigitalOutput(44);
  DigitalOutput *key  = new DigitalOutput(43);
  DigitalOutput *nats = new DigitalOutput(53);
  TimedOutput *crank  = new TimedOutput(new DigitalOutput(41));
} ignitionOutputs;


  //////////////////////////////////
 // HEADLIGHT WASHER DEFINITIONS // 
//////////////////////////////////

Button headlightWasherButton(new DigitalSensor(34, 20, HIGH, INPUT_PULLUP));
TimedOutput headlightWasherRelay(new DigitalOutput(40));


  /////////////////////////////
 // STEERING WHEEL CONTROLS // 
/////////////////////////////

Button swcVolumeUpButton(new AnalogSensor(A11, 28, 34), 0);
Button swcVolumeDownButton(new AnalogSensor(A10, 28, 34), 0);
Button swcPhoneButton(new AnalogSensor(A11, 10, 16), 0);
Button swcVoiceButton(new AnalogSensor(A10, 10, 16), 0);
Button swcSeekUpButton(new AnalogSensor(A11, 15, 20), 0);
Button swcSeekDownButton(new AnalogSensor(A10, 15, 20), 0);


//************************** Primera STW Inputs *******************************

int RevGear = 38;           // Rückwärtsgang  OK
int RevGear_Stat = 0;


  /////////////////////
 // BCM DEFINITIONS //
/////////////////////

Bcm bcm;


  ///////////////////////
 // SLEEP DEFINITIONS //
///////////////////////

Sleep sleep(13, 10 * 60 * 1000);


  /////////////////////
 // ACM DEFINITIONS // 
/////////////////////

Acm acm;


  //////////////////////
 // HVAC DEFINITIONS // 
//////////////////////

Hvac hvac;


  ////////////////////////
 // SERIAL DEFINITIONS //
////////////////////////

SerialPacket statusInitSuccess(0x61, 0x01);
SerialPacket statusInitError(0x65, 0x01);
SerialDataPacket<unsigned long> baudRateChange(0x65, 0x01);

SerialReader serialReader(128);


  //////////////////
 // SKETCH SETUP // 
//////////////////

void setup() {
  Serial.begin(115200);
  statusInitSuccess.serialize(Serial);
    
  // *********************** Primera STW Inputs ************************
  
  pinMode(RevGear, INPUT_PULLUP);
  digitalWrite(RevGear, HIGH); 
}


  /////////////////
 // SKETCH LOOP // 
/////////////////

void loop() {  
  updateMmi();
  updateIllumination();

  updateIgnition();

  updateHeadlightWasher();

  updateSwc();

  bcm.update(updateBcm);

  updateHvac();

  updateSleep();
}


  //////////////////
 // SERIAL EVENT // 
//////////////////

void serialEvent() {
  serialReader.read(Serial, readSerial);
}

void readSerial(uint8_t type, uint8_t id, BinaryBuffer *payloadBuffer) {
  switch (type) {
    case 0x61:
      switch (id) {
        case 0x72: { // set baud rate
          BinaryData::LongResult result = payloadBuffer->readLong();
          if (result.state == BinaryData::OK) {
            baudRateChange.payload(htonl(result.data));
            baudRateChange.serialize(Serial);
            Serial.flush();
            Serial.end();
            Serial.begin(result.data);
            while (Serial.available()) {
              Serial.read();
            }
          }
          break;
        }
      }
      break;
    case 0x63:
      hvac.write(id, payloadBuffer);
      break;
  }
}


  ///////////////////
 // MMI FUNCTIONS // 
///////////////////

void updateMmi() {
  mmi.update(mmiEvent);
  
  if (mmiNavButton->wasPressedTimes(1)) {
    mmiNavLight.toggle();
  }
  if (mmiInfoButton->wasPressedTimes(1)) {
    mmiInfoLight.toggle();
  }
  if (mmiCarButton->wasPressedTimes(1)) {
    mmiCarLight.toggle();
  }
  if (mmiSetupButton->wasPressedTimes(1)) {
    mmiSetupLight.toggle();
  }
  if (mmiRadioButton->wasPressedTimes(1)) {
    mmiRadioLight.toggle();
  }
  if (mmiMediaButton->wasPressedTimes(1)) {
    mmiMediaLight.toggle();
  }
  if (mmiNameButton->wasPressedTimes(1)) {
    mmiNameLight.toggle();
  }
  if (mmiTelButton->wasPressedTimes(1)) {
    mmiTelLight.toggle();
  }
  if (mmiTopLeftButton->wasPressedTimes(1)) {
    mmiTopLeftLight.toggle();
  }
  if (mmiTopRightButton->wasPressedTimes(1)) {
    mmiTopRightLight.toggle();
  }
  if (mmiBottomLeftButton->wasPressedTimes(1)) {
    mmiBottomLeftLight.toggle();
  }
  if (mmiBottomRightButton->wasPressedTimes(1)) {
    mmiBottomRightLight.toggle();
  }
  if (mmiPreviousButton->wasPressedTimes(1)) {
    Keyboard.press(KEY_MEDIA_PREV_TRACK);
    Keyboard.release(KEY_MEDIA_PREV_TRACK);
  }
  if (mmiReturnButton->wasPressedTimes(1)) {
    Keyboard.press(KEY_ESC);
    Keyboard.release(KEY_ESC);
  }
  if (mmiNextButton->wasPressedTimes(1)) {
    Keyboard.press(KEY_MEDIA_NEXT_TRACK);
    Keyboard.release(KEY_MEDIA_NEXT_TRACK);
  }
  if (mmiNextButton->wasPressedTimes(2)) {
    Keyboard.press(KEY_MEDIA_PLAY_SKIP);
    Keyboard.release(KEY_MEDIA_PLAY_SKIP);
  }
  
  if (mmiBigWheel->wasTurned()) {
    if (mmiBigWheel->getAmount() < 0) {
      Keyboard.press(MODIFIERKEY_SHIFT);
      Keyboard.press(KEY_TAB);    
      Keyboard.release(KEY_TAB);
      Keyboard.release(MODIFIERKEY_SHIFT);    
    }
    else {
      Keyboard.press(KEY_TAB);    
      Keyboard.release(KEY_TAB);
    }
  }
  if (mmiBigWheelButton->wasPressedTimes(1)) {
    Keyboard.press(KEY_MEDIA_PLAY_PAUSE);
    Keyboard.release(KEY_MEDIA_PLAY_PAUSE);     
  }
  
  if (mmiSmallWheel->wasTurned()) {
    if (mmiSmallWheel->getAmount() < 0) {
      Keyboard.press(KEY_MEDIA_VOLUME_DEC);
      Keyboard.release(KEY_MEDIA_VOLUME_DEC);
    }
    else {
      Keyboard.press(KEY_MEDIA_VOLUME_INC);
      Keyboard.release(KEY_MEDIA_VOLUME_INC);
    }
  }
  if (mmiSmallWheelButton->wasPressedTimes(1)) {
    Keyboard.press(KEY_MEDIA_MUTE);
    Keyboard.release(KEY_MEDIA_MUTE);
  }
}

void mmiEvent(uint8_t code) {
  if (code == 0xff || code == 0x38) {
    mmi.enableKeys();
  }
  else if (code == 0x35) {
    mmi.setIllumination(illuminationLevel);
    mmi.setHighlightLevel(0x99);
  }
}


  ////////////////////////////
 // ILLUMINATION FUNCTIONS // 
////////////////////////////

void updateIllumination() {
  illuminationDimUpButton.update();
  illuminationDimDownButton.update();
  
  if (illuminationDimUpButton.isPressed() || illuminationDimUpButton.wasHeldFor(500, 200)) {
    desiredIlluminationLevel = min(255, (desiredIlluminationLevel + 0xFF / 16));
  }
  if (illuminationDimDownButton.isPressed() || illuminationDimDownButton.wasHeldFor(500, 200)) {
    desiredIlluminationLevel = max(46, (desiredIlluminationLevel - 0xFF / 16));
  }  

  changeIllumination(illuminationSensor.getState(), desiredIlluminationLevel);
}

void changeIllumination(bool newState, uint8_t newLevel) {
  newLevel = newState ? newLevel : 0x00; 
  if (illuminationState != newState || illuminationLevel != newLevel) {
    illuminationState = newState;
    illuminationLevel = newLevel;
    
    Serial.printf("Illumination %d.\r\n", illuminationLevel);
    illuminationOutput.set(illuminationLevel);
    mmi.setIllumination(illuminationLevel);
    mmi.setHighlightLevel(max(0x46, illuminationLevel));
  }
}


  ////////////////////////
 // IGNITION FUNCTIONS // 
////////////////////////

void updateIgnition() {
  ignitionLights.red->update();
  ignitionLights.green->update();
  ignitionButton.update();
  crankSensor.update();
  ignitionOutputs.crank->update();
  
  neutralSensor.getState();
  bool isClutchPressed  = clutchSensor.getState();
  bool isBrakePressed   = brakeSensor.getState();
  bool isKeyInserted    = keySensor.getState();

  // key signal
  ignitionOutputs.key->toggle(isKeyInserted || ignition.state >= IGNITION_ON);

  // ignition button
  if (!ignition.engine) {  
    if (ignitionButton.isPressed() && isKeyInserted) {
      // switch engine on
      if (isClutchPressed) {
        startEngine();
      }
      // toggle through ignition states
      else {
        setIgnition(ignition.state >= IGNITION_ON ? IGNITION_OFF : ignition.state+1);
      }
    }
  }
  else {
    if ((ignitionButton.isPressed() && isBrakePressed) || ignitionButton.wasHeldFor(ENGINE_BUTTON_STOP_DURATION)) {
      stopEngine();
    }
  }
  
  // ignition light
  if (ignition.engine) {
    if (crankSensor.isHeld()) {
      setIgnitionLight(3, 100, 50);
    }
    else {
      setIgnitionLight(2, 0, 0);
    }
  }
  else if (isKeyInserted) {
    if (isClutchPressed) {
      setIgnitionLight(2, 1000, 100);
    }
    else {
      if (ignition.state >= IGNITION_ACC) {
        setIgnitionLight(1, 0, 0);
      }
      else {
        setIgnitionLight(1, 2000, 100);
      }
    }
  }
  else if (bcm.areDoorsUnlocked()) {
    setIgnitionLight(1, 0, 0);
  }
  else {
    setIgnitionLight(0, 0, 0);
  }

  // reenable ACC after crank has stopped
  if (crankSensor.wasPressedTimes(1)) {
    ignitionOutputs.acc->toggle(ignition.state >= IGNITION_ACC);
  }
}

void setIgnitionLight(uint8_t newState, unsigned int interval, unsigned int duration) {
  if (newState != ignitionLights.state || ignitionLights.red->getInterval() != interval || ignitionLights.green->getDuration() != duration) {
    if (newState == 0) {
      ignitionLights.red->deactivate();
      ignitionLights.green->deactivate();
    }
    if (newState == 1) {
      ignitionLights.red->blink(interval, duration);
      ignitionLights.green->deactivate();
    }
    if (newState == 2) {
      ignitionLights.red->deactivate();
      ignitionLights.green->blink(interval, duration);
    }
    if (newState == 3) {
      ignitionLights.red->blink(interval, duration);
      ignitionLights.green->blink(interval, duration);
    }
    ignitionLights.state = newState;
  }
}

void setIgnition(uint8_t newState) {
  if (ignition.state != newState) {
    ignition.state = newState;

    ignitionOutputs.acc->toggle(ignition.state >= IGNITION_ACC && !ignitionOutputs.crank->isActive());
    ignitionLights.acc->toggle(ignition.state == IGNITION_ACC);
    ignitionLights.nats->toggle(ignition.state >= IGNITION_ACC);

    ignitionOutputs.nats->toggle(ignition.state >= IGNITION_ON);
    ignitionOutputs.on->toggle(ignition.state >= IGNITION_ON);
    ignitionLights.on->toggle(ignition.state >= IGNITION_ON);
    Serial.printf("Ignition: %d.\r\n", ignition.state);
  }
}

void stopEngine() {
  if (ignition.engine) {
    setIgnition(IGNITION_OFF);
    ignition.engine = false;
    Serial.println("Stop engine!");
  }
}

void startEngine() {
  if (!ignition.engine && (clutchSensor.getState() || neutralSensor.getState())) {
    setIgnition(IGNITION_ON);
    ignition.engine = true;
    ignitionOutputs.acc->deactivate();
    ignitionOutputs.crank->set(HIGH, CRANK_DURATION);
    Serial.println("Start engine!");
  }
}


  ////////////////////////////////
 // HEADLIGHT WASHER FUNCTIONS //
////////////////////////////////

void updateHeadlightWasher(){
  headlightWasherButton.update();
  headlightWasherRelay.update();

  if (illuminationSensor.getState()) {
    if (headlightWasherButton.wasPressedTimes(1)) {
      Serial.println("ScheinwerferWaschanlage aktiviert");
      headlightWasherRelay.set(HIGH, 6000);
    }
  }
}


  //////////////////////////////
 // STEERING WHEEL FUNCTIONS //
//////////////////////////////

void updateSwc() {
  swcVolumeUpButton.update();
  swcVolumeDownButton.update();
  swcPhoneButton.update();
  swcVoiceButton.update();
  swcSeekUpButton.update();
  swcSeekDownButton.update();

  if (swcVolumeUpButton.wasPressedTimes(1)) {
    Keyboard.press(KEY_MEDIA_VOLUME_INC);
    Keyboard.release(KEY_MEDIA_VOLUME_INC);
  }
  if (swcVolumeDownButton.wasPressedTimes(1)) {
    Keyboard.press(KEY_MEDIA_VOLUME_DEC);
    Keyboard.release(KEY_MEDIA_VOLUME_DEC);
  }
  if (swcPhoneButton.wasPressedTimes(1)) {
    // TODO start the phone app
  }
  if (swcVoiceButton.wasPressedTimes(1)) {
    // TODO start google assistant
  }
  if (swcSeekUpButton.wasPressedTimes(1)) {
    Keyboard.press(KEY_MEDIA_PREV_TRACK);
    Keyboard.release(KEY_MEDIA_PREV_TRACK);
  }
  if (swcSeekDownButton.wasPressedTimes(1)) {
    Keyboard.press(KEY_MEDIA_NEXT_TRACK);
    Keyboard.release(KEY_MEDIA_NEXT_TRACK);
  }
}
 

  ///////////////////
 // BCM FUNCTIONS //
///////////////////

void updateBcm(Button *lockButton, Button *unlockButton, Bcm *bcm) {
  acm.setHub(keySensor.getState());
  
  if (unlockButton->wasPressedTimes(1)) {
    acm.setOtg(true);
  } 
  else if (unlockButton->wasPressedTimes(3)) {
    bcm->openWindows();
  }

  if (lockButton->wasPressedTimes(1)) {
    if (bcm->isAnyDoorOpen()) {
      bcm->unlockDoors();
    } 
    else {
      if (!keySensor.getState()) {
        sleep.deepSleep();
        acm.setOtg(false);
      }
    }
  }
  else if (lockButton->wasPressedTimes(3)) {
    bcm->closeWindows();
  }
  else if (lockButton->wasPressedTimesOrMore(4)) {
    // TODO: Start engine for 10 minutes
  }
}


  ////////////////////
 // HVAC FUNCTIONS // 
////////////////////

void updateHvac() {
  hvac.update();
}

  /////////////////////
 // SLEEP FUNCTIONS // 
/////////////////////

void updateSleep() {
  sleep.update();
}
