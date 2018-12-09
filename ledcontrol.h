#ifndef _LEDCONTROL_H_
#define _LEDCONTROL_H_

//#include <FastLED.h>

class PixelGroupDefinition {
public:
	virtual ~PixelGroupDefinition() {};
	virtual void set(struct CRGB value) = 0;
	virtual void setLed(int index, struct CRGB value) = 0;
	virtual int getLedCount() = 0;
	virtual struct CRGB * getLed(int index) = 0;
};

template <int START_INDEX, int NUM_LEDS>
class PixelGroup : public PixelGroupDefinition {
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

    int getLedCount() {
      return NUM_LEDS;
    }

    struct CRGB * getLed(int index) {
      return &this->leds[START_INDEX + index];
    }
};

class PixelEffect {
private:
	uint8_t numGroups = 0;
	PixelGroupDefinition * groups[10];
	unsigned long lastUpdate = 0;
	unsigned long updateTime = 1000 / (float)60;
	unsigned int ledCount = 0;
	CFastLED * fastLed;
public:
	PixelEffect(uint8_t frameRate) {
		this->updateTime = 1000 / (float)frameRate;
		this->fastLed = fastLed;
	}
	virtual ~PixelEffect() {
	}
	void addGroup(PixelGroupDefinition * group) {
		this->groups[this->numGroups] = group;
		this->numGroups++;
		this->ledCount += group->getLedCount();
	}
	void update(CFastLED * fastLed) {
		unsigned long deltaTime = millis() - lastUpdate;
		if (deltaTime >= this->updateTime) {
			this->beforeFrame(this->ledCount, deltaTime);
			unsigned int pixelIndex = 0;
			for (PixelGroupDefinition * group : this->groups) {
				if (group != NULL) {
					for (int i = 0; i < group->getLedCount(); i++) {
						onFrame(group->getLed(i), pixelIndex, this->ledCount,
								deltaTime);
						pixelIndex++;
					}
				}
			}
			this->afterFrame(this->ledCount, deltaTime);
			fastLed->show();
			this->lastUpdate = millis();
		}
	}
	virtual void beforeFrame(unsigned int pixelCount,
			unsigned long deltaTime) = 0;
	virtual void onFrame(struct CRGB * pixel, unsigned int pixelIndex,
			unsigned int pixelCount, unsigned long deltaTime) = 0;
	virtual void afterFrame(unsigned int pixelCount,
			unsigned long deltaTime) = 0;
};

#endif
