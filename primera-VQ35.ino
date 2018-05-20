#include <SPI.h>
#include <FlexCAN.h>
#include <arduinoIO.h>
#include <arduinoMmi.h>

#include "Binary.h"
#include "Serial.h"
#include "Hvac.h"
#include "Sleep.h"

#include "Acm.h"
#include "Bcm.h"
#include "Ecm.h"

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

DigitalInput illuminationSensor(37, 20, LOW, INPUT_PULLUP);
Button illuminationDimUpButton(new DigitalInput(45, 20, LOW, INPUT_PULLUP), 0);
Button illuminationDimDownButton(new DigitalInput(46, 20, LOW, INPUT_PULLUP), 0);
AnalogOutput illuminationOutput(21);

uint8_t desiredIlluminationLevel = 0xFF / 2;
uint8_t illuminationLevel = 0x00;
bool illuminationState = false;


  ////////////////////////////
 // CAR MODULE DEFINITIONS //
////////////////////////////

DigitalInput clutchSensor(16, 20, HIGH, INPUT);
DigitalInput brakeSensor(17, 20, HIGH, INPUT);
DigitalInput neutralSensor(36, 20, HIGH, INPUT);
DigitalInput keySensor(23, 20, HIGH, INPUT);

Sleep sleep(13, 10 * 60 * 1000);
Acm acm;
Hvac hvac;
Bcm bcm;
Ecm ecm(&clutchSensor, &brakeSensor, &neutralSensor, &keySensor, &bcm);


  /////////////////////////////
 // STEERING WHEEL CONTROLS //
/////////////////////////////

Button swcVolumeUpButton(new AnalogInput(A11, 28, 34), 0);
Button swcVolumeDownButton(new AnalogInput(A10, 28, 34), 0);
Button swcPhoneButton(new AnalogInput(A11, 10, 16), 0);
Button swcVoiceButton(new AnalogInput(A10, 10, 16), 0);
Button swcSeekUpButton(new AnalogInput(A11, 15, 20), 0);
Button swcSeekDownButton(new AnalogInput(A10, 15, 20), 0);


//************************** Primera STW Inputs *******************************

int RevGear = 38;           // Rückwärtsgang  OK
int RevGear_Stat = 0;


  ////////////////////////
 // SERIAL DEFINITIONS //
////////////////////////

SerialPacket statusInitSuccess(0x61, 0x01);
SerialPacket statusInitError(0x65, 0x01);
SerialDataPacket<unsigned long> baudRateChange(0x65, 0x01);

SerialReader serialReader(128);


  /////////////////////////
 // CAN BUS DEFINITIONS //
/////////////////////////

struct CanData {
  union {
    unsigned char data[4];
    BitFieldMember<0, 32> canId;
  } metaData;
  uint8_t rxBuf[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
};

SerialDataPacket<CanData> canSnifferPacket(0x62, 0x6d);

boolean isCanSnifferActive = false;

CAN_message_t canMessage;


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

  ecm.update();

  updateSwc();

  bcm.update(updateBcm);

  hvac.update();
  
  sleep.update();

  updateCanSniffer();
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
        case 0x0a: // start sniffer
          startCanSniffer();
          break;
        case 0x0b: // stop sniffer
          stopCanSniffer();
          break;
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

void updateBcm(Button *lockButton, Button *unlockButton, Button *headlightWasherButton, Bcm *bcm) {
  acm.setHub(keySensor.getState());

  // headlight washer
  if (illuminationSensor.getState() && headlightWasherButton->wasPressedTimes(1)) {
    bcm->washHeadlights(1200);
  }

  // car remote unlock button
  if (unlockButton->wasPressedTimes(1)) {
    acm.setOtg(true);
    sleep.cancelSleepRequest();
  }
  else if (unlockButton->wasPressedTimes(3)) {
    bcm->openWindows();
  }

  // car remote lock button
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
  else if (lockButton->wasPressedTimesOrMore(4) && !keySensor.getState()) {
    ecm.startEngineDefrost();
  }
}


  ///////////////////////
 // CAN BUS FUNCTIONS //
///////////////////////

void startCanSniffer() {
  Can0.begin(500000);
  isCanSnifferActive = true;
}

void stopCanSniffer() {
  Can0.end();
  isCanSnifferActive = false;
}

void updateCanSniffer() {
  if (!isCanSnifferActive) {
    return;
  }
  
  while (Can0.available()) {
    Can0.read(canMessage);
    canSnifferPacket.payload()->metaData.canId = canMessage.id;
    for (uint8_t i = 0; i < 8; i++) {
      uint8_t data = 0x00;
      if (i < canMessage.len) {
         data = canMessage.buf[i];
      }
      canSnifferPacket.payload()->rxBuf[i] = data;
    }
    canSnifferPacket.serialize(Serial);
  }
}

