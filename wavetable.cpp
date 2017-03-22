/***** wavetable.cpp *****/

#include "wavetable.h"
#include <array>
#include <Bela.h>
#include <math_neon.h>


wavetable::wavetable(int basicWaveform) {
	std::fill_n(wavetable0, WAVETABLE_SIZE, 0);
	std::fill_n(wavetable1, WAVETABLE_SIZE, 0);
	std::fill_n(wavetable2, WAVETABLE_SIZE, 0);
	std::fill_n(wavetable3, WAVETABLE_SIZE, 0);
	std::fill_n(wavetable4, WAVETABLE_SIZE, 0);
	std::fill_n(wavetable5, WAVETABLE_SIZE, 0);
	std::fill_n(wavetable6, WAVETABLE_SIZE, 0);
	std::fill_n(wavetable7, WAVETABLE_SIZE, 0);
	std::fill_n(wavetable8, WAVETABLE_SIZE, 0);
	std::fill_n(wavetable9, WAVETABLE_SIZE, 0);
	std::fill_n(wavetable10, WAVETABLE_SIZE, 0);

	wavetableContainer[0] = wavetable0;
	wavetableContainer[1] = wavetable1;
	wavetableContainer[2] = wavetable2;
	wavetableContainer[3] = wavetable3;
	wavetableContainer[4] = wavetable4;
	wavetableContainer[5] = wavetable5;
	wavetableContainer[6] = wavetable6;
	wavetableContainer[7] = wavetable7;
	wavetableContainer[8] = wavetable8;
	wavetableContainer[9] = wavetable9;
	wavetableContainer[10] = wavetable10;
	
	wavetableReadPointer = 0;
	wavetableWritePointer = 0;
	
	if (basicWaveform == 0) generateSawtooth();
	if (basicWaveform == 1) generateSquare();
	if (basicWaveform == 2) generateTriangle();
	if (basicWaveform == 3) generateSine();

}


void wavetable::buildWavetables() {
	for (int i = 1; i < NUM_WAVETABLES_PER_VOICE; i++) {
		wavetableContainer[i] = wavetable0;
	}
}

float* wavetable::chooseWaveTable(float pitchValue) {
	
	float cutoffFreq = 44100 / (WAVETABLE_SIZE / pitchValue);
	
	if (cutoffFreq < 20) return wavetableContainer[0];
	if (cutoffFreq < 40) return wavetableContainer[1];
	if (cutoffFreq < 80) return wavetableContainer[2];
	if (cutoffFreq < 160) return wavetableContainer[3];
	if (cutoffFreq < 320) return wavetableContainer[4];
	if (cutoffFreq < 640) return wavetableContainer[5];
	if (cutoffFreq < 1280) return wavetableContainer[6];
	if (cutoffFreq < 2560) return wavetableContainer[7];
	if (cutoffFreq < 5120) return wavetableContainer[8];
	if (cutoffFreq < 10240) return wavetableContainer[9];
	return wavetableContainer[10];
}

float wavetable::linearInterpolate(float* currentWavetable, float index) {
	float a = currentWavetable[(int)index];
	float b = currentWavetable[(int)(index + 1) % WAVETABLE_SIZE];
	float fraction = index - (int)index;
	float difference = b - a;
	float toAdd = fraction * difference;
	return (a + toAdd);
}


void wavetable::generateSawtooth() {
	float sawtoothInterval = 2.0 / (float)WAVETABLE_SIZE;
	float sawtoothValue = -1.0;
	for (int i = 0; i < WAVETABLE_SIZE; i++) {
		wavetable0[i] = sawtoothValue;
		sawtoothValue += sawtoothInterval;
	}
	buildWavetables();
}

void wavetable::generateSquare() {
	float max = 1.0;
	float min = -1.0;
	int half = WAVETABLE_SIZE / 2;
	for (int i = 0; i < WAVETABLE_SIZE; i++) {
		if (i < half) {
			wavetable0[i] = max;
		}
		else {
			wavetable0[i] = min;
		}
	}
	buildWavetables();
}

void wavetable::generateSine() {
	float twoPi = 6.28318530718;
	float sineInterval = twoPi / (float)WAVETABLE_SIZE;
	float sineValue = 0.0;
	for (int i = 0; i < WAVETABLE_SIZE; i++) {
		wavetable0[i] = sinf_neon(sineValue);
		sineValue += sineInterval;
	}
	buildWavetables();
}

void wavetable::generateTriangle() {
	float triangleInterval = 2.0 / (WAVETABLE_SIZE / 2);
	float triangleValue = -1.0;
	int half = WAVETABLE_SIZE / 2;
	for (int i = 0; i < WAVETABLE_SIZE; i++) {
		if (i < half) {
			wavetable0[i] = triangleValue;
			triangleValue += triangleInterval;
		}
		else {
			wavetable0[i] = triangleValue;
			triangleValue -= triangleInterval;
		}
	}
	buildWavetables();
}

