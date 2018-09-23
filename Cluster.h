#ifndef CLUSTER_H
#define CLUSTER_H

#include <mcp2515.h>
#include "bitfield.h"

class Cluster {
  public:
    Cluster(uint8_t csPin, DigitalInput *clutchSensor) {
      this->can = new MCP2515(csPin);
      this->clutchSensor = clutchSensor;
    }
    ~Cluster() {
      delete this->can;
    }
    boolean setup(CAN_SPEED speed, CAN_CLOCK clock) {
      this->can->reset();
      this->can->setBitrate(speed, clock);
      this->isInitialized = this->can->setNormalMode() == MCP2515::ERROR_OK;
      return this->isInitialized;
    }
    void updateCan(CAN_message_t inputMessage) {
      if (this->isInitialized) {
        this->packet1.clutchPressed = this->clutchSensor->getState();
        this->packet5.clutchPressed1 = this->clutchSensor->getState();
        this->packet1.clutchBrakePressed = this->clutchSensor->getState();
                        
        switch (inputMessage.id) {
          case (0x0002): //STRG
            this->sendMessage(0x0002, inputMessage.len, (uint8_t*)inputMessage.buf);
            break;
          case (0x0160): 
            this->sendMessage(0x0160, inputMessage.len, (uint8_t*)inputMessage.buf);
            break;  
          case (0x0182): // ECM
            this->sendMessage(0x0182, inputMessage.len, (uint8_t*)inputMessage.buf);
            break;
          case (0x01F9): // ECM
            this->sendMessage(0x01F9, inputMessage.len, (uint8_t*)inputMessage.buf);
            break;
          case (0x0215): 
            this->sendMessage(0x0215, inputMessage.len, (uint8_t*)inputMessage.buf);
            break;
          case (0x023D): // ect/rpm
            this->packet1.rpm = (inputMessage.buf[4] * 256 + inputMessage.buf[3]) * 2.3 * 10;
            this->packet2.coolantTemperature = inputMessage.buf[7];

            this->sendMessage(0x0180, sizeof(this->packet1), (uint8_t*)this->packet1.data);
            this->sendMessage(0x0216, sizeof(this->packet5), (uint8_t*)this->packet5.data);
            this->sendMessage(0x0551, sizeof(this->packet2), (uint8_t*)this->packet2.data);
            break;
          case (0x0233): // ect/rpm
            packet2.cruisecontrol = ((inputMessage.buf[3] & B00000010) == B00000010);
            this->sendMessage(0x0551, sizeof(this->packet2), (uint8_t*)this->packet2.data);
            break; 
          case (0x0245): 
            this->sendMessage(0x0245, inputMessage.len, (uint8_t*)inputMessage.buf);
            break;
          case (0x0280): 
            this->sendMessage(0x0280, inputMessage.len, (uint8_t*)inputMessage.buf);
            break;
          case (0x0284): // ABS
            this->sendMessage(0x0284, inputMessage.len, (uint8_t*)inputMessage.buf);
            break;
          case (0x0285): // ABS
            this->sendMessage(0x0285, inputMessage.len, (uint8_t*)inputMessage.buf);
            break;
          case (0x0292): 
            this->sendMessage(0x0292, inputMessage.len, (uint8_t*)inputMessage.buf);
            break;
          case (0x02DE): 
            this->sendMessage(0x02DE, inputMessage.len, (uint8_t*)inputMessage.buf);
            break;
          case (0x0342):
            this->sendMessage(0x0342, inputMessage.len, (uint8_t*)inputMessage.buf);
            break;
          case (0x0354): // ABS
            this->sendMessage(0x0354, inputMessage.len, (uint8_t*)inputMessage.buf);
            break;
          case (0x0355): 
            this->sendMessage(0x0355, inputMessage.len, (uint8_t*)inputMessage.buf);
            break;
          case (0x0358): // bcm
            packet3.keyInsert = ((inputMessage.buf[1] & B00000110) == B00000110);
            this->sendMessage(0x0358, sizeof(this->packet3), (uint8_t*)this->packet3.data);
            break; 
          case (0x0385): 
            this->sendMessage(0x0385, inputMessage.len, (uint8_t*)inputMessage.buf);
            break;
          case (0x0421): // ECM
            this->sendMessage(0x0421, inputMessage.len, (uint8_t*)inputMessage.buf);
            break;
          case (0x0512): 
            this->sendMessage(0x0512, inputMessage.len, (uint8_t*)inputMessage.buf);
            break;
          case (0x054C): 
            this->sendMessage(0x054C, inputMessage.len, (uint8_t*)inputMessage.buf);
            break;
          case (0x0580): // ABS
            this->sendMessage(0x0580, inputMessage.len, (uint8_t*)inputMessage.buf);
            break;
          case (0x060D): // bcm
            this->packet4.keyPresent = ((inputMessage.buf[1] & B00000110) == B00000110);
            this->sendMessage(0x0351, sizeof(this->packet4), (uint8_t*)this->packet4.data);
            packet2.ignition = ((inputMessage.buf[1] & B00000110) == B00000110) ? 0xA0 : 0x20;
            packet3.IGNon = ((inputMessage.buf[1] & B00000110) == B00000110) ? 0xA0 : 0x20;
            packet7.iGnOn = ((inputMessage.buf[1] & B00000110) == B00000110) ? 0xA0 : 0x80;
            packet5.ignOn = ((inputMessage.buf[1] & B00000110) == B00000110);
           
            this->sendMessage(0x060D, inputMessage.len, (uint8_t*)inputMessage.buf);
            this->sendMessage(0x0216, sizeof(this->packet5), (uint8_t*)this->packet5.data);
            this->sendMessage(0x0358, sizeof(this->packet3), (uint8_t*)this->packet3.data);
            this->sendMessage(0x035D, sizeof(this->packet7), (uint8_t*)this->packet7.data);
            this->sendMessage(0x0551, sizeof(this->packet2), (uint8_t*)this->packet2.data);
            break;
          case (0x0625): // IPDM
            this->sendMessage(0x0625, inputMessage.len, (uint8_t*)inputMessage.buf);
            break;
          case (0x0682): // BCM
            this->sendMessage(0x0682, inputMessage.len, (uint8_t*)inputMessage.buf);
            break;
          case (0x06E2): // ABS
            this->sendMessage(0x06E2, inputMessage.len, (uint8_t*)inputMessage.buf);
            break;
          case (0x06F1): // bcm
            packet6.parkingBrake = ((inputMessage.buf[4] & B00010000) == B00010000);
            this->sendMessage(0x05C5, sizeof(this->packet6), (uint8_t*)this->packet6.data);
            break;       
        }
      }
    }
  private:
    boolean sendMessage(uint32_t can_id, uint8_t can_dlc, uint8_t* data) {
      struct can_frame message;
      message.can_id = can_id;
      message.can_dlc = can_dlc;
      for (uint8_t i = 0; i < can_dlc && i < 8; i++) {
        message.data[i] = data[i];
      }
      return this->can->sendMessage(&message) == MCP2515::ERROR_OK;
    }
  
