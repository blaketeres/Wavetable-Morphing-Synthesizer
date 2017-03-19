/*
 ____  _____ _        _    
| __ )| ____| |      / \   
|  _ \|  _| | |     / _ \  
| |_) | |___| |___ / ___ \ 
|____/|_____|_____/_/   \_\

The platform for ultra-low latency audio and sensor processing

http://bela.io

A project of the Augmented Instruments Laboratory within the
Centre for Digital Music at Queen Mary University of London.
http://www.eecs.qmul.ac.uk/~andrewm

(c) 2016 Augmented Instruments Laboratory: Andrew McPherson,
	Astrid Bin, Liam Donovan, Christian Heinrichs, Robert Jack,
	Giulio Moro, Laurel Pardue, Victor Zappi. All rights reserved.

The Bela software is distributed under the GNU Lesser General Public License
(LGPL 3.0), available here: https://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include <Bela.h>
#include <cmath>
#include <math_neon.h>
#include "wavetable.h"

#define WAVETABLE_SIZE 4096

float gInterval = 1;
float gSecondsElapsed = 0;
int gCount = 0;
int gAudioFramesPerAnalogFrame;

// Initialize containers for wavetable voices
float wavetableVoice0[WAVETABLE_SIZE];
float voice0Index = 0;
float wavetableVoice1[WAVETABLE_SIZE];
float voice1Index = 0;
float wavetableVoice2[WAVETABLE_SIZE];
float voice2Index = 0;
float wavetableVoice3[WAVETABLE_SIZE];
float voice3Index = 0;

wavetable voice0;
wavetable voice1;
wavetable voice2;
wavetable voice3;

// Initialize control variables for potentiometer inputs
int xCoordinate;
int lastXCoordinate;
int xCoordinateChannel = 0;

int yCoordinate;
int lastYCoordinate;
int yCoordinateChannel = 1;

int indexSelector;
int lastIndexSelector;
int indexSelectorChannel = 2;

float voice0Pitch;
float voice0PitchLast;
int voice0PitchChannel = 3;

float voice1Pitch;
float voice1PitchLast;
int voice1PitchChannel = 4;

float voice2Pitch;
float voice2PitchLast;
int voice2PitchChannel = 5;

float voice3Pitch;
float voice3PitchLast;
int voice3PitchChannel = 6;


// Initialize control vairables for button inputs
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

void removePotFlutterFloat(float &potValue, float &lastPotValue, float range) {
	if (potValue == 0 || potValue == 0.99999) {
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
		if (digitalRead(context, n, encoderPinB) == LOW) {
			encoderPos--;
	    }
	    else {
			encoderPos++;
		}
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
	
	voice0.generateSawtooth();
	voice1.generateSquare();
	voice2.generateTriangle();
	voice3.generateSine();
	
	/*
	float sawtoothInterval = 2.0 / (float)WAVETABLE_SIZE;
	float sawtoothValue = -1.0;
	for (int i = 0; i < WAVETABLE_SIZE; i++) {
		sawtoothValue += sawtoothInterval;
		wavetableVoice0[i] = sawtoothValue;
		wavetableVoice1[i] = sawtoothValue;
		wavetableVoice2[i] = sawtoothValue;
		wavetableVoice3[i] = sawtoothValue;
	}
	*/

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
			xCoordinate = (int)map(analogRead(context, n/gAudioFramesPerAnalogFrame, xCoordinateChannel), 0, 1, 0, WAVETABLE_SIZE);
			removePotFlutter(xCoordinate, lastXCoordinate, 10, WAVETABLE_SIZE - 1);
			
			yCoordinate = (int)map(analogRead(context, n/gAudioFramesPerAnalogFrame, yCoordinateChannel), 0, 1, 0, 1024);
			removePotFlutter(yCoordinate, lastYCoordinate, 10, 1023);
			
			indexSelector = (int)map(analogRead(context, n/gAudioFramesPerAnalogFrame, indexSelectorChannel), 0, 1, 2, WAVETABLE_SIZE);
			removePotFlutter(indexSelector, lastIndexSelector, 10, WAVETABLE_SIZE - 1);
			
			voice0Pitch = analogRead(context, n/gAudioFramesPerAnalogFrame, voice0PitchChannel);
			removePotFlutterFloat(voice0Pitch, voice0PitchLast, 0.005);
			voice0Pitch = powf_neon(voice0Pitch + 1.0, 11.0);
			
			voice1Pitch = analogRead(context, n/gAudioFramesPerAnalogFrame, voice1PitchChannel);
			removePotFlutterFloat(voice1Pitch, voice1PitchLast, 0.005);
			voice1Pitch = powf_neon(voice1Pitch + 1.0, 11.0);
			
			voice2Pitch = analogRead(context, n/gAudioFramesPerAnalogFrame, voice2PitchChannel);
			removePotFlutterFloat(voice2Pitch, voice2PitchLast, 0.005);
			voice2Pitch = powf_neon(voice2Pitch + 1.0, 11.0);
			
			voice3Pitch = analogRead(context, n/gAudioFramesPerAnalogFrame, voice3PitchChannel);
			removePotFlutterFloat(voice3Pitch, voice3PitchLast, 0.005);
			voice3Pitch = powf_neon(voice3Pitch + 1.0, 11.0);
		}
		
		/*
		gCount++;
		
		// Print a message every second indicating the number of seconds elapsed
		if(gCount % (int)(context->audioSampleRate*gInterval) == 0) {
		    gSecondsElapsed += gInterval;
		    rt_printf("Encoder Status: %d\n", encoder3Pos);
		}
		*/
		
		out0 = voice0.wavetable0[(int)(voice0Index)];
		out1 = voice1.wavetable0[(int)(voice1Index)];
		out2 = voice2.wavetable0[(int)(voice2Index)];
		out3 = voice3.wavetable0[(int)(voice3Index)];
		voice0Index += voice0Pitch;
		voice1Index += voice1Pitch;
		voice2Index += voice2Pitch;
		voice3Index += voice3Pitch;
		if (voice0Index >= WAVETABLE_SIZE) {
			voice0Index = voice0Index - WAVETABLE_SIZE;
		}
		if (voice1Index >= WAVETABLE_SIZE) {
			voice1Index = voice1Index - WAVETABLE_SIZE;
		}
		if (voice2Index >= WAVETABLE_SIZE) {
			voice2Index = voice2Index - WAVETABLE_SIZE;
		}
		if (voice3Index >= WAVETABLE_SIZE) {
			voice3Index = voice3Index - WAVETABLE_SIZE;
		}
		if (voiceOn == HIGH) {
			gain = 0.25;
		}
		if (voiceOn == LOW) {
			gain = 0.25;
		}
		out = (out0 + out1 + out2 + out3) * gain;
		for(unsigned int channel = 0; channel < context->audioOutChannels; channel++) {
			audioWrite(context, n, channel, out);
		}
	}
}

void cleanup(BelaContext *context, void *userData)
{

}

