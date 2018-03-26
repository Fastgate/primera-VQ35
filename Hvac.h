#ifndef HVAC_H
#define HVAC_H

#include "Button.h"
#include "Output.h"
#include "bitfield.h"

union ClimateControl {
  unsigned char data[3];
  BitFieldMember<0, 1> isAcOn;
  BitFieldMember<1, 1> isAuto;
  BitFieldMember<2, 1> isAirductWindshield;
  BitFieldMember<3, 1> isAirductFace;
  BitFieldMember<4, 1> isAirductFeet;
  BitFieldMember<5, 1> isWindshieldHeating;
  BitFieldMember<6, 1> isRearWindowHeating;
  BitFieldMember<7, 1> isRecirculation;
  BitFieldMember<8, 8> fanLevel;
  BitFieldMember<16, 8> desiredTemperature;
};

class Hvac {
public:
  Hvac() {}
  void update() {
    this->rearHeaterButton->update();
    this->recirculationButton->update();
    this->airConditionButton->update();

    this->isRecirculating = this->recirculationLed->getState();
    this->isAirConditioning = this->airConditionLed->getState();
  }
  void toggleAirCondition() {
    this->airConditionButton->set(ButtonVoltage, 100);
  }
  void toggleRearHeater() {
    this->rearHeaterButton->set(ButtonVoltage, 100);
  }
  void toggleRecirculation() {
    this->recirculationButton->set(ButtonVoltage, 100);
  }
  void toggleAutomatic() {
    this->setFanLevel(1);
  }
  void setTemperatureLevel(uint8_t level) {
    if (level > Hvac::DialStepsTemperature) {
      level = Hvac::DialStepsTemperature;
    }
    if (level < 0) {
      level = 0;
    }

    this->temperatureDial->set((DialMaximum - DialMinimum) / DialStepsTemperature * level + DialMaximum);
    this->temperatureLevel = level;
  }
  void setFanLevel(uint8_t level) {
    if (level > DialStepsFan) {
      level = DialStepsFan;
    }
    if (level < 0) {
      level = 0;
    }

    this->fanDial->set((DialMaximum - DialMinimum) / DialStepsFan * level + DialMaximum);
    this->fanLevel = level;
  }
  void write(uint8_t buttonId, BinaryBuffer *payloadBuffer) {
    switch (buttonId) {
      case 0x01: // OFF BUTTON
        
        break;
      case 0x02: // ac button
        this->toggleAirCondition();
        break;
      case 0x03: // auto button
        this->toggleAutomatic();
        break;
      case 0x04: // recirculation button
        this->toggleRecirculation();
        break;
      case 0x05: // windshield heating button
        
        break;
      case 0x06: // rear window heating button
        this->toggleRearHeater();
        break;
      case 0x07: // mode button
        
        break;
      case 0x08: // temperature knob
        if (payloadBuffer->available() > 0) {
          //payloadBuffer->readByte().data;
        }
        break;
      case 0x09: // fan level
        if (payloadBuffer->available() > 0) {
          //(uint8_t) (payloadBuffer->readByte().data * 8);
        }
        break;
    }
  }
private:
  SerialDataPacket<ClimateControl> *climateControl = new SerialDataPacket<ClimateControl>(0x73, 0x63);
  
  uint8_t fanLevel = 0;
  uint8_t temperatureLevel = 0;
  uint8_t airDuctSetting = 0;

  boolean isAirConditioning = false;
  boolean isRecirculating   = false;
  /*
   * In a real-world application these pointers should be
   * initialized in constructor deleted in destructor.
   * Since this class will only be created once, we do not care. :-P
   */
  AnalogOutput *temperatureDial     = new AnalogOutput(14, LOW);
  AnalogOutput *fanDial             = new AnalogOutput(30, LOW);
  AnalogOutput *airductDial         = new AnalogOutput(35, LOW);
  TimedOutput *rearHeaterButton     = new TimedOutput(new AnalogOutput(25));
  TimedOutput *recirculationButton  = new TimedOutput(new AnalogOutput(26));
  TimedOutput *airConditionButton   = new TimedOutput(new AnalogOutput(48));

  DigitalSensor *rearHeaterLed    = new DigitalSensor(32, 20, LOW, INPUT_PULLUP);
  DigitalSensor *freshAirLed      = new DigitalSensor(31, 20, LOW, INPUT_PULLUP);
  DigitalSensor *recirculationLed = new DigitalSensor(29, 20, LOW, INPUT_PULLUP);
  DigitalSensor *airConditionLed  = new DigitalSensor(47, 20, LOW, INPUT_PULLUP);

  static const int DialMinimum   = 0.33 / 5 * 1024;
  static const int DialMaximum   = 1024;
  static const int ButtonVoltage = 3.89 / 5 * 1024;

  static const uint8_t TempMin = 18;
  static const uint8_t TempMax = 32;

  static const uint8_t DialStepsTemperature = 31;
  static const uint8_t DialStepsFan         = 27;
};

#endif
