#include <Arduino.h>
#include <carduinotest.h>
#include "can.h"
#include <arduinoIO.h>
#include "Bcm.h"
#include "Ecm.h"

#define combineUint16(L, H) (H * 256 + L)
#define highUint16(V) (V >> 8)
#define lowUint16(V) (V & 0xFF)
#define CAN_WRITE_INTERVAL 100

void canCallback(const CAN_message_t &message);
void onCarduinoSerialTimeout();
void onCarduinoSerialEvent(uint8_t type, uint8_t id, BinaryBuffer *payloadBuffer);
void shiftGauge();
void canWrite();

u_int8_t transferFlag(uint8_t sourceValue, uint8_t sourceMask, uint8_t targetValue, uint8_t targetMask);
bool readFlag(uint8_t value, uint8_t mask);
uint8_t setFlag(uint8_t value, uint8_t mask);
uint8_t clearFlag(uint8_t value, uint8_t mask);


DigitalInput pnpSwitch(26, 20, LOW, INPUT);
DigitalInput reverseSwitch(27, 20, HIGH, INPUT);
//DigitalInput bluetoothConnect(2, 20, HIGH, INPUT);

Button bluetoothConnection(new DigitalInput(33, 20, HIGH, INPUT));


Button rearFogButton(new AnalogInput(A10, 250, 700), 0);
DigitalInput ClutchSwitchButton(28, 20, HIGH, INPUT);

CanInput headlightSensor    (0x060D, 0, B00000110);  
CanInput runningLightSensor (0x060D, 0, B00000100);
CanInput frontFogLight      (0x060D, 1, B00000001);
CanInput rearFogLight       (0x0358, 4, B10000000); 
CanInput handbrakeSensor    (0x06F1, 4, B00010000);
CanInput ignitionAcc        (0x060D, 1, B00000010);
CanInput ignitionOn         (0x060D, 1, B00000110);
CanInput brakeSensor        (0x06F1, 4, B01000000);
CanInput keySensor          (0x0358, 0, B00000001);

void updateRearFog();
void updateBcm(Button *lockButton, Button *unlockButton, Button *headlightWasherButton, Bcm *bcm);
Bcm bcm;
Ecm ecm(&ClutchSwitchButton, &brakeSensor, &pnpSwitch,  &keySensor, &bluetoothConnection, &bcm);




Can can(&Serial);
Carduino carduino(&Serial, onCarduinoSerialEvent, onCarduinoSerialTimeout);

uint32_t lastCanWrite = 0;

/////////////////////////This Can message are modify://///////////////////////// 
// ID 0x0180
// ID 0x0551
///////////////////////////////////////////////////////////////////////////////


CAN_message_t message0002;
CAN_message_t message0160;
CAN_message_t message0180;
CAN_message_t message0182;
CAN_message_t message01F9;
CAN_message_t message0215;
CAN_message_t message0216;
CAN_message_t message0233;
CAN_message_t message0245;
CAN_message_t message0280;
CAN_message_t message02DE;
CAN_message_t message0351;
CAN_message_t message0354;
CAN_message_t message0355;
CAN_message_t message0358;
CAN_message_t message0385;
CAN_message_t message035D;
CAN_message_t message0421;
CAN_message_t message0551;
CAN_message_t message0580;
CAN_message_t message05C5;
CAN_message_t message0625;
CAN_message_t message0682;
CAN_message_t message060D;
CAN_message_t message06F1;





