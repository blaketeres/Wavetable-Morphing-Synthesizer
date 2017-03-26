/***** morphedWavetable.h *****/

#ifndef MORPHEDWAVETABLE_H
#define MORPHEDWAVETABLE_H

#include "wavetable.h"

class morphedWavetable : public wavetable {
public:
	morphedWavetable(wavetable* a, wavetable* b, int sampleRate);
	
	float outputMorph(float timeInSeconds);
private:
	wavetable* tableA;
	wavetable* tableB;
	
	float multiplier;
	
	float multiplierInterval;
	
	bool inc;
};

#endif