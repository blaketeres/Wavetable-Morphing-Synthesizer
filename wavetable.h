/***** wavetable.h *****/

#ifndef WAVETABLE_H
#define WAVETABLE_H

#define WAVETABLE_SIZE 4096
#define NUM_WAVETABLES_PER_VOICE 11

class wavetable
{
public:
	wavetable();
	void antiAliasWavetables(wavetable &baseTable);
	void lowPassFilter();
	void generateSawtooth();
	void generateSquare();
	void generateTriangle();
	void generateSine();
	
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
	int wavetable0ReadPointer;
	int wavetable0WritePointer;
};

#endif