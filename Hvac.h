#ifndef HVAC_H
#define HVAC_H

#include "Button.h"
#include "Output.h"

class Hvac {
public:
  Hvac() {}
  void update() {
    this->rearHeaterButton->update();
    this->recirculationButton->update();
    this->airConditionButton->update();
  }
  void toggleAirCondition() {
    this->airConditionButton->set(ButtonVoltage, 100);
  }
  void toggleRearHeater() {
    this->airConditionButton->set(ButtonVoltage, 100);
  }
  void toggleRecirculation() {
    this->airConditionButton->set(ButtonVoltage, 100);
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
private:
  uint8_t fanLevel = 0;
  uint8_t temperatureLevel = 0;
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

  static const uint8_t DialStepsTemperature = 31;
  static const uint8_t DialStepsFan         = 27;
  
};

#endif
