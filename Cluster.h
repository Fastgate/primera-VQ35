#ifndef CLUSTER_H
#define CLUSTER_H

#include <mcp2515.h>
#include "bitfield.h"

class Cluster {
  public:
    Cluster(uint8_t csPin) {
      this->can = new MCP2515(csPin);
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
        switch (inputMessage.id) {
          case (0x023D): // ect/rpm
            this->packet1.rpm = (inputMessage.buf[4] * 256 + inputMessage.buf[3]) * 2.3 * 10;
            this->packet2.coolantTemperature = inputMessage.buf[7];

            this->sendMessage(0x0180, sizeof(this->packet1), (uint8_t*)this->packet1.data);
            this->sendMessage(0x0551, sizeof(this->packet2), (uint8_t*)this->packet2.data);
            break;
          case (0x060D): // bcm
            this->sendMessage(0x060D, inputMessage.len, (uint8_t*)inputMessage.buf);
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
    boolean isInitialized = false;

    union {
      unsigned char data[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
      BitFieldMember<0, 16> rpm;
    } packet1;

    union {
      unsigned char data[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
      BitFieldMember<0, 8> coolantTemperature;
    } packet2;
};

#endif
