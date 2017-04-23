/***** morphedWavetable.h *****/

#ifndef MORPHEDWAVETABLE_H
#define MORPHEDWAVETABLE_H

#include "wavetable.h"
#include <stdlib.h>
#include <time.h>

class morphedWavetable : public wavetable {
public:
	// morph 2 tables
	morphedWavetable(wavetable* a, wavetable* b);
	
	// morph 3 tables
	morphedWavetable(wavetable* a, wavetable* b, wavetable* c);
	
	//morph 4 tables
	morphedWavetable(wavetable* a, wavetable* b, wavetable* c, wavetable* d);
	
	// Give names to types of morphing
	enum MorphType {backAndForth, fullCircle, random, hard};
	
	// Grab morphed table output
	float outputMorph(int morphType);
	
	// Generate a random integer
	void getRandomInt();
	
	// Modify morph speed from pot input
	void setMorphSpeed(float potInput);
	
	// Modify morph mod from pot input
	void setMorphMod(float potInput);

private:

	wavetable* container[4];
	
	float phaser;
	float phaserInterval;
	float morphSpeed;
	
	float mod;
	float modInterval;
	float modPhaser;
	
	int currentTable;
	int backAndForthIndex;
	int morphType;
	int numTables;
	
	bool countUp;
	
	int randomInt;
	int lastRandomInt;
};

#endif