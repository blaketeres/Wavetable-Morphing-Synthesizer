/***** morphedWavetable.cpp *****/

#include "morphedWavetable.h"

morphedWavetable::morphedWavetable(wavetable* a, wavetable* b, int sampleRate) {

	container[0] = a;
	container[1] = b;
	
	morphType = backAndForth;
	multiplier = 0.0;
	numTables = 2;
	currentTable = 0;
}

morphedWavetable::morphedWavetable(wavetable* a, wavetable* b, wavetable* c, int sampleRate) {
	
	container[0] = a;
	container[1] = b;
	container[2] = c;
	
	morphType = backAndForth;
	multiplier = 0.0;
	numTables = 3;
	currentTable = 0;
}

morphedWavetable::morphedWavetable(wavetable* a, wavetable* b, wavetable* c, wavetable* d, int sampleRate) {
	
	container[0] = a;
	container[1] = b;
	container[2] = c;
	container[3] = d;
	
	morphType = backAndForth;
	multiplier = 0.0;
	numTables = 4;
	currentTable = 0;
}

float morphedWavetable::outputMorph(float timeInSeconds, int morphType) {
	
	int numSamplesToCrossFade = (int)(timeInSeconds * 44100);
	multiplierInterval = 1.0 / (numSamplesToCrossFade * (float)numTables);
	
	float outA;
	float outB;
	float outC;
	float outD;
	
	switch(morphType) {
		
		case backAndForth:
		{
			outA = container[currentTable % numTables]->getTableOutAndInc() * (-multiplier + 1);
			outB = container[(currentTable + 1) % numTables]->getTableOutAndInc() * multiplier;
			if (numTables == 3) outC = container[currentTable % numTables]->getTableOutAndInc() * (-multiplier + 1);
			if (numTables == 4) {
				outC = container[currentTable % numTables]->getTableOutAndInc() * (-multiplier + 1);
				outD = container[(currentTable + 1) % numTables]->getTableOutAndInc() * multiplier;
			}
			multiplier += multiplierInterval;
			
			if (multiplier >= 1.0 ) {
				currentTable += 1;
				currentTable = currentTable % (numTables);
				multiplier = 0.0;
			}

			return outA + outB;
		}
		case fullCircle: break;
		case random: break;
	}
}