void setup() {

  carduino.begin();

  Serial.begin(9600);
  //Serial2.begin(9600);

  can.setup(500000, 500000);

  // Add all can packets that should be redirected without change
  can.addCanId(0x0002);
  can.addCanId(0x0160);
  //can.addCanId(0x0180);
  can.addCanId(0x0182);
  can.addCanId(0x01F9);
  can.addCanId(0x0215);
  can.addCanId(0x0216);
  can.addCanId(0x0233);
  can.addCanId(0x0245);
  can.addCanId(0x0280);
  can.addCanId(0x0284);
  can.addCanId(0x0285);
  can.addCanId(0x0292);
  can.addCanId(0x023D);
  can.addCanId(0x02DE);
  can.addCanId(0x0342);
  can.addCanId(0x0351);
  can.addCanId(0x0354);
  can.addCanId(0x0355);
  //can.addCanId(0x0358);
  //can.addCanId(0x0385);
  //can.addCanId(0x0421);
  can.addCanId(0x0512);
  can.addCanId(0x054C);
  can.addCanId(0x0580);

  can.addCanId(0x0625);
  can.addCanId(0x060D);
  can.addCanId(0x0682);
  can.addCanId(0x06E2);
  can.addCanId(0x06F1);

///////////// initialize composed packets /////////////////////////

  message0002.id = 0x0002;
  message0002.len = 8;

  message0160.id = 0x0160;
  message0160.len = 8;

  message0180.id = 0x0180;
  message0180.len = 8;

  message0182.id = 0x0182;
  message0182.len = 8;

  message01F9.id = 0x01F9;
  message01F9.len = 8;

  message0215.id = 0x0215;
  message0215.len = 8;

  message0216.id = 0x0216;
  message0216.len = 8;

  message0233.id = 0x0233;
  message0233.len = 8;

  message0245.id = 0x0245;
  message0245.len = 8;

  message0280.id = 0x0280;
  message0280.len = 8;

  message02DE.id = 0x02DE;
  message02DE.len = 8;

  message0351.id = 0x0351;
  message0351.len = 8;

  message0354.id = 0x0354;
  message0354.len = 8;

  message0355.id = 0x0355;
  message0355.len = 8;

  message0358.id = 0x0358;
  message0358.len = 8;

  message0385.id = 0x0385;
  message0385.len = 8;

  message035D.id = 0x035D;
  message035D.len = 8;

  message0421.id = 0x0421;
  message0421.len = 8;

  message0551.id = 0x0551;
  message0551.len = 8;

  message0580.id = 0x0580;
  message0580.len = 8;

  message05C5.id = 0x05C5;
  message05C5.len = 8;

  message0625.id = 0x0625;
  message0625.len = 8;

  message0682.id = 0x0682;
  message0682.len = 8;

  message060D.id = 0x060D;
  message060D.len = 8;

  message06F1.id = 0x06F1;
  message06F1.len = 8;

  can.startSniffer();

 
 
}

void loop() {
  if (carduino.update()) {
    //can.update(canCallback);
  }   
  can.update(canCallback);

  bcm.update(updateBcm); 

  ecm.update();

  shiftGauge(); 
  
  updateRearFog();

  toggleESD();

 
  if (millis() - lastCanWrite > CAN_WRITE_INTERVAL) {
    canWrite();
    lastCanWrite = millis();
  }

  bluetoothConnection.update();
  if (bluetoothConnection.wasHeldFor(3000)) {
    // Nats einschalten
    bcm.toggleESD();
    bcm.toggleNatsRly();
    Serial.println("verbunden");
  }
  if (bluetoothConnection.wasPressedFor(1100) && bcm.isESDActive()) {
    // Nats ausschalten
    bcm.toggleESD();
    Serial.println("ESD_aus");
  } 
  if (bluetoothConnection.wasPressedFor(1200) && bcm.isNatsRly()) {
    // Nats ausschalten
    bcm.toggleNatsRly();
    Serial.println("NATS_aus");
  } 

  
 
}

