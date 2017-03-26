/***** wavetable.cpp *****/

#include "wavetable.h"

int numHarmonicsPerTable[11] = {1024, 512, 256, 128, 64, 32, 16, 8, 4, 2, 1};

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
		case 0: fillSawtooth(); break;
		case 1: fillSquare(); break;
		case 2: fillTriangle(); break;
		case 3: fillSine(); break;
	}
	
}

float wavetable::getTableOutAndInc() {
	
	float out = linearInterpolate();
	readIndex += pitch;
	
	if (readIndex >= WAVETABLE_SIZE)
		readIndex = readIndex - WAVETABLE_SIZE;
		
	return out;
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

void wavetable::fillSawtooth() {
	for (int i = 0; i < NUM_WAVETABLES_PER_VOICE; i++) {
		float amplitude;
		for (int j = 1; j < numHarmonicsPerTable[i]; j++) {
			amplitude = 1.0 / j;
			generateHarmonic(wavetableContainer[i], j, amplitude);
		}
	}
}


void wavetable::fillSquare() {
	for (int i = 0; i < NUM_WAVETABLES_PER_VOICE; i++) {
		float amplitude;
		for (int j = 1; j < numHarmonicsPerTable[i]; j += 2) {
			amplitude = 1.0 / j;
			generateHarmonic(wavetableContainer[i], j, amplitude);
		}
	}
}

void wavetable::fillTriangle() {
	for (int i = 0; i < NUM_WAVETABLES_PER_VOICE; i++) {
		float amplitude;
		for (int j = 1; j < numHarmonicsPerTable[i]; j += 2) {
			amplitude = 1.0 / (j * j);
			generateHarmonic(wavetableContainer[i], j, amplitude);
		}
	}
}

void wavetable::fillSine() {
	float twoPi = 6.28318530718;
	float sineInterval = twoPi / (float)WAVETABLE_SIZE;
	float sineValue = 0.0;
	for (int i = 0; i < WAVETABLE_SIZE; i++) {
		wavetable0[i] = sinf_neon(sineValue);
		sineValue += sineInterval;
	}
	for (int i = 1; i < NUM_WAVETABLES_PER_VOICE; i++) {
		wavetableContainer[i] = wavetable0;
	}
}

void wavetable::generateHarmonic(float* wavetable, int harmonicMultiple, float amplitude) {
	float twoPi = 6.28318530718;
	float sineInterval = twoPi / ((float)WAVETABLE_SIZE / (float)harmonicMultiple);
	float sineValue = 0.0;
	for (int i = 0; i < WAVETABLE_SIZE; i++) {
		wavetable[i] = wavetable[i] + (sinf_neon(sineValue) * amplitude);
		sineValue += sineInterval;
	}
}

