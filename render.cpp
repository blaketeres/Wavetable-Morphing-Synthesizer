/*
Harmonic Generating Wavetable Synthesizer
Blake Teres
2017
*/

#include <Bela.h>
//#include <Scope.h>
#include <vector>
#include "wavetable.h"
#include "morphedWavetable.h"

//Scope scope;

// timer stuff for printing/debugging
float gInterval = 1.5;
float gSecondsElapsed = 0;
int gCount = 0;
int gAudioFramesPerAnalogFrame;

float cwh0[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
float cwa0[10] = {1.0, 0.5, 0.33, 0.25, 0.2, 0.16, 0.14, 0.125, 0.111, 0.1};
std::vector<float> customWavetableHarmonics0 (cwh0, cwh0 + sizeof(cwh0) / sizeof(float));
std::vector<float> customWavetableAmplitude0 (cwa0, cwa0 + sizeof(cwa0) / sizeof(float));

float cwh1[10] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
float cwa1[10] = {1.0, 0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1, 0.05};
std::vector<float> customWavetableHarmonics1 (cwh1, cwh1 + sizeof(cwh1) / sizeof(float));
std::vector<float> customWavetableAmplitude1 (cwa1, cwa1 + sizeof(cwa1) / sizeof(float));

float cwh2[10] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
float cwa2[10] = {1.0, 0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1, 0.05};
std::vector<float> customWavetableHarmonics2 (cwh2, cwh2 + sizeof(cwh2) / sizeof(float));
std::vector<float> customWavetableAmplitude2 (cwa2, cwa2 + sizeof(cwa2) / sizeof(float));

float cwh3[10] = {4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
float cwa3[10] = {1.0, 0.0, 0.5, 0.0, 0.33, 0.0, 0.2, 0.0, 0.1, 0.0};
std::vector<float> customWavetableHarmonics3 (cwh3, cwh3 + sizeof(cwh3) / sizeof(float));
std::vector<float> customWavetableAmplitude3 (cwa3, cwa3 + sizeof(cwa3) / sizeof(float));

// Initialize wavetable objects
wavetable voice0;
wavetable voice1;
wavetable voice2;
wavetable voice3;

wavetable* voice0Ptr = &voice0;
wavetable* voice1Ptr = &voice1;
wavetable* voice2Ptr = &voice2;
wavetable* voice3Ptr = &voice3;

morphedWavetable morphTable0(voice0Ptr, voice1Ptr, voice2Ptr, voice3Ptr);
//morphedWavetable morphTable1(voice2Ptr, voice3Ptr);


// Initialize control variables for potentiometer inputs
float morphSpeed0;
int morphSpeed0Channel = 0;

float morphSpeed1;
int morphSpeed1Channel = 1;

float waveShaper0;
int waveShaper0Channel = 2;

float waveShaper1;
int waveShaper1Channel = 3;

float voice0Pitch;
int voice0PitchChannel = 4;

float voice1Pitch;
int voice1PitchChannel = 5;

float voice2Pitch;
int voice2PitchChannel = 6;

float voice3Pitch;
int voice3PitchChannel = 7;


// Initialize control variables for button inputs
int addWavetablePoint;
int removeWavetablePoint;
int pointSelect;
int resetBezierCurver;
int voiceToggle;
int flipBoundaries;
int voiceOn;

int encoder0PinA = P8_15;
int encoder0PinB = P8_16;
int encoder0Pos = 0;
int encoder0PinALast = LOW;
int encoder0Status = LOW;

int encoder1PinA = P8_18;
int encoder1PinB = P8_27;
int encoder1Pos = 0;
int encoder1PinALast = LOW;
int encoder1Status = LOW;

int encoder2PinA = P8_28;
int encoder2PinB = P8_29;
int encoder2Pos = 0;
int encoder2PinALast = LOW;
int encoder2Status = LOW;

int encoder3PinA = P8_30;
int encoder3PinB = P9_12;
int encoder3Pos = 0;
int encoder3PinALast = LOW;
int encoder3Status = LOW;

int xBezier;
int yBezier;
int boundaryLeft;
int boundaryRight;

void removePotFlutter(int &potValue, int &lastPotValue, int range, int max) {
	if (potValue == 0 || potValue == max) {
		lastPotValue = potValue;
		return;
	}
	if (potValue < (lastPotValue + range) && potValue > (lastPotValue - range)) {
		potValue = lastPotValue;
		return;
	}
	lastPotValue = potValue;
}

	
void handleEncoder(BelaContext *context, int encoderStatus, int encoderPinA, int &encoderPinALast, int encoderPinB, int &encoderPos, int n) {
	encoderStatus = digitalRead(context, n, encoderPinA);
	if ((encoderPinALast == LOW) && (encoderStatus == HIGH)) {
		if (digitalRead(context, n, encoderPinB) == LOW)
			encoderPos--;
	    else
			encoderPos++;
	}
	encoderPinALast = encoderStatus;
}


bool setup(BelaContext *context, void *userData)
{
	// Check if analog channels are enabled
	if(context->analogFrames == 0 || context->analogFrames > context->audioFrames) {
		rt_printf("Error: this example needs analog enabled, with 4 or 8 channels\n");
		return false;
	}
	
	// For this example we need the same amount of audio and analog input and output channels
	if(context->audioInChannels != context->audioOutChannels ||
			context->analogInChannels != context-> analogOutChannels) {
		printf("Error: for this project, you need the same number of input and output channels.\n");
		return false;
	}

	voice0.fillOtherWaveform(customWavetableHarmonics0, customWavetableAmplitude0);
	voice1.fillOtherWaveform(customWavetableHarmonics1, customWavetableAmplitude1);
	voice2.fillOtherWaveform(customWavetableHarmonics2, customWavetableAmplitude2);
	voice3.fillOtherWaveform(customWavetableHarmonics3, customWavetableAmplitude3);

	//scope.setup(1, context->audioSampleRate);
	gAudioFramesPerAnalogFrame = context->audioFrames / context->analogFrames;
	
	pinMode(context, 0, P8_07, INPUT);
	pinMode(context, 0, P8_08, INPUT);
	pinMode(context, 0, P8_09, INPUT);
	pinMode(context, 0, P8_10, INPUT);
	pinMode(context, 0, P8_11, INPUT);
	pinMode(context, 0, P8_12, INPUT);
	pinMode(context, 0, P8_15, INPUT);
	pinMode(context, 0, P8_16, INPUT);
	pinMode(context, 0, P8_18, INPUT);
	pinMode(context, 0, P8_27, INPUT);
	pinMode(context, 0, P8_28, INPUT);
	pinMode(context, 0, P8_29, INPUT);
	pinMode(context, 0, P8_30, INPUT);
	pinMode(context, 0, P9_12, INPUT);
	pinMode(context, 0, P9_14, INPUT);
	
	voiceOn = LOW;
	
	return true;
}

void render(BelaContext *context, void *userData)
{
	float out;
	float out0;
	float out1;
	float out2;
	float out3;
	float gain;
	
	for(unsigned int n = 0; n < context->audioFrames; n++) {
		addWavetablePoint = digitalRead(context, 0, P8_07);
		removeWavetablePoint = digitalRead(context, 0, P8_08);
		pointSelect = digitalRead(context, 0, P8_09);
		resetBezierCurver = digitalRead(context, 0, P8_10);
		voiceToggle = digitalRead(context, 0, P8_11);
		flipBoundaries = digitalRead(context, 0, P8_12);
		voiceOn = digitalRead(context, 0, P9_14);
		
		handleEncoder(context, encoder0Status, encoder0PinA, encoder0PinALast, encoder0PinB, encoder0Pos, n);
		handleEncoder(context, encoder1Status, encoder1PinA, encoder1PinALast, encoder1PinB, encoder1Pos, n);
		handleEncoder(context, encoder2Status, encoder2PinA, encoder2PinALast, encoder2PinB, encoder2Pos, n);
		handleEncoder(context, encoder3Status, encoder3PinA, encoder3PinALast, encoder3PinB, encoder3Pos, n);
		
		if(!(n % gAudioFramesPerAnalogFrame)) {
			morphSpeed0 = analogRead(context, n/gAudioFramesPerAnalogFrame, morphSpeed0Channel);
			morphTable0.setMorphSpeed(morphSpeed0);
			
			//morphSpeed1 = analogRead(context, n/gAudioFramesPerAnalogFrame, morphSpeed1Channel);
			//morphTable1.setMorphSpeed(morphSpeed1);
			
			waveShaper0 = analogRead(context, n/gAudioFramesPerAnalogFrame, waveShaper0Channel);
			waveShaper1 = analogRead(context, n/gAudioFramesPerAnalogFrame, waveShaper1Channel);
			
			voice0Pitch = analogRead(context, n/gAudioFramesPerAnalogFrame, voice0PitchChannel);
			voice0.getPitch(voice0Pitch);
			
			voice1Pitch = analogRead(context, n/gAudioFramesPerAnalogFrame, voice1PitchChannel);
			voice1.getPitch(voice1Pitch);
			
			voice2Pitch = analogRead(context, n/gAudioFramesPerAnalogFrame, voice2PitchChannel);
			voice2.getPitch(voice2Pitch);
			
			voice3Pitch = analogRead(context, n/gAudioFramesPerAnalogFrame, voice3PitchChannel);
			voice3.getPitch(voice3Pitch);
		}
		
		out0 = voice0.getTableOut() * 0.25;
		out1 = voice1.getTableOut() * 0.25;
		out2 = voice2.getTableOut() * 0.25;
		out3 = voice3.getTableOut() * 0.25;
		
		//out0 = morphTable0.outputMorph(morphSpeed0, morphedWavetable::MorphType::backAndForth);
		//out1 = morphTable1.outputMorph(morphSpeed1, morphedWavetable::MorphType::random);
		
		gain = 0.25;
		
		//scope.log(out0);
		out = out0 + out1 + out2 + out3;
		//out = out0 * 0.2;
		
		for(unsigned int channel = 0; channel < context->audioOutChannels; channel++) {
			audioWrite(context, n, channel, out);
		}
		/*
		// Increment a counter on every frame
		gCount++;
		
		// Print a message every second indicating the number of seconds elapsed
		if(gCount % (int)(context->audioSampleRate*gInterval) == 0) {
<<<<<<< HEAD
			scope.trigger();
		    //gSecondsElapsed += gInterval;
		    rt_printf("Frequency: %f\n", voice0.getData());
=======
			//scope.trigger();
		    //gSecondsElapsed += gInterval;
		    rt_printf("%d\n", voiceOn);
>>>>>>> bb601f4a70332f20bec59a60a956fc68c11860fe
		}
		*/
	}
}

void cleanup(BelaContext *context, void *userData)
{

}

