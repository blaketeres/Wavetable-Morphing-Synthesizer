/***** morphedWavetable.h *****/

#ifndef MORPHEDWAVETABLE_H
#define MORPHEDWAVETABLE_H

#include "wavetable.h"

class morphedWavetable : public wavetable {
public:
	// morph 2 tables
	morphedWavetable(wavetable* a, wavetable* b, int sampleRate);
	
	// morph 3 tables
	morphedWavetable(wavetable* a, wavetable* b, wavetable* c, int sampleRate);
	
	//morph 4 tables
	morphedWavetable(wavetable* a, wavetable* b, wavetable* c, wavetable* d, int sampleRate);
	
	float outputMorph(float timeInSeconds, int morphType);
	
private:

	wavetable* container[4];
	
	enum MorphType {backAndForth, fullCircle, random};
	
	float multiplier;
	float multiplierInterval;

	int currentTable;
	int morphType;
	int numTables;
};

#endif