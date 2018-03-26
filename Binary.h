#ifndef BINARY_H
#define BINARY_H

#include "network.h"

class BinaryData {
  public:
    BinaryData(uint8_t len) {
      _len = len;
      _payload = (char*)calloc(sizeof(char), _len+1);
    }
    ~BinaryData() {
      free(_payload);
    }
    typedef enum STATUS {
      OK,
      INDEXOUTOFBOUNDS
    } AccessStatus;
    struct ByteResult {
      AccessStatus state;
      unsigned char data;
    };
    struct BoolResult {
      AccessStatus state;
      bool data;
    };
    struct LongResult {
      AccessStatus state;
      unsigned long int data;
    };
    AccessStatus toggleFlag(uint8_t index, unsigned char mask) {
      if (checkIndex(index) == AccessStatus::INDEXOUTOFBOUNDS) {
        return AccessStatus::INDEXOUTOFBOUNDS;
      }
    
      _payload[index] ^= mask;
    
      return AccessStatus::OK;
    }
    AccessStatus writeFlag(uint8_t index, unsigned char mask, bool value) {
      if (checkIndex(index) == AccessStatus::INDEXOUTOFBOUNDS) {
        return AccessStatus::INDEXOUTOFBOUNDS;
      }
    
      if (value) {
        _payload[index] |= mask;
      } 
      else {
        _payload[index] &= ~mask;
      }
      return AccessStatus::OK;
    }
    AccessStatus writeByte(uint8_t index, unsigned char value) {
      if (checkIndex(index) == AccessStatus::INDEXOUTOFBOUNDS) {
        return AccessStatus::INDEXOUTOFBOUNDS;
      }
    
      _payload[index] = value;
      return AccessStatus::OK;
    }
    AccessStatus writeData(uint8_t index, BinaryData* data, uint8_t from, uint8_t to) {
      int inputLen = to - from;
    
      if (checkIndex(index) == AccessStatus::INDEXOUTOFBOUNDS) {
        return AccessStatus::INDEXOUTOFBOUNDS;
      }
      if (checkIndex(index + inputLen - 1) == AccessStatus::INDEXOUTOFBOUNDS) {
        return AccessStatus::INDEXOUTOFBOUNDS;
      }
    
      char* inputData = data->_payload;
    
      for (int i = 0; i < inputLen; i++) {
        _payload[index + i] = inputData[from + i];
      }
    
      return AccessStatus::OK;
    }
    BoolResult readFlag(uint8_t index, unsigned char mask, unsigned char comparator) {
      struct BoolResult result;
  
      if (checkIndex(index) == AccessStatus::INDEXOUTOFBOUNDS) {
        result.state = AccessStatus::INDEXOUTOFBOUNDS;
        return result;
      }
    
      result.data = (_payload[index] & mask) == comparator;
      result.state = AccessStatus::OK;
      return result;
    }
    ByteResult readByte(uint8_t index) {
      struct ByteResult result;
  
      if (checkIndex(index) == AccessStatus::INDEXOUTOFBOUNDS) {
        result.state = AccessStatus::INDEXOUTOFBOUNDS;
        return result;
      }
    
      result.data = _payload[index];
      result.state = AccessStatus::OK;
      return result;
    }
    LongResult readLong(uint8_t index) {
      struct LongResult result;
    
      if (checkIndex(index + 4) == AccessStatus::INDEXOUTOFBOUNDS) {
        result.state = AccessStatus::INDEXOUTOFBOUNDS;
        return result;
      }
    
      result.data = (unsigned long)ntohl(*((long *)&_payload[index]));
      result.state = AccessStatus::OK;
      return result;
    }
    char* getData() {
      return _payload;
    }
    uint8_t getSize() {
      return _len;
    }
  private:
    uint8_t _len = 0;
    char* _payload;
    AccessStatus checkIndex(uint8_t index) {
      if (_len <= index) {
        return AccessStatus::INDEXOUTOFBOUNDS;
      }
      return AccessStatus::OK;
    }
};

class BinaryBuffer {
  public:
    BinaryBuffer(uint8_t len) {
      _data = new BinaryData(len);
    }
    ~BinaryBuffer() {
      delete _data;
    }
    int available() {
      return _data->getSize() - _position;
    }
    BinaryData::ByteResult peek() {
      return _data->readByte(_position);
    }
    BinaryData::ByteResult readByte() {
      BinaryData::ByteResult result = _data->readByte(_position);
      if (result.state == BinaryData::OK) {
        _position++;
      }
      return result;
    }
    BinaryData::LongResult readLong() {
      BinaryData::LongResult result = _data->readLong(_position);
      if (result.state == BinaryData::OK) {
        _position++;
      }
      return result;
    }
    BinaryData::AccessStatus write(unsigned char value) {
      BinaryData::AccessStatus result = _data->writeByte(_position, value);
      if (result == BinaryData::OK) {
        _position++;
      }
      return result;
    }
    BinaryData::AccessStatus write(BinaryBuffer *otherBuffer) {
      BinaryData::AccessStatus result = _data->writeData(_position, otherBuffer->_data, otherBuffer->_position, otherBuffer->_position + _data->getSize());
      if (result == BinaryData::OK) {
        _position+= _data->getSize() - 1;
      }
      return result;
  }
    boolean goTo(int index) {
      if (index < _data->getSize() - 1) {
        _position = index;
        return true;
      }
      return false;
    }
    uint8_t getPosition() {
      return _position;
    }
    boolean next() {
      if (_position < _data->getSize() - 1) {
        _position++;
        return true;
      }
      return false;
    }
    uint8_t getSize() {
      return _data->getSize();
    }
  private:
    BinaryData* _data;
    uint8_t _position = 0;
};

#endif