    MCP2515* can;
    DigitalInput *clutchSensor;
    
    boolean isInitialized = false;

    union // ID 0x0180
    {
      unsigned char data[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
      BitFieldMember<0, 16> rpm;
      BitFieldMember<61, 1> clutchPressed;
      BitFieldMember<63, 1> clutchBrakePressed;
     } packet1;

    union // ID 0x0216
    {
      unsigned char data[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
      BitFieldMember<7, 1> ignOn;
      BitFieldMember<12, 1> clutchPressed1;
    } packet5; 

    union // ID 0x0551
    {
      unsigned char data[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
      BitFieldMember<0, 8> coolantTemperature;
      BitFieldMember<24, 8> ignition;
      BitFieldMember<43, 1> cruisecontrol;
    } packet2;

    union // ID 0x0358
    {
      unsigned char data[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
      BitFieldMember<7, 1> keyInsert;
      BitFieldMember<24, 8> IGNon;
    } packet3; 

   union // ID 0x0351
    {
      unsigned char data[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
      BitFieldMember<47, 1> keyPresent; 
     } packet4;  

   union // ID 0x05C5
    {
      unsigned char data[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
      BitFieldMember<5, 1> parkingBrake;
    } packet6;

   union // ID 0x035D
    {
      unsigned char data[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
      BitFieldMember<0, 8> iGnOn;
    } packet7;  

   

/*
 * ID 351:
 * 39 = 
 * 40 = blink Key Symbol
 * 41 = Key Sybol
 * 42 = beep tone
 * 43 = other beep tone
 * 44 = no Key (display)
 * 
 * /
 */
    
};

#endif