void canCallback(const CAN_message_t &message) {

  headlightSensor.updateCan(message);
  runningLightSensor.updateCan(message);
  frontFogLight.updateCan(message);
  rearFogLight.updateCan(message);
  handbrakeSensor.updateCan(message);
  ignitionAcc.updateCan(message);
  ignitionOn.updateCan(message);
  brakeSensor.updateCan(message);
  keySensor.updateCan(message);
  
   
  switch (message.id) {
    case (0x023D):{
      uint16_t rpm = combineUint16(message.buf[3], message.buf[4]) * 2.3 * 10;
      message0180.buf[0] = highUint16(rpm);
      message0180.buf[1] = lowUint16(rpm);

      message0551.buf[0] = message.buf[7];
      
      can.write(message0180);
      can.write(message0551);
      
      }
      break;
    //case (0x060D):

      
      //message0351.buf[5] = B00001111; //Push Brake and Start Button to drive
      //message0358.buf[4] = B10000000; // Rear Fog Lamp
      //message0358.buf[1] = B00011000;
      //message0358.buf[0] = B00000001;
     
      //message0358.buf[3] = B00100000;
      //message0385.buf[0] = B00000010;  // TPMS ERROR
      
      //can.write(message0358);
      //can.write(message0385);
      
      //}else{
        // message0358.buf[4] = B00000000;
      
      //can.write(message0358);
       // }
      //break;
      
      case (0x0233):

       message0551.buf[5] = transferFlag(message.buf[3], B00000010, message0551.buf[5], B01010000);
       //someMessage.buf[0] = readFlag(message.buf[1], B00000110) ? 0xA0 : 0x20;

       can.write(message0551);
      //Serial.println(message0551.buf[5], HEX);
      
     break;
     
  
  }
}

void canWrite() {
  can.write(message0358);
  can.write(message0421);
}

void shiftGauge(){
  if (ignitionAcc.getState() || ignitionOn.getState()){ 
    if (handbrakeSensor.getState()) { // Handbrake is pulled
      message0421.buf[0] = B00001000; // P
    } else if (reverseSwitch.getState()) { // Gear switch in reverse
      message0421.buf[0] = B00010000; // R
    } else if (pnpSwitch.getState()) { // Gear switch in a gear
      message0421.buf[0] = B00101001; // O/D OFF --> Sport
    } else { // Gear switch in neutral
      message0421.buf[0] = B00011000; // N  
    }
  } 
}

void onCarduinoSerialTimeout() {
  carduino.end();
  delay(1000);
  carduino.begin();
}

void onCarduinoSerialEvent(uint8_t type, uint8_t id, BinaryBuffer *payloadBuffer) {
  if (type == 0x61) {
    if (id == 0x0a) {
      // start sniffer
      can.startSniffer();
    } else if (id == 0x0b) {
      // stop sniffer
      can.stopSniffer();
    }
  }
}

u_int8_t transferFlag(uint8_t sourceValue, uint8_t sourceMask, uint8_t targetValue, uint8_t targetMask) {
  return readFlag(sourceValue, sourceMask) ? setFlag(targetValue, targetMask) : clearFlag(targetValue, targetMask);
}

bool readFlag(uint8_t value, uint8_t mask) {
  return mask ==(value & mask);
}

uint8_t setFlag(uint8_t value, uint8_t mask) {
  return value | mask;
}

uint8_t clearFlag(uint8_t value, uint8_t mask) {
  return value & (mask ^ 0xFF);
}


void updateRearFog(){
  rearFogButton.update();
  
  if (headlightSensor.getState() && frontFogLight.getState()){
    if (rearFogButton.wasHeldFor(400)) {
      bcm.toggleRearFogLight();
    }
  }
  if (!frontFogLight.getState() && bcm.isRearFogLightActive()) {
    bcm.toggleRearFogLight();
  }

  if (bcm.isRearFogLightActive()) {
    message0358.buf[4] = B10000000; // Rear Fog Lamp
  } else {
    message0358.buf[4] = B00000000; // Rear Fog Lamp
  }

  
}

void updateBcm(Button *lockButton, Button *unlockButton, Button *headlightWasherButton, Bcm *bcm) {
  


  // headlight washer
  if (headlightSensor.getState() && headlightWasherButton->wasHeldFor(500)) {
    bcm->washHeadlights(1200);
  }

  if (unlockButton->wasPressedTimes(1)) {
   
  }
  else if (!keySensor.getState()&& unlockButton->wasPressedTimes(3)) {
    bcm->openWindows();
  }

  // car remote lock button
  if (lockButton->wasPressedTimes(1)) {
    if (bcm->isAnyDoorOpen()) {
        bcm->unlockDoors();
            
    }
    else {
      if (!keySensor.getState()) {
        
      }
    }
  }
  else if (!keySensor.getState()&& lockButton->wasPressedTimes(2)) {
    bcm->closeWindows();
  }
}

void toggleESD() {
     
      
    }
