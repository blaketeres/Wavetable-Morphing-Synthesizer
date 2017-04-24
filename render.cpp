/*
Harmonic Generating Wavetable Synthesizer
Blake Teres
2017

To Do:

- Add stereo capabilities
- Perfect volume control for voices
- Add waveshaping
- Add save/load feature
*/

#include <Bela.h>
#include <Scope.h>
#include <algorithm>
#include <vector>
#include "wavetable.h"
#include "morphedWavetable.h"

#define MAX_HARMONICS 512

Scope scope;

// timer stuff for printing/debugging
float gInterval = 1.5;
float gSecondsElapsed = 0;
int gCount = 0;
int gAudioFramesPerAnalogFrame;

// Initialize wavetable objects
std::vector<int> customWavetableHarmonics0;
std::vector<float> customWavetableAmplitudes0;

std::vector<int> customWavetableHarmonics1;
std::vector<float> customWavetableAmplitudes1;

std::vector<int> customWavetableHarmonics2;
std::vector<float> customWavetableAmplitudes2;

std::vector<int> customWavetableHarmonics3;
std::vector<float> customWavetableAmplitudes3;

// Initialize harmonics/amplitudes containers for easy access in functions
std::vector<int>* allWavetableHarmonics[4];
std::vector<float>* allWavetableAmplitudes[4];

wavetable voice0;
wavetable voice1;
wavetable voice2;
wavetable voice3;

// Initialize voice container for easy access in functions
wavetable* allVoices[4] = {&voice0, &voice1, &voice2, &voice3};

// Initialize morphtable vector
std::vector<morphedWavetable> morphTables;


// Initialize control variables for potentiometer inputs
float morphSpeed;
int morphSpeedChannel = 0;

float morphMod;
int morphModChannel = 1;

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
int addHarmonic;
int removeHarmonic;
int scrollMorphs;
int morphOnOff;
int morphTypeButton;
int morphType;
int morphIndex;

bool removeFlag;
bool morphFlag;
bool morphOn;
bool morphOnOffFlag;
bool morphTypeFlag;


// Initialize rotary encoders
int encoder0PinA = P8_15;
int encoder0PinB = P8_16;
int encoder0Pos = 0;
int encoder0PinALast = LOW;
int encoder0Status = LOW;

int encoder1PinA = P8_18;
int encoder1PinB = P8_27;
int encoder1Pos = 100;
int encoder1PinALast = LOW;
int encoder1Status = LOW;

int encoder2PinA = P8_28;
int encoder2PinB = P8_29;
int encoder2Pos = -1;
int encoder2PinALast = LOW;
int encoder2Status = LOW;

int encoder3PinA = P8_30;
int encoder3PinB = P9_12;
int encoder3Pos = -1;
int encoder3PinALast = LOW;
int encoder3Status = LOW;

int encoder4PinA = P9_14;
int encoder4PinB = P9_16;
int encoder4Pos = 99;
int encoder4PinALast = LOW;
int encoder4Status = LOW;

unsigned int newHarmonic;
unsigned int existingHarmonic;
unsigned int selectedVoice;
float newHarmonicAmplitude;


// Initialize output variables
float out;
float out0;
float out1;
float out2;
float out3;
float gainKnob;

//----------------------------------------------------------------

int positiveModulo(int i, int n) {
    return (i % n + n) % n;
}

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

void addHarmonicToVector(unsigned int selectedVoice, unsigned int harmonicMultiple, float harmonicAmplitude) {
	if (std::find(allWavetableHarmonics[selectedVoice]->begin(), allWavetableHarmonics[selectedVoice]->end(), harmonicMultiple) != allWavetableHarmonics[selectedVoice]->end())
		return;
	else {
		allWavetableHarmonics[selectedVoice]->push_back(harmonicMultiple);
		allWavetableAmplitudes[selectedVoice]->push_back(harmonicAmplitude);
		allVoices[selectedVoice]->fillVectorWaveform(*allWavetableHarmonics[selectedVoice], *allWavetableAmplitudes[selectedVoice]);
	}
}

