/***** wavetable.cpp *****/

#include "wavetable.h"
#include <array>
#include <Bela.h>
#include <math_neon.h>


wavetable::wavetable(int basicWaveform) {
	
	readIndex = 0.0;
	
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
	
	switch(basicWaveform) {
		case 0: generateSawtooth(); break;
		case 1: generateSquare(); break;
		case 2: generateTriangle(); break;
		case 3: generateSine(); break;
	}
}

float wavetable::getTableOutAndInc() {
	
	float out = linearInterpolate();
	readIndex += pitch;
	
	if (readIndex >= WAVETABLE_SIZE)
		readIndex = readIndex - WAVETABLE_SIZE;
		
	return out;
}


void wavetable::buildWavetables() {
	for (int i = 1; i < NUM_WAVETABLES_PER_VOICE; i++) {
		wavetableContainer[i] = wavetable0;
	}
}

void wavetable::chooseWaveTable(float pitchValue) {
	
	float cutoffFreq = 44100 / (WAVETABLE_SIZE / pitchValue);
	
	if (cutoffFreq < 20) {currentWavetable = wavetableContainer[0]; return;}
	if (cutoffFreq < 40) {currentWavetable = wavetableContainer[1]; return;}
	if (cutoffFreq < 80) {currentWavetable = wavetableContainer[2]; return;}
	if (cutoffFreq < 160) {currentWavetable = wavetableContainer[3]; return;}
	if (cutoffFreq < 320) {currentWavetable = wavetableContainer[4]; return;}
	if (cutoffFreq < 640) {currentWavetable = wavetableContainer[5]; return;}
	if (cutoffFreq < 1280) {currentWavetable = wavetableContainer[6]; return;}
	if (cutoffFreq < 2560) {currentWavetable = wavetableContainer[7]; return;}
	if (cutoffFreq < 5120) {currentWavetable = wavetableContainer[8]; return;}
	if (cutoffFreq < 10240) {currentWavetable = wavetableContainer[9]; return;}
	currentWavetable = wavetableContainer[10];
}

float wavetable::linearInterpolate() {
	float a = currentWavetable[(int)readIndex];
	float b = currentWavetable[(int)(readIndex + 1) % WAVETABLE_SIZE];
	float fraction = readIndex - (int)readIndex;
	float difference = b - a;
	float toAdd = fraction * difference;
	return (a + toAdd);
}

void wavetable::getPitch(float potInput) {
	pitch = powf_neon(potInput + 1, 11.0);
	chooseWaveTable(pitch);
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

