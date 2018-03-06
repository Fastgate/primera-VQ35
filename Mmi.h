#ifndef MMI_H
#define MMI_H

#include "Button.h"

class MmiButtonTrigger : public Trigger {
  public:
    MmiButtonTrigger(int buttonId) {
      _buttonId = buttonId;
    }
    virtual boolean getState() { 
      return _state;
    }
    void update(int buttonId, boolean newState) {
      if (buttonId == _buttonId) {
        _state = newState;
      }
    }
  private:
    uint8_t _buttonId = 0x00;
    boolean _state = false;
};

class MmiButton : public Button {
  public:
    MmiButton(MmiButtonTrigger *trigger) : Button(trigger) {
      _trigger = trigger;
    }
    void updateTrigger(int buttonId, boolean newState) {
      _trigger->update(buttonId, newState);
    }
    ~MmiButton() {
      delete _trigger;
    }
  private:
    MmiButtonTrigger *_trigger;
};

class MmiWheel {
  public:
    MmiWheel(uint8_t id) {
      _id = id;
    }
    void turn(uint8_t id, int8_t amount) {
      if (_id == id) {
        _amount = amount;
      }
    }
    int8_t getAmount() {
      return _state;
    }
    bool wasTurned() {
      return _state != 0;
    }
    void update() {
      if (_amount != _state) {
        _state = _amount;
        _amount = 0;
      }
    }
  private:
    int8_t _state = 0;
    int8_t _amount = 0;
  
    uint8_t _id = 0x00;
};

class Mmi {
public:
  Mmi(HardwareSerial *serial, uint32_t mode, uint8_t bufferSize, uint8_t buttonCount, uint8_t wheelCount) {
    _serial = serial;
    _serial->begin(9600, mode);
    _buffer = (uint8_t*)calloc(sizeof(uint8_t), bufferSize);
    _bufferSize = bufferSize;
    
    _buttonCount = buttonCount;
    _buttons = new MmiButton*[buttonCount];

    _wheelCount = wheelCount;
    _wheels = new MmiWheel*[wheelCount];
  }
  ~Mmi() {
    free(_buffer);
    for (int i = 0; i < _assignedButtonCount; i++) {
      delete _buttons[i];
    }
    delete[] _buttons;
  }
  MmiButton *createButton(uint8_t buttonId) {
    if (_assignedButtonCount >= _buttonCount) {
      return nullptr;
    }
    MmiButton *button = new MmiButton(new MmiButtonTrigger(buttonId));
    _buttons[_assignedButtonCount] = button;
    _assignedButtonCount++;
    return button;
  }
  MmiWheel *createWheel(uint8_t wheelId) {
    if (_assignedWheelCount >= _wheelCount) {
      return nullptr;
    }
    MmiWheel *wheel = new MmiWheel(wheelId);
    _wheels[_assignedWheelCount] = wheel;
    _assignedWheelCount++;
    return wheel;
  }
  void setIllumination(uint8_t brightness) {
    uint8_t message[] = { 0x60, brightness };
    this->write(message, sizeof(message));
  }
  void setHighlightLevel(uint8_t brightness) {
    uint8_t message[] = { 0x64, brightness, 0x01 };
    this->write(message, sizeof(message));
  }
  void setLight(uint8_t id, bool state) {
    if (id == 0x10) {
      uint8_t message[] = { 0x68, (uint8_t)(state ? 0x01 : 0x00), id, id };
      this->write(message, sizeof(message));
    }
    else {
      uint8_t message[] = { 0x68, (uint8_t)(state ? 0x01 : 0x00), id };
      this->write(message, sizeof(message));
    }
  }
  void enableKeys() {
    uint8_t message[] = { 0x70, 0x12 };
    this->write(message, 2);
  }
  void shutdown() {
    uint8_t message[] = { 0x70, 0x11 };
    this->write(message, sizeof(message));
  }
  void update(void (*mmiCallback)(uint8_t code)) {
    while (_serial->available() && _readIndex < _bufferSize) {
      uint8_t data = _serial->read();
      _buffer[_readIndex++] = data;

      // Detect packet start
      if (_startIndex == -1) {
        if (data == 0x10) {
          _startIndex = -2;
        }
        else if (data == 0x06) {
/*          
          Serial.println("< ACK");
*/          
          _readIndex = 0;
        }
        continue;
      }
      else if (_startIndex == -2) {
        if (data == 0x02) {
          _startIndex = _readIndex;
        }
        else {
          _startIndex = -1;
        }
        continue;
      }

      // Detect packet end
      if (_endIndex == -1 && data == 0x10) {
        _endIndex = -2;
        continue;
      }
      else if (_endIndex == -2) {
        if (data == 0x03) {
          _endIndex = _readIndex - 2;
        }
        else if (data != 0x10) {
          _endIndex = -1;
        }
        continue;
      }

      // We have a packet ...
      if (_endIndex > 0) {
        uint8_t remoteChecksum = data;
        uint8_t localChecksum = 0;
/*
        Serial.print("< ");
*/
        for (uint8_t i = _startIndex - 2; i < _endIndex + 2; i++) {
          localChecksum += _buffer[i];
/*
          Serial.printf("%02X ", _buffer[i]);
*/
        }
/*
        Serial.printf("%02X ", data);

        Serial.printf("(%d =", remoteChecksum);
        Serial.printf(" %d)", localChecksum);
        Serial.println();
*/

        if (localChecksum == remoteChecksum) {
          ack();
          uint8_t payloadSize = _endIndex - _startIndex;
  
          uint8_t *payload = (uint8_t*)calloc(sizeof(uint8_t), payloadSize+1);
          for (uint8_t i = 0; i < payloadSize; i++) {
            payload[i] = _buffer[_startIndex+i];
          }
          this->serialEvent(payload, payloadSize, mmiCallback);
          free(payload);
        }

        _startIndex = -1;
        _endIndex = -1;
        _readIndex = 0;
      }
      else if (_readIndex >= _bufferSize) {
/*
        Serial.print("< ");
        for (uint8_t i = 0; i < _bufferSize; i++) {
          Serial.printf("%02X ", _buffer[i]);
        }
        Serial.println("(!INVALID)");
*/
        
        _startIndex = -1;
        _endIndex = -1;
        _readIndex = 0;
      }
    }

    // Update button states
    for (int i = 0; i < _assignedButtonCount; i++) {
      _buttons[i]->update();
    }

    // Update wheel states
    for (int i = 0; i < _assignedWheelCount; i++) {
      _wheels[i]->update();
    }
  }
private:
  uint8_t _buttonCount = 0;
  uint8_t _assignedButtonCount = 0;
  MmiButton **_buttons;

