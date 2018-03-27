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
  Hvac() {
    this->climateControl->payload()->isAcOn               = false;
    this->climateControl->payload()->isAuto               = false;
    this->climateControl->payload()->isAirductWindshield  = false;
    this->climateControl->payload()->isAirductFace        = false;
    this->climateControl->payload()->isAirductFeet        = false;
    this->climateControl->payload()->isWindshieldHeating  = false;
    this->climateControl->payload()->isRearWindowHeating  = false;
    this->climateControl->payload()->isRecirculation      = false;
    this->climateControl->payload()->fanLevel             = 0;
    this->climateControl->payload()->desiredTemperature   = 21 * 2;
  }
  void update() {
    this->rearHeaterButton->update();
    this->recirculationButton->update();
    this->airConditionButton->update();

    if (millis() - this->lastUpdate > UpdateRate) {
       this->climateControl->payload()->isRecirculation     = this->recirculationLed->getState();
       this->climateControl->payload()->isAcOn              = this->airConditionLed->getState();
       this->climateControl->payload()->isRearWindowHeating = this->rearHeaterLed->getState();

       this->climateControl->serialize(Serial);

       this->lastUpdate = millis();
    }
  }
  void toggleAirCondition() {
    this->airConditionButton->set(ButtonVoltage, ButtonPressDuration);
  }
  void toggleRearHeater() {
    this->rearHeaterButton->set(ButtonVoltage, ButtonPressDuration);
  }
  void toggleRecirculation() {
    this->recirculationButton->set(ButtonVoltage, ButtonPressDuration);
  }
  void toggleAutomatic() {
    this->setFanLevel(1);
  }
  void setTemperature(uint8_t temperature) {
    if (temperature > TempMax) {
      temperature = TempMax;
    }
    if (temperature < TempMin) {
      temperature = TempMin;
    }

    // (DialMaximum - DialMinimum) / DialStepsTemperature * level + DialMaximum)
    uint8_t level = (temperature - TempMin) / (TempMax - TempMin) * (DialMaximum - DialMinimum) + DialMinimum;
    
    this->climateControl->payload()->desiredTemperature = temperature;
    this->temperatureDial->set(level);
  }
  void setFanLevel(uint8_t level) {   
    if (level > DialStepsFan) {
      level = DialStepsFan;
    }
    if (level < 0) {
      level = 0;
    }

    this->climateControl->payload()->fanLevel = level;
    this->fanDial->set((DialMaximum - DialMinimum) / DialStepsFan * level + DialMaximum);
  }
  void write(uint8_t buttonId, BinaryBuffer *payloadBuffer) {
    switch (buttonId) {
      case 0x01: // OFF BUTTON
        this->setFanLevel(0);
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
        // set mode to defrosting (maximum)
        break;
      case 0x06: // rear window heating button
        this->toggleRearHeater();
        break;
      case 0x07: // mode button
        // toggle between modes except auto and defrost
        break;
      case 0x08: // temperature knob
        if (payloadBuffer->available() > 0) {
          this->setTemperature(payloadBuffer->readByte().data);
        }
        break;
      case 0x09: // fan level
        if (payloadBuffer->available() > 0) {
          this->setFanLevel(payloadBuffer->readByte().data);
        }
        break;
    }
  }
private:
  SerialDataPacket<ClimateControl> *climateControl = new SerialDataPacket<ClimateControl>(0x73, 0x63);
  
  uint8_t airDuctSetting = 0;

  unsigned long lastUpdate = 0;
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
  
  static const unsigned int ButtonPressDuration = 300;

  static const uint8_t TempMin = 18;
  static const uint8_t TempMax = 32;

  static const uint8_t DialStepsTemperature = 31;
  static const uint8_t DialStepsFan         = 27;
  
  static const unsigned int UpdateRate = 250;
};

#endif
