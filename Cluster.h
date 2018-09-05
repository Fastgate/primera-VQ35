#ifndef CLUSTER_H
#define CLUSTER_H

#include <mcp_can.h>
#include "bitfield.h"

class Cluster {
  public:
    Cluster(uint8_t csPin) {
      this->can = new MCP_CAN(csPin);
    }
    ~Cluster() {
      delete this->can;
    }
    boolean setup(uint8_t mode, uint8_t speed, uint8_t clock) {
      uint8_t canStatus = this->can->begin(mode, speed, clock);
      if (canStatus == CAN_OK) {
        this->can->setMode(MCP_NORMAL);
        this->isInitialized = true;
      }
      return this->isInitialized;
    }
    void updateCan(CAN_message_t inputMessage) {
      if (this->isInitialized) {
        switch (inputMessage.id) {
          case (0x023D): // ect/rpm
            this->packet1.rpm = (inputMessage.buf[4] * 256 + inputMessage.buf[3]) * 2.3 * 10;
            this->packet2.coolantTemperature = inputMessage.buf[7];

            this->can->sendMsgBuf(0x0180, 0, sizeof(this->packet1), (uint8_t*)this->packet1.data);
            this->can->sendMsgBuf(0x0551, 0, sizeof(this->packet2), (uint8_t*)this->packet2.data);
            break;
          case (0x060D): // bcm
            this->can->sendMsgBuf(0x060D, 0, inputMessage.len, (uint8_t*)inputMessage.buf);
            break;
        }
      }
    }
  private:
    MCP_CAN* can;
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
