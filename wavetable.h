/***** wavetable.h *****/

#ifndef WAVETABLE_H
#define WAVETABLE_H

#define WAVETABLE_SIZE 4096
#define NUM_WAVETABLES_PER_VOICE 11

class wavetable
{
public:
	wavetable(int basicWaveform = -1);
	
	void buildWavetables();
	void generateSine();
	
	void fillSawtooth();
	void fillSquare();
	void fillTriangle();
	
	void generateHarmonic(float* wavetable, int harmonicMultiple, float amplitude);
	
	void getPitch(float potInput);
	void chooseWaveTable(float pitchValue);
	
	float getTableOutAndInc();
	float linearInterpolate();
	
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
	
	float readIndex;
	float pitch;
	float* currentWavetable;

};

#endif