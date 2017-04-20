/*
Harmonic Generating Wavetable Synthesizer
Blake Teres
2017
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

wavetable voice0;
wavetable voice1;
wavetable voice2;
wavetable voice3;

wavetable* voice0Ptr = &voice0;
wavetable* voice1Ptr = &voice1;
wavetable* voice2Ptr = &voice2;
wavetable* voice3Ptr = &voice3;

//morphedWavetable morphTable0(voice0Ptr, voice1Ptr, voice2Ptr, voice3Ptr);
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
int addHarmonic;
int removeHarmonic;
int pointSelect;
int resetBezierCurver;
int voiceToggle;
int flipBoundaries;
bool removeFlag;

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
int encoder4Pos = -1;
int encoder4PinALast = LOW;
int encoder4Status = LOW;

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
	switch (selectedVoice) {
		case 0:{
			if(std::find(customWavetableHarmonics0.begin(), customWavetableHarmonics0.end(), harmonicMultiple) != customWavetableHarmonics0.end())
			    break;
			else {
			customWavetableHarmonics0.push_back(harmonicMultiple);
			customWavetableAmplitudes0.push_back(harmonicAmplitude);
			voice0.fillVectorWaveform(customWavetableHarmonics0, customWavetableAmplitudes0);
			break;
			}
		}
		case 1: {
			if(std::find(customWavetableHarmonics1.begin(), customWavetableHarmonics1.end(), harmonicMultiple) != customWavetableHarmonics1.end())
			    break;
			else {
			customWavetableHarmonics1.push_back(harmonicMultiple);
			customWavetableAmplitudes1.push_back(harmonicAmplitude);
			voice1.fillVectorWaveform(customWavetableHarmonics1, customWavetableAmplitudes1);
			break;
			}
		}
		case 2: {
			if(std::find(customWavetableHarmonics2.begin(), customWavetableHarmonics2.end(), harmonicMultiple) != customWavetableHarmonics2.end())
			    break;
			else {
			customWavetableHarmonics2.push_back(harmonicMultiple);
			customWavetableAmplitudes2.push_back(harmonicAmplitude);
			voice2.fillVectorWaveform(customWavetableHarmonics2, customWavetableAmplitudes2);
			break;
			}
		}
		case 3: {
			if(std::find(customWavetableHarmonics3.begin(), customWavetableHarmonics3.end(), harmonicMultiple) != customWavetableHarmonics3.end())
			    break;
			else {
			customWavetableHarmonics3.push_back(harmonicMultiple);
			customWavetableAmplitudes3.push_back(harmonicAmplitude);
			voice3.fillVectorWaveform(customWavetableHarmonics3, customWavetableAmplitudes3);
			break;
			}
		}
	}
}

void removeHarmonicFromVector(unsigned int selectedVoice, unsigned int harmonicMultiple) {
	switch (selectedVoice) {
		case 0: {
			std::vector<int>::iterator position = std::find(customWavetableHarmonics0.begin(), customWavetableHarmonics0.end(), harmonicMultiple);
			if (position != customWavetableHarmonics0.end()) {
    			customWavetableHarmonics0.erase(position);
    			customWavetableAmplitudes0.erase(customWavetableAmplitudes0.begin() + *position);
    			voice0.clearAllTables();
				voice0.fillVectorWaveform(customWavetableHarmonics0, customWavetableAmplitudes0);
			}
			break;
		}
		case 1: {
			std::vector<int>::iterator position = std::find(customWavetableHarmonics1.begin(), customWavetableHarmonics1.end(), harmonicMultiple);
			if (position != customWavetableHarmonics1.end()) {
    			customWavetableHarmonics1.erase(position);
    			customWavetableAmplitudes1.erase(customWavetableAmplitudes1.begin() + *position);
    			voice1.clearAllTables();
				voice1.fillVectorWaveform(customWavetableHarmonics1, customWavetableAmplitudes1);
			}
			break;
		}
		case 2: {
			std::vector<int>::iterator position = std::find(customWavetableHarmonics2.begin(), customWavetableHarmonics2.end(), harmonicMultiple);
			if (position != customWavetableHarmonics2.end()) {
    			customWavetableHarmonics2.erase(position);
    			customWavetableAmplitudes2.erase(customWavetableAmplitudes2.begin() + *position);
    			voice2.clearAllTables();
				voice2.fillVectorWaveform(customWavetableHarmonics2, customWavetableAmplitudes2);
			}
			break;
		}
		case 3: {
			std::vector<int>::iterator position = std::find(customWavetableHarmonics3.begin(), customWavetableHarmonics3.end(), harmonicMultiple);
			if (position != customWavetableHarmonics3.end()) {
    			customWavetableHarmonics3.erase(position);
    			customWavetableAmplitudes3.erase(customWavetableAmplitudes3.begin() + *position);
    			voice3.clearAllTables();
				voice3.fillVectorWaveform(customWavetableHarmonics3, customWavetableAmplitudes3);
			}
			break;
		}
	}
}

int findExistingHarmonic (unsigned int selectedVoice, unsigned int index) {
	switch (selectedVoice) {
		case 0: {
			if (customWavetableHarmonics0.size() > 0) {
				index = index % customWavetableHarmonics0.size();
				return customWavetableHarmonics0[index];
			}
		}
		case 1: {
			if (customWavetableHarmonics1.size() > 0) {
				index = index % customWavetableHarmonics1.size();
				return customWavetableHarmonics1[index];
			}
		}
		case 2: {
			if (customWavetableHarmonics2.size() > 0) {
				index = index % customWavetableHarmonics2.size();
				return customWavetableHarmonics2[index];
			}
		}
		case 3: {
			if (customWavetableHarmonics3.size() > 0) {
				index = index % customWavetableHarmonics3.size();
				return customWavetableHarmonics3[index];
			}
		}
	}
	return 0;
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
	
	customWavetableHarmonics0.reserve(1024);
	customWavetableAmplitudes0.reserve(1024);
	customWavetableHarmonics1.reserve(1024);
	customWavetableAmplitudes1.reserve(1024);
	customWavetableHarmonics2.reserve(1024);
	customWavetableAmplitudes2.reserve(1024);
	customWavetableHarmonics3.reserve(1024);
	customWavetableAmplitudes3.reserve(1024);
	
	removeFlag = false;
	
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
	float out;
	float out0;
	float out1;
	float out2;
	float out3;
	float gain;
	
	unsigned int newHarmonic;
	unsigned int existingHarmonic;
	unsigned int selectedVoice;
	float newHarmonicAmplitude;
	
	for(unsigned int n = 0; n < context->audioFrames; n++) {
		handleEncoder(context, encoder0Status, encoder0PinA, encoder0PinALast, encoder0PinB, encoder0Pos, n);
		handleEncoder(context, encoder1Status, encoder1PinA, encoder1PinALast, encoder1PinB, encoder1Pos, n);
		handleEncoder(context, encoder2Status, encoder2PinA, encoder2PinALast, encoder2PinB, encoder2Pos, n);
		handleEncoder(context, encoder3Status, encoder3PinA, encoder3PinALast, encoder3PinB, encoder3Pos, n);
		//handleEncoder(context, encoder4Status, encoder4PinA, encoder4PinALast, encoder4PinB, encoder4Pos, n);
		
		addHarmonic = digitalRead(context, 0, P8_07);
		removeHarmonic = digitalRead(context, 0, P8_08);
		
		/*
		pointSelect = digitalRead(context, 0, P8_09);
		resetBezierCurver = digitalRead(context, 0, P8_10);
		voiceToggle = digitalRead(context, 0, P8_11);
		flipBoundaries = digitalRead(context, 0, P8_12);
		*/
		
		newHarmonic = constrain(encoder0Pos, 1, MAX_HARMONICS);
		newHarmonicAmplitude = constrain((encoder1Pos / 100.0), 0.0, 1.0);
		selectedVoice = positiveModulo(encoder2Pos, 4);
		existingHarmonic = findExistingHarmonic(selectedVoice, encoder3Pos);
		
		if (addHarmonic == HIGH) addHarmonicToVector(selectedVoice, newHarmonic, newHarmonicAmplitude);
		if (removeHarmonic == LOW && removeFlag == false) removeFlag = true;
		if (removeHarmonic == HIGH && removeFlag == true) {
			removeFlag = false;
			removeHarmonicFromVector(selectedVoice, existingHarmonic);
		}
		
		
		if(!(n % gAudioFramesPerAnalogFrame)) {
			//morphSpeed0 = analogRead(context, n/gAudioFramesPerAnalogFrame, morphSpeed0Channel);
			//morphTable0.setMorphSpeed(morphSpeed0);
			
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
		
		//gain = 0.25;
		
		out = out0 + out1 + out2 + out3;
		scope.log(out0);
		//out = out0 * 0.2;
		
		for(unsigned int channel = 0; channel < context->audioOutChannels; channel++) {
			audioWrite(context, n, channel, out);
		}
		
		// Increment a counter on every frame
		gCount++;
		
		// Print a message every second indicating the number of seconds elapsed
		if(gCount % (int)(context->audioSampleRate*gInterval) == 0) {
			scope.trigger();
		    //gSecondsElapsed += gInterval;
		    rt_printf("Selected Voice: %d\n", selectedVoice);
		    rt_printf("Harmonic To Add: %d\n", newHarmonic);
		    rt_printf("Harmonic Amplitude To Add: %f\n", newHarmonicAmplitude);
		    rt_printf("Selected Harmonic: %d\n", existingHarmonic);
		}
	}
}

void cleanup(BelaContext *context, void *userData)
{

}

