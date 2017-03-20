/***** wavetable.cpp *****/

#include "wavetable.h"
#include <array>
#include <Bela.h>
#include <math_neon.h>


wavetable::wavetable() {
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
	
	wavetable0ReadPointer = 0;
	wavetable0WritePointer = 0;
}

void wavetable::butterworthLowpassFilter0100SixthOrder(const double src[], double dest[], int size)
{   
    const int NZEROS = 6;
    const int NPOLES = 6;
    const double GAIN = 2.936532839e+03;
 
    double xv[NZEROS+1] = {0.0}, yv[NPOLES+1] = {0.0};
 
    for (int i = 0; i < size; i++)
    { 
        xv[0] = xv[1]; xv[1] = xv[2]; xv[2] = xv[3]; xv[3] = xv[4]; xv[4] = xv[5]; xv[5] = xv[6]; 
        xv[6] = src[i] / GAIN;
        yv[0] = yv[1]; yv[1] = yv[2]; yv[2] = yv[3]; yv[3] = yv[4]; yv[4] = yv[5]; yv[5] = yv[6]; 
        yv[6] =   (xv[0] + xv[6]) + 6.0 * (xv[1] + xv[5]) + 15.0 * (xv[2] + xv[4])
                     + 20.0 * xv[3]
                     + ( -0.0837564796 * yv[0]) + (  0.7052741145 * yv[1])
                     + ( -2.5294949058 * yv[2]) + (  4.9654152288 * yv[3])
                     + ( -5.6586671659 * yv[4]) + (  3.5794347983 * yv[5]);
        dest[i] = yv[6];
    }
}

void wavetable::antiAliasWavetables(float baseTable[]) {
	for (int i = 1; i < NUM_WAVETABLES_PER_VOICE; i++) {
		lowPassFilter(wavetableContainer[i - 1], wavetableContainer[i], 10);
	}
}

void wavetable::lowPassFilter(float rawData[], float* output, float LPF_Beta) {
    for (int i = 0; i < WAVETABLE_SIZE; i++) {
       output[i] = output[i] - (LPF_Beta * (output[i] - rawData[i]));
	}
}

void wavetable::generateSawtooth() {
	float sawtoothInterval = 2.0 / (float)WAVETABLE_SIZE;
	float sawtoothValue = -1.0;
	for (int i = 0; i < WAVETABLE_SIZE; i++) {
		wavetable0[i] = sawtoothValue;
		sawtoothValue += sawtoothInterval;
	}
	antiAliasWavetables(wavetable0);
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
	antiAliasWavetables(wavetable0);
}

void wavetable::generateSine() {
	float twoPi = 6.28318530718;
	float sineInterval = twoPi / (float)WAVETABLE_SIZE;
	float sineValue = 0.0;
	for (int i = 0; i < WAVETABLE_SIZE; i++) {
		wavetable0[i] = sinf_neon(sineValue);
		sineValue += sineInterval;
	}
	antiAliasWavetables(wavetable0);
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
	antiAliasWavetables(wavetable0);
}

