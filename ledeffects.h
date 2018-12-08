#ifndef LEDEFFECTS_H_
#define LEDEFFECTS_H_

#include "ledcontrol.h"

class SweepEffect: public PixelEffect {
private:
	unsigned int tailLength = 0;
	struct CRGB color;
	unsigned int speed = 1;
	unsigned int currentPixel = 0;
	unsigned long totalDeltaTime = 0;
public:
	SweepEffect(struct CRGB color, unsigned int speed) :
			PixelEffect(60) {
		this->color = color;
		this->speed = speed;
	}
	void onUpdate(struct CRGB * pixel, unsigned int pixelIndex,
			unsigned int pixelCount, unsigned long deltaTime) {
		float factor = 0;

		if (pixelIndex >= this->currentPixel - this->tailLength
				&& pixelIndex <= this->currentPixel) {
			factor = 1 - ((this->currentPixel - pixelIndex) / (float)this->tailLength);
		}

		pixel->r = this->color.r * factor;
		pixel->g = this->color.g * factor;
		pixel->b = this->color.b * factor;

		if (this->totalDeltaTime > 1000 / (float)speed) {
			if (this->currentPixel < pixelCount - 1) {
				this->currentPixel++;
			} else {
				this->currentPixel = 0;
			}
			this->totalDeltaTime = 0;
		} else {
			this->totalDeltaTime += deltaTime;
		}
	}
	void setTailLength(unsigned int tailLength) {
		this->tailLength = tailLength;
	}
};

#endif /* LEDEFFECTS_H_ */