  uint8_t _wheelCount = 0;
  uint8_t _assignedWheelCount = 0;
  MmiWheel **_wheels;

  HardwareSerial *_serial;
  uint8_t *_buffer;
  uint8_t _bufferSize = 0;
  
  int _startIndex = -1;
  int _endIndex = -1;
  uint8_t _readIndex = 0;

  void write(uint8_t *data, uint8_t len) {
    uint8_t *message = (uint8_t*)calloc(sizeof(uint8_t), len+5);
    message[0] = 0x10;
    message[1] = 0x02;
    uint8_t checksum = 0x25;
    for (uint8_t i = 0; i < len; i++) {
      checksum += data[i];
      message[2+i] = data[i];
    }
    message[2+len] = 0x10;
    message[3+len] = 0x03;
    message[4+len] = checksum;
    _serial->write(message, len + 5);
    _serial->flush();
/*
    Serial.print("> ");
    for (uint8_t i = 0; i < len + 5; i++) {
      Serial.printf("%02X ", message[i]);
    }
    Serial.println();
    Serial.flush();
*/
  }
  void ack() {
/*    
    Serial.println("> ACK");
*/    
    _serial->write(0x06);
    _serial->flush();
  }
  void serialEvent(uint8_t *data, uint8_t len, void (*mmiCallback)(uint8_t code)) {
    // power on or volume button pushed initially
    if (data[0] == 0x79 && len > 1) {
      if (data[1] == 0x38 || data[1] == 0xff) {
        mmiCallback(data[1]);
      }
    }
    // unknown ...
    else if (data[0] == 0x35) {
      // ... but it comes as return to the activation sequence 70 12
      // data package only has this byte ... however the user might want to use it as trigger:
      mmiCallback(data[0]);
    }
    // a button has been pressed
    else if ((data[0] == 0x30 || data[0] == 0x31) && len > 1) {
      for (int i = 0; i < _assignedButtonCount; i++) {
        _buttons[i]->updateTrigger(data[1], data[0] == 0x30);
      }
    }
    // the small wheel has been turned
    else if ((data[0] == 0x40 || data[0] == 0x41) && len > 1) {
      for (int i = 0; i < _assignedWheelCount; i++) {
        _wheels[i]->turn(0x40, data[1] * (data[0] == 0x40 ? 1 : -1));
      }
    }
    // the big wheel has been turned
    else if ((data[0] == 0x50 || data[0] == 0x51) && len > 1) {
      for (int i = 0; i < _assignedWheelCount; i++) {
        _wheels[i]->turn(0x50, data[1] * (data[0] == 0x50 ? 1 : -1));
      }
    }
  }
};

class MmiLight {
  public:
    MmiLight(uint8_t lightId, Mmi *mmi) {
      _mmi = mmi;
      _lightId = lightId;
    }
    void on() {
      this->set(true);
    }
    void off() {
      this->set(false);
    }
    void toggle() {
      this->set(!_state);
    }
    boolean isOn() {
      return _state;
    }
    void set(boolean state) {
      _state = state;
      _mmi->setLight(_lightId, state);
    }
  private:
    uint8_t _lightId = 0;
    boolean _state = false;
    Mmi *_mmi;
};

#endif
