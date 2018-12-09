#ifndef LEDEFFECTS_H_
#define LEDEFFECTS_H_

#include "ledcontrol.h"

class SweepEffect: public PixelEffect {
private:
	unsigned int tailLength = 0;
	struct CRGB color;
	float speed = 1;
	unsigned int currentPixel = 0;
	unsigned long totalDeltaTime = 0;
public:
	SweepEffect(struct CRGB color, float speed) :
			PixelEffect(60) {
		this->color = color;
		this->speed = speed;
	}
	void beforeFrame(unsigned int pixelCount, unsigned long deltaTime) {
		(void)(pixelCount);
		(void)(deltaTime);
	}
	void afterFrame(unsigned int pixelCount, unsigned long deltaTime) {
		if (this->totalDeltaTime > 1000 / speed) {
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
	void onFrame(struct CRGB * pixel, unsigned int pixelIndex,
			unsigned int pixelCount, unsigned long deltaTime) {
		(void)(pixelCount);
		(void)(deltaTime);
		float factor = 0;

		if (pixelIndex >= constrain((long)this->currentPixel - this->tailLength, 0, this->currentPixel)
				&& pixelIndex <= this->currentPixel) {
			factor = 1 - ((this->currentPixel - pixelIndex) / (float)this->tailLength);
		}

		pixel->r = this->color.r * factor;
		pixel->g = this->color.g * factor;
		pixel->b = this->color.b * factor;
	}
	void setTailLength(unsigned int tailLength) {
		this->tailLength = tailLength;
	}
};

#endif /* LEDEFFECTS_H_ */
