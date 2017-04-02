/***** wavetable.cpp *****/

#include "wavetable.h"

int numHarmonicsPerTable[17] = {1024, 682, 455, 303, 202, 134, 89, 59, 39, 26, 17, 11, 7, 5, 4, 2, 1};

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
	std::fill_n(wavetable11, WAVETABLE_SIZE, 0);
	std::fill_n(wavetable12, WAVETABLE_SIZE, 0);
	std::fill_n(wavetable13, WAVETABLE_SIZE, 0);
	std::fill_n(wavetable14, WAVETABLE_SIZE, 0);
	std::fill_n(wavetable15, WAVETABLE_SIZE, 0);
	std::fill_n(wavetable16, WAVETABLE_SIZE, 0);

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
	wavetableContainer[11] = wavetable11;
	wavetableContainer[12] = wavetable12;
	wavetableContainer[13] = wavetable13;
	wavetableContainer[14] = wavetable14;
	wavetableContainer[15] = wavetable15;
	wavetableContainer[16] = wavetable16;
	
	switch(basicWaveform) {
		case 0: fillSawtooth(); break;
		case 1: fillSquare(); break;
		case 2: fillTriangle(); break;
		case 3: fillSine(); break;
	}
	
}

float wavetable::getTableOut() {
	
	float out = linearInterpolate();
	readIndex += pitch;
	
	if (readIndex >= WAVETABLE_SIZE)
		readIndex = readIndex - WAVETABLE_SIZE;
		
	return out;
}

void wavetable::chooseWavetable(float pitchValue) {
	
	cutoffFreq = 44100 / (WAVETABLE_SIZE / pitchValue);
	
	if (cutoffFreq < 20) {currentWavetable = wavetableContainer[0]; return;}
	if (cutoffFreq < 30) {currentWavetable = wavetableContainer[1]; return;}
	if (cutoffFreq < 45) {currentWavetable = wavetableContainer[2]; return;}
	if (cutoffFreq < 67) {currentWavetable = wavetableContainer[3]; return;}
	if (cutoffFreq < 101) {currentWavetable = wavetableContainer[4]; return;}
	if (cutoffFreq < 151) {currentWavetable = wavetableContainer[5]; return;}
	if (cutoffFreq < 227) {currentWavetable = wavetableContainer[6]; return;}
	if (cutoffFreq < 341) {currentWavetable = wavetableContainer[7]; return;}
	if (cutoffFreq < 512) {currentWavetable = wavetableContainer[8]; return;}
	if (cutoffFreq < 768) {currentWavetable = wavetableContainer[9]; return;}
	if (cutoffFreq < 1153) {currentWavetable = wavetableContainer[10]; return;}
	if (cutoffFreq < 1729) {currentWavetable = wavetableContainer[11]; return;}
	if (cutoffFreq < 2594) {currentWavetable = wavetableContainer[12]; return;}
	if (cutoffFreq < 3892) {currentWavetable = wavetableContainer[13]; return;}
	if (cutoffFreq < 5835) {currentWavetable = wavetableContainer[14]; return;}
	if (cutoffFreq < 11025) {currentWavetable = wavetableContainer[15]; return;}
	currentWavetable = wavetableContainer[16];
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
	pitch = powf(potInput + 1, 11.0);
	chooseWavetable(pitch);
}

void wavetable::fillSawtooth() {
	float amplitude;
	for (int i = 0; i < NUM_WAVETABLES_PER_VOICE; i++) {
		for (int j = 1; j < numHarmonicsPerTable[i] + 1; j++) {
			amplitude = 1.0 / j;
			generateHarmonic(wavetableContainer[i], j, amplitude);
		}
	}
}


void wavetable::fillSquare() {
	float amplitude;
	for (int i = 0; i < NUM_WAVETABLES_PER_VOICE; i++) {
		for (int j = 1; j < numHarmonicsPerTable[i] + 1; j += 2) {
			amplitude = 1.0 / j;
			generateHarmonic(wavetableContainer[i], j, amplitude);
		}
	}
}

void wavetable::fillTriangle() {
	float amplitude;
	for (int i = 0; i < NUM_WAVETABLES_PER_VOICE; i++) {
		for (int j = 1; j < numHarmonicsPerTable[i] + 1; j += 2) {
			amplitude = 1.0 / (j * j);
			generateHarmonic(wavetableContainer[i], j, amplitude);
		}
	}
}

void wavetable::fillSine() {
	float sineInterval = TWO_PI / (float)WAVETABLE_SIZE;
	float sineValue = 0.0;
	for (int i = 0; i < WAVETABLE_SIZE; i++) {
		wavetable0[i] = sinf_neon(sineValue);
		sineValue += sineInterval;
	}
	for (int i = 1; i < NUM_WAVETABLES_PER_VOICE; i++) {
		wavetableContainer[i] = wavetable0;
	}
}

void wavetable::fillOtherWaveform(std::vector<float>& listOfHarmonics, std::vector<float>& listOfAmplitudes) {
	if (listOfHarmonics.size() != listOfAmplitudes.size()) return;
	
	for (int i = 0; i < NUM_WAVETABLES_PER_VOICE; i++) {
		for (int j = 0; j < listOfHarmonics.size(); j++) {
			if (listOfHarmonics[j] <= numHarmonicsPerTable[i]) {
				generateHarmonic(wavetableContainer[i], listOfHarmonics[j], listOfAmplitudes[j]);
			}
		}
	}
}

void wavetable::generateHarmonic(float* wavetable, float harmonicMultiple, float amplitude) {
	float sineInterval = TWO_PI / ((float)WAVETABLE_SIZE / harmonicMultiple);
	float sineValue = 0.0;
	for (int i = 0; i < WAVETABLE_SIZE; i++) {
		wavetable[i] = wavetable[i] + (sinf_neon(sineValue) * amplitude);
		sineValue += sineInterval;
	}
}



