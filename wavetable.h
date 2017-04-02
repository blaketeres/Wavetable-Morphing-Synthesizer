/***** wavetable.h *****/

#ifndef WAVETABLE_H
#define WAVETABLE_H

#define WAVETABLE_SIZE 4096
#define NUM_WAVETABLES_PER_VOICE 17
#define TWO_PI 6.28318530718

#include <array>
#include <cmath>
#include <math_neon.h>
#include <vector>

class wavetable
{
public:
	wavetable(int basicWaveform = -1);
	
	void fillSawtooth();
	void fillSquare();
	void fillTriangle();
	void fillSine();
	void fillOtherWaveform(std::vector<float>& listOfHarmonics, std::vector<float>& listOfAmplitudes);
	
	void generateHarmonic(float* wavetable, float harmonicMultiple, float amplitude);
	
	void getPitch(float potInput);
	void chooseWavetable(float pitchValue);
	
	float getTableOut();
	float linearInterpolate();
	
	float* wavetableContainer[NUM_WAVETABLES_PER_VOICE];
	
private:
	float wavetable0[WAVETABLE_SIZE];
	float wavetable1[WAVETABLE_SIZE];
	float wavetable2[WAVETABLE_SIZE];
	float wavetable3[WAVETABLE_SIZE];
	float wavetable4[WAVETABLE_SIZE];
	float wavetable5[WAVETABLE_SIZE];
	float wavetable6[WAVETABLE_SIZE];
	float wavetable7[WAVETABLE_SIZE];
	float wavetable8[WAVETABLE_SIZE];
	float wavetable9[WAVETABLE_SIZE];
	float wavetable10[WAVETABLE_SIZE];
	float wavetable11[WAVETABLE_SIZE];
	float wavetable12[WAVETABLE_SIZE];
	float wavetable13[WAVETABLE_SIZE];
	float wavetable14[WAVETABLE_SIZE];
	float wavetable15[WAVETABLE_SIZE];
	float wavetable16[WAVETABLE_SIZE];
	
	float readIndex;
	float* currentWavetable;
	
	float pitch;
	float cutoffFreq;
};

#endif