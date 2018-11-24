#ifndef _LEDCONTROL_H_
#define _LEDCONTROL_H_

//#include <FastLED.h>

template <int START_INDEX, int NUM_LEDS>
class PixelGroup {
  private:
    CRGB * leds;
    CFastLED * fastLed;
    int absoluteLimit = START_INDEX + NUM_LEDS - 1;

    void setLedInternal(int index, struct CRGB value) {
      int absoluteIndex = START_INDEX + index;
      if (absoluteIndex < START_INDEX || absoluteIndex > absoluteLimit) {
        // Index out of bounds. Lets just fail silently.
        return;
      }

      this->leds[absoluteIndex] = value;
    }
  public:
    PixelGroup(struct CRGB * leds, CFastLED * fastLed) {
      this->leds = leds;
      this->fastLed = fastLed;
    }
    
    void set(struct CRGB value) {
      for (int index = 0; index < NUM_LEDS; index++) {
        this->setLedInternal(index, value);
      }
      this->fastLed->show();
    }
    
    void setLed(int index, struct CRGB value) {
      this->setLedInternal(index, value);
      this->fastLed->show();
    }
};

#endif
