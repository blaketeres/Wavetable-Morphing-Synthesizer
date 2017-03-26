/***** morphedWavetable.cpp *****/

#include "morphedWavetable.h"

morphedWavetable::morphedWavetable(wavetable* a, wavetable* b, int sampleRate) {
	
	tableA = a;
	tableB = b;
	
	multiplier = 0.0;
	
	inc = false;
}

float morphedWavetable::outputMorph(float timeInSeconds) {
	
	int numSamplesToCrossFade = (int)(timeInSeconds * 44100);
	multiplierInterval = 1.0 / numSamplesToCrossFade;
	
	float outA = tableA->getTableOutAndInc() * multiplier;
	float outB = tableB->getTableOutAndInc() * (-multiplier + 1);
	
	if (inc) multiplier += multiplierInterval;
	if (!inc) multiplier -= multiplierInterval;
	
	if (multiplier >= 1.0)
		inc = false;
	if (multiplier <= 0.0)
		inc = true;
	
	return outA + outB;
}

