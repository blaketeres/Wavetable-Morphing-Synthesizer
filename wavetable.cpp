/***** wavetable.cpp *****/

#include "wavetable.h"

#include <Bela.h>
#include <math_neon.h>


wavetable::wavetable() {
	
}

void wavetable::antiAliasWavetables(wavetable &baseTable) {
	for (int i = 0; i < NUM_WAVETABLES_PER_VOICE; i++) {
		// FFT(wavetableContainer[i]);
	}
}
/*
void lowPassFilter() {
	int RawData;
	float SmoothData;
	float LPF_Beta = 0.025; // 0<ß<1

    // LPF: Y(n) = (1-ß)*Y(n-1) + (ß*X(n))) = Y(n-1) - (ß*(Y(n-1)-X(n)));
    while(1) {
       // Function that brings Fresh Data into RawData
       RawData = GetRawData();
       SmoothData = SmoothData - (LPF_Beta * (SmoothData - RawData));
	}
}
*/
void wavetable::generateSawtooth() {
	float sawtoothInterval = 2.0 / (float)WAVETABLE_SIZE;
	float sawtoothValue = -1.0;
	for (int i = 0; i < WAVETABLE_SIZE; i++) {
		wavetable0[i] = sawtoothValue;
		sawtoothValue += sawtoothInterval;
	}
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
}

void wavetable::generateSine() {
	float twoPi = 6.28318530718;
	float sineInterval = twoPi / (float)WAVETABLE_SIZE;
	float sineValue = 0.0;
	for (int i = 0; i < WAVETABLE_SIZE; i++) {
		wavetable0[i] = sinf_neon(sineValue);
		sineValue += sineInterval;
	}
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
}