void removeHarmonicFromVector(unsigned int selectedVoice, unsigned int harmonicMultiple) {
	std::vector<int>::iterator position;
	position = std::find(allWavetableHarmonics[selectedVoice]->begin(), allWavetableHarmonics[selectedVoice]->end(), harmonicMultiple);
	if (position != allWavetableHarmonics[selectedVoice]->end()) {
		allWavetableHarmonics[selectedVoice]->erase(position);
		allWavetableAmplitudes[selectedVoice]->erase(allWavetableAmplitudes[selectedVoice]->begin() + *position);
		allVoices[selectedVoice]->clearAllTables();
		allVoices[selectedVoice]->fillVectorWaveform(*allWavetableHarmonics[selectedVoice], *allWavetableAmplitudes[selectedVoice]);
	}
}

float findExistingHarmonic (unsigned int selectedVoice, unsigned int index) {
	if (allWavetableHarmonics[selectedVoice]->size() > 0) {
		index = index % allWavetableHarmonics[selectedVoice]->size();
		return (*allWavetableHarmonics[selectedVoice])[index];
	}
	return 0;
}

void adjustGain(int selectedVoice, float newGain, int pos, int posLast) {
	switch(selectedVoice) {
		case 0: voice0.setGain(newGain); break;
		case 1: voice1.setGain(newGain); break;
		case 2: voice2.setGain(newGain); break;
		case 3: voice2.setGain(newGain); break;
	}
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

	scope.setup(1, context->audioSampleRate);
	gAudioFramesPerAnalogFrame = context->audioFrames / context->analogFrames;
	
	allWavetableHarmonics[0] = &customWavetableHarmonics0;
	allWavetableHarmonics[1] = &customWavetableHarmonics1;
	allWavetableHarmonics[2] = &customWavetableHarmonics2;
	allWavetableHarmonics[3] = &customWavetableHarmonics3;
	allWavetableAmplitudes[0] = &customWavetableAmplitudes0;
	allWavetableAmplitudes[1] = &customWavetableAmplitudes1;
	allWavetableAmplitudes[2] = &customWavetableAmplitudes2;
	allWavetableAmplitudes[3] = &customWavetableAmplitudes3;
	
	customWavetableHarmonics0.reserve(1024);
	customWavetableHarmonics1.reserve(1024);
	customWavetableHarmonics2.reserve(1024);
	customWavetableHarmonics3.reserve(1024);
	customWavetableAmplitudes0.reserve(1024);
	customWavetableAmplitudes1.reserve(1024);
	customWavetableAmplitudes2.reserve(1024);
	customWavetableAmplitudes3.reserve(1024);
	
	morphTables.reserve(11);
	
	// Predetermine morphing setup. Need better UI to do this dynamically
	morphTables[0] = morphedWavetable(allVoices[0], allVoices[1]);
	morphTables[1] = morphedWavetable(allVoices[0], allVoices[2]);
	morphTables[2] = morphedWavetable(allVoices[0], allVoices[3]);
	morphTables[3] = morphedWavetable(allVoices[1], allVoices[2]);
	morphTables[4] = morphedWavetable(allVoices[1], allVoices[3]);
	morphTables[5] = morphedWavetable(allVoices[2], allVoices[3]);
	morphTables[6] = morphedWavetable(allVoices[0], allVoices[1], allVoices[2]);
	morphTables[7] = morphedWavetable(allVoices[0], allVoices[1], allVoices[3]);
	morphTables[8] = morphedWavetable(allVoices[0], allVoices[2], allVoices[3]);
	morphTables[9] = morphedWavetable(allVoices[1], allVoices[2], allVoices[3]);
	morphTables[10] = morphedWavetable(allVoices[0], allVoices[1], allVoices[2], allVoices[3]);
	
	morphIndex = 0;
	
	morphFlag = false;
	morphOn = false;
	morphOnOffFlag = false;
	removeFlag = false;
	morphTypeFlag = false;
	
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
	pinMode(context, 0, P9_16, INPUT);

	return true;
}

