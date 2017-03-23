/***** morphedWavetable.cpp *****/

#include "morphedWavetable.h"

morphedWavetable::morphedWavetable(wavetable a, wavetable b) {
	buildWavetables(a, b);
}

float morphedWavetable::linearInterpolate(float a, float b, float fraction) {
	float difference = b - a;
	float toAdd = fraction * difference;
	return (a + toAdd);
}

void morphedWavetable::buildWavetables(wavetable a, wavetable b) {
	
	morphedWavetableContainer[0] = a;
	
	float fraction;
	
	// loop through each empty table that needs to contain morph data
	for (int i = 1; i < NUM_MORPH_TABLES - 1; i++) {
		
		wavetable emptyTable;
		morphedWavetableContainer[i] = emptyTable;
		
		fraction = (float)i / (float)NUM_MORPH_TABLES;
		
		for (int j = 0; j < NUM_WAVETABLES_PER_VOICE; j++) {
			
			for (int k = 0; k < WAVETABLE_SIZE; k++) {
				
				float value = linearInterpolate(morphedWavetableContainer[0].wavetableContainer[j][k],
								morphedWavetableContainer[NUM_MORPH_TABLES - 1].wavetableContainer[j][k], fraction);
				morphedWavetableContainer[i].wavetableContainer[j][k] = value;
			}
		}
	}
	morphedWavetableContainer[NUM_MORPH_TABLES - 1] = b;
}
