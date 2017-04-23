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
#include <algorithm>
#include <Bela.h>

class wavetable
{
public:
	wavetable(int basicWaveform = -1);
	
	// Generates a sawtooth wave in the base table (maximum harmonics)
	void fillSawtooth();
	
	// Generates a square wave in the base table (maximum harmonics)
	void fillSquare();
	
	// Generates a triangle wave in the base table (maximum harmonics)
	void fillTriangle();
	
	// Generates a sine wave in the base table (maximum harmonics)
	void fillSine();
	
	// Fills all wavetables based off of harmonics
	// found in the harmonics vectors
	void fillVectorWaveform(std::vector<int>& listOfHarmonics, std::vector<float>& listOfAmplitudes);
	
	// Generates a single harmonic (sine wave) off of a base
	// table given a multiple value and an amplitude
	void generateHarmonic(float* wavetable, unsigned int harmonicMultiple, float amplitude);
	
	// Normalizes a wavetable (-1.0 to 1.0)
	void normalize(float* wavetable);
	
	// Gets a value that is used to determine the speed
	// at which tables are read. Not related to frequency in Hz
	void getPitch(float potInput);
	
	// Chooses which wavetable to read from depending
	// on pitch. Tables with less harmonics will be read
	// as the pitch increases
	void chooseWavetable(float pitchValue);
	
	// Gets the current value of the table at the readIndex
	// and increments to the next interval as determined by the pitch
	float getTableOut();
	
	// Interpolation function called when the
	// readIndex is not an integer value
	float linearInterpolate();
	
	// Fills the tables with 0s
	void clearAllTables();
	
	// Gets current gain level of voice
	float getGain();
	
	// Sets current gain level of voice
	void setGain(float newGain);
	
	// Array of pointers to all of the wavetables
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
	
	float gain;
};

#endif