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

void wavetable::butterworthLowpassFilter0100SixthOrder(const float src[], float* dest, int size)
{   
    const int NZEROS = 6;
    const int NPOLES = 6;
    const float GAIN = 4.004448900e+05;
    float xv[NZEROS+1] = {0.0}, yv[NPOLES+1] = {0.0};
 
    for (int i = 0; i < size; i++)
    { 
        xv[0] = xv[1]; xv[1] = xv[2]; xv[2] = xv[3]; xv[3] = xv[4]; xv[4] = xv[5]; xv[5] = xv[6]; 
        xv[6] = src[i] / GAIN;
        yv[0] = yv[1]; yv[1] = yv[2]; yv[2] = yv[3]; yv[3] = yv[4]; yv[4] = yv[5]; yv[5] = yv[6]; 
        yv[6] =   (xv[0] + xv[6]) + 6.0 * (xv[1] + xv[5]) + 15.0 * (xv[2] + xv[4])
                     + 20.0 * xv[3]
                     + ( -0.3774523864 * yv[0]) + (  2.6310551285 * yv[1])
                     + ( -7.6754745482 * yv[2]) + ( 11.9993158160 * yv[3])
                     + (-10.6070421840 * yv[4]) + (  5.0294383514 * yv[5]);
        dest[i] = yv[6];
    }
}

void wavetable::antiAliasWavetables() {
	/*
	int cutoffFreq = 10000;
	float empty[WAVETABLE_SIZE];
	for (int i = 1; i < NUM_WAVETABLES_PER_VOICE; i++) {
		std::fill_n(empty, WAVETABLE_SIZE, 0);
		float cutoffSample = WAVETABLE_SIZE * cutoffFreq / 44100;
		cutoffFreq /= 2;
		//lowPassFilter(wavetableContainer[i - 1], wavetableContainer[i], 40);
		//butterworthLowpassFilter0100SixthOrder(wavetableContainer[i - 1], wavetableContainer[i], WAVETABLE_SIZE);
		fft(WAVETABLE_SIZE, wavetableContainer[i], empty);
		if (i < cutoffSample) {
			wavetableContainer[i - 1][i] = 1.0;
		}
		else {
			wavetableContainer[i - 1][i] = 1.0;
		}
	}
	*/
}

void wavetable::lowPassFilter(float rawData[], float* output, float LPF_Beta) {
    for (int i = 0; i < WAVETABLE_SIZE; i++) {
       output[i] = output[i] - (LPF_Beta * (output[i] - rawData[i]));
	}
}

void wavetable::fft(int N, float *ar, float *ai)
/*
 in-place complex fft
 
 After Cooley, Lewis, and Welch; from Rabiner & Gold (1975)
 
 program adapted from FORTRAN 
 by K. Steiglitz  (ken@princeton.edu)
 Computer Science Dept. 
 Princeton University 08544          */
{    
    int i, j, k, L;            /* indexes */
    int M, TEMP, LE, LE1, ip;  /* M = log N */
    int NV2, NM1;
    float t;               /* temp */
    float Ur, Ui, Wr, Wi, Tr, Ti;
    float Ur_old;
    
    // if ((N > 1) && !(N & (N - 1)))   // make sure we have a power of 2
    
    NV2 = N >> 1;
    NM1 = N - 1;
    TEMP = N; /* get M = log N */
    M = 0;
    while (TEMP >>= 1) ++M;
    
    /* shuffle */
    j = 1;
    for (i = 1; i <= NM1; i++) {
        if(i<j) {             /* swap a[i] and a[j] */
            t = ar[j-1];     
            ar[j-1] = ar[i-1];
            ar[i-1] = t;
            t = ai[j-1];
            ai[j-1] = ai[i-1];
            ai[i-1] = t;
        }
        
        k = NV2;             /* bit-reversed counter */
        while(k < j) {
            j -= k;
            k /= 2;
        }
        
        j += k;
    }
    
    LE = 1.;
    for (L = 1; L <= M; L++) {            // stage L
        LE1 = LE;                         // (LE1 = LE/2) 
        LE *= 2;                          // (LE = 2^L)
        Ur = 1.0;
        Ui = 0.; 
        Wr = cosf_neon(M_PI/(float)LE1);
        Wi = -sinf_neon(M_PI/(float)LE1); // Cooley, Lewis, and Welch have "+" here
        for (j = 1; j <= LE1; j++) {
            for (i = j; i <= N; i += LE) { // butterfly
                ip = i+LE1;
                Tr = ar[ip-1] * Ur - ai[ip-1] * Ui;
                Ti = ar[ip-1] * Ui + ai[ip-1] * Ur;
                ar[ip-1] = ar[i-1] - Tr;
                ai[ip-1] = ai[i-1] - Ti;
                ar[i-1]  = ar[i-1] + Tr;
                ai[i-1]  = ai[i-1] + Ti;
            }
            Ur_old = Ur;
            Ur = Ur_old * Wr - Ui * Wi;
            Ui = Ur_old * Wi + Ui * Wr;
        }
    }
}

void wavetable::generateSawtooth() {
	float sawtoothInterval = 2.0 / (float)WAVETABLE_SIZE;
	float sawtoothValue = -1.0;
	for (int i = 0; i < WAVETABLE_SIZE; i++) {
		wavetable0[i] = sawtoothValue;
		sawtoothValue += sawtoothInterval;
	}
	antiAliasWavetables();
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
	antiAliasWavetables();
}

void wavetable::generateSine() {
	float twoPi = 6.28318530718;
	float sineInterval = twoPi / (float)WAVETABLE_SIZE;
	float sineValue = 0.0;
	for (int i = 0; i < WAVETABLE_SIZE; i++) {
		wavetable0[i] = sinf_neon(sineValue);
		sineValue += sineInterval;
	}
	antiAliasWavetables();
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
	antiAliasWavetables();
}

