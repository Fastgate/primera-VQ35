#ifndef LEDEFFECTS_H_
#define LEDEFFECTS_H_

#include "ledcontrol.h"

void _simple_plot_(Stream &output, byte numData, int16_t data1, int16_t data2,
		int16_t data3, int16_t data4, int16_t data5, int16_t data6) {
	uint8_t plotBuffer[16];
	uint16_t pktSize;
	uint16_t numDataBytes = numData * sizeof(uint16_t); //Size of data in bytes. Does not include the header and size fields

	plotBuffer[0] = 0xAB;
	plotBuffer[1] = 0xCD; //SimPlot packet header. Indicates start of data packet
	plotBuffer[2] = lowByte(numDataBytes);
	plotBuffer[3] = highByte(numDataBytes);
	plotBuffer[4] = lowByte(data1);
	plotBuffer[5] = highByte(data1);
	plotBuffer[6] = lowByte(data2);
	plotBuffer[7] = highByte(data2);
	plotBuffer[8] = lowByte(data3);
	plotBuffer[9] = highByte(data3);
	plotBuffer[10] = lowByte(data4);
	plotBuffer[11] = highByte(data4);
	plotBuffer[12] = lowByte(data5);
	plotBuffer[13] = highByte(data5);
	plotBuffer[14] = lowByte(data6);
	plotBuffer[15] = highByte(data6);

	pktSize = (numData + 2) * sizeof(uint16_t); //Header bytes + size field bytes + data

	output.write((uint8_t *) plotBuffer, pktSize);
	//output.println();
}

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
	bool beforeFrame(unsigned int pixelCount, unsigned long deltaTime) {
		(void)(pixelCount);
		this->totalDeltaTime += deltaTime;
		return this->totalDeltaTime > 1000 / speed;
	}
	void afterFrame(unsigned int pixelCount, unsigned long deltaTime) {
		(void)(deltaTime);
		if (this->totalDeltaTime > 1000 / speed) {
			if (this->currentPixel < pixelCount - 1) {
				this->currentPixel++;
			} else {
				this->currentPixel = 0;
			}
			this->totalDeltaTime = 0;
			Serial.println("");
		}
	}
	void onFrame(struct CRGB * pixel, unsigned int pixelIndex,
			unsigned int pixelCount, unsigned long deltaTime) {
		(void)(pixelCount);
		(void)(deltaTime);
		float factor = 0;

		if (this->currentPixel - pixelIndex <= this->tailLength
				&& pixelIndex <= this->currentPixel) {
			factor = 1 - ((this->currentPixel - pixelIndex) / (float)this->tailLength);
		}

		pixel->r = this->color.r * factor;
		pixel->g = this->color.g * factor;
		pixel->b = this->color.b * factor;

		Serial.print(pixel->r);
		Serial.print("	");
	}
	void setTailLength(unsigned int tailLength) {
		this->tailLength = tailLength;
	}
};

#endif /* LEDEFFECTS_H_ */
