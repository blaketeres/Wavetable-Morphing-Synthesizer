/***** morphedWavetable.h *****/

#ifndef MORPHEDWAVETABLE_H
#define MORPHEDWAVETABLE_H

#include "wavetable.h"

#define NUM_MORPH_TABLES 64

class morphedWavetable : public wavetable {
public:
	morphedWavetable(wavetable a, wavetable b);
	
	void buildWavetables(wavetable a, wavetable b);
	float linearInterpolate(float a, float b, float index);
	
private:
	wavetable morphedWavetableContainer[NUM_MORPH_TABLES];
};

#endif