void render(BelaContext *context, void *userData)
{
	
	for(unsigned int n = 0; n < context->audioFrames; n++) {
		handleEncoder(context, encoder0Status, encoder0PinA, encoder0PinALast, encoder0PinB, encoder0Pos, n);
		handleEncoder(context, encoder1Status, encoder1PinA, encoder1PinALast, encoder1PinB, encoder1Pos, n);
		handleEncoder(context, encoder2Status, encoder2PinA, encoder2PinALast, encoder2PinB, encoder2Pos, n);
		handleEncoder(context, encoder3Status, encoder3PinA, encoder3PinALast, encoder3PinB, encoder3Pos, n);
		handleEncoder(context, encoder4Status, encoder4PinA, encoder4PinALast, encoder4PinB, encoder4Pos, n);
		
		addHarmonic = digitalRead(context, 0, P8_07);
		removeHarmonic = digitalRead(context, 0, P8_08);
		scrollMorphs = digitalRead(context, 0, P8_09);
		morphOnOff = digitalRead(context, 0, P8_10);
		morphTypeButton = digitalRead(context, 0, P8_11);
		
		newHarmonic = constrain(encoder0Pos, 1, MAX_HARMONICS);
		newHarmonicAmplitude = constrain((encoder1Pos / 100.0), 0.0, 1.0);
		selectedVoice = positiveModulo(encoder2Pos, 4);
		existingHarmonic = findExistingHarmonic(selectedVoice, encoder3Pos);
		
		gainKnob = constrain((encoder4Pos / 100.0), 0.0, 1.0);
		adjustGain(selectedVoice, gainKnob, encoder4Pos, encoder4PinALast);
		
		if (addHarmonic == HIGH) addHarmonicToVector(selectedVoice, newHarmonic, newHarmonicAmplitude);
		if (removeHarmonic == LOW && removeFlag == false) removeFlag = true;
		if (removeHarmonic == HIGH && removeFlag == true) {
			removeFlag = false;
			removeHarmonicFromVector(selectedVoice, existingHarmonic);
		}
		if (scrollMorphs == LOW && morphFlag == false) morphFlag = true;
		if (scrollMorphs == HIGH && morphFlag == true) {
			morphIndex = (morphIndex + 1) % 11;
			morphFlag = false;
		}
		if (morphOnOff == LOW && morphOnOffFlag == false) morphOnOffFlag = true;
		if (morphOnOff == HIGH && morphOnOffFlag == true) {
			morphOn = !morphOn;
			morphOnOffFlag = false;
		}
		if (morphTypeButton == LOW && morphTypeFlag == false) morphTypeFlag = true;
		if (morphTypeButton == HIGH && morphTypeFlag == true) {
			morphType = (morphType + 1) % 4;
			morphTypeFlag = false;
		}
		
		
		if(!(n % gAudioFramesPerAnalogFrame)) {
			morphSpeed = analogRead(context, n/gAudioFramesPerAnalogFrame, morphSpeedChannel) + 0.1;
			morphTables[morphIndex].setMorphSpeed(morphSpeed);
			
			morphMod = analogRead(context, n/gAudioFramesPerAnalogFrame, morphModChannel);
			morphTables[morphIndex].setMorphMod(morphMod);
			
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
		
		if (!morphOn) {
			out0 = voice0.getTableOut();
			out1 = voice1.getTableOut();
			out2 = voice2.getTableOut();
			out3 = voice3.getTableOut();
			
			out = (out0 + out1 + out2 + out3) * 0.25;
		}
		
		else {
			out = morphTables[morphIndex].outputMorph(morphType);
		}
		
		scope.log(out);
		
		for(unsigned int channel = 0; channel < context->audioOutChannels; channel++) {
			audioWrite(context, n, channel, out);
		}
		
		// Print stuff
		gCount++;
		if(gCount % (int)(context->audioSampleRate*gInterval) == 0) {
			scope.trigger();
		    //gSecondsElapsed += gInterval;
		    rt_printf("Selected Voice: %d\n", selectedVoice);
		    rt_printf("Harmonic To Add: %d\n", newHarmonic);
		    rt_printf("Harmonic Amplitude To Add: %f\n", newHarmonicAmplitude);
		    rt_printf("Selected Harmonic: %d\n", existingHarmonic);
		    rt_printf("Selected Channel Gain: %f\n", gainKnob);
		    rt_printf("Morph Index: %d\n", morphIndex);
		    rt_printf("Morph Type: %d", morphType);
		}
	}
}

void cleanup(BelaContext *context, void *userData)
{

}

