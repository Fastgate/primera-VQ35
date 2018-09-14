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
                
        switch (inputMessage.id) {
          case (0x023D): // ect/rpm
            this->packet1.rpm = (inputMessage.buf[4] * 256 + inputMessage.buf[3]) * 2.3 * 10;
            this->packet2.coolantTemperature = inputMessage.buf[7];

            this->sendMessage(0x0180, sizeof(this->packet1), (uint8_t*)this->packet1.data);
            this->sendMessage(0x0216, sizeof(this->packet5), (uint8_t*)this->packet5.data);
            this->sendMessage(0x0551, sizeof(this->packet2), (uint8_t*)this->packet2.data);
            break;
          case (0x060D): // bcm
            packet2.ignition = ((inputMessage.buf[0] & B00000100) == B00000100) ? 0xA0 : 0x20;

            packet5.ignOn = ((inputMessage.buf[1] & B00000100) == B00000100);
            this->sendMessage(0x0216, sizeof(this->packet5), (uint8_t*)this->packet5.data);

            this->sendMessage(0x0551, sizeof(this->packet2), (uint8_t*)this->packet2.data);
            this->sendMessage(0x060D, inputMessage.len, (uint8_t*)inputMessage.buf);
            break;
          case (0x0354): // ABS
            this->sendMessage(0x0354, inputMessage.len, (uint8_t*)inputMessage.buf);
            break;
          case (0x0358): // bcm
            packet3.keyInsert = ((inputMessage.buf[0] & B00000001) == B00000001);
            this->sendMessage(0x0358, sizeof(this->packet3), (uint8_t*)this->packet3.data);
            packet4.keyPresent = ((inputMessage.buf[0] & B00000001) == B00000001);
            this->sendMessage(0x0351, sizeof(this->packet4), (uint8_t*)this->packet4.data);
            break; 
          case (0x0233): // ect/rpm
            packet2.cruisecontrol = ((inputMessage.buf[3] & B00000010) == B00000010);
            this->sendMessage(0x0551, sizeof(this->packet2), (uint8_t*)this->packet2.data);
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
};

#endif
