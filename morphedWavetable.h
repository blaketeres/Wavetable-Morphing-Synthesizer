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
	
	
	enum MorphType {backAndForth, fullCircle, random};
	float outputMorph(float timeInSeconds, int morphType);
	int positiveModulo(int i, int n);
	void getRandomInt();
	void setMorphSpeed(float potInput);

private:

	wavetable* container[4];
	
	float phaser;
	float phaserInterval;
	float morphSpeed;

	int currentTable;
	int backAndForthIndex;
	int morphType;
	int numTables;
	
	bool countUp;
	
	int randomInt;
	int lastRandomInt;
	
};

#endif
