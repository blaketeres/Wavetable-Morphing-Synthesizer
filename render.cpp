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

float gInterval = 1;
float gSecondsElapsed = 0;
int gCount = 0;
int gAudioFramesPerAnalogFrame;

// Initialize containers for wavetable voices
float wavetableVoice0[2048];
float wavetableVoice1[2048];
float wavetableVoice2[2048];
float wavetableVoice3[2048];

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

// Initialize control vairables for button inputs
int addWavetablePoint;
int removeWavetablePoint;
int pointSelect;
int resetBezierCurver;
int voiceToggle;
int flipBoundaries;

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
	
	return true;
}

void render(BelaContext *context, void *userData)
{
	for(unsigned int n = 0; n < context->digitalFrames; n++) {
		addWavetablePoint = digitalRead(context, 0, P8_07);
		removeWavetablePoint = digitalRead(context, 0, P8_08);
		pointSelect = digitalRead(context, 0, P8_09);
		resetBezierCurver = digitalRead(context, 0, P8_10);
		voiceToggle = digitalRead(context, 0, P8_11);
		flipBoundaries = digitalRead(context, 0, P8_12);
		
		encoder0Status = digitalRead(context, n, encoder0PinA);
	    if ((encoder0PinALast == LOW) && (encoder0Status == HIGH)) {
	      	if (digitalRead(context, n, encoder0PinB) == LOW) {
	        	encoder0Pos--;
	      	} else {
	        	encoder0Pos++;
	      	}
	    }
	    encoder0PinALast = encoder0Status;
	    
	    encoder1Status = digitalRead(context, n, encoder1PinA);
	    if ((encoder1PinALast == LOW) && (encoder1Status == HIGH)) {
	      	if (digitalRead(context, n, encoder1PinB) == LOW) {
	        	encoder1Pos--;
	      	} else {
	        	encoder1Pos++;
	      	}
	    }
	    encoder1PinALast = encoder1Status;
	    
	    encoder2Status = digitalRead(context, n, encoder2PinA);
	    if ((encoder2PinALast == LOW) && (encoder2Status == HIGH)) {
	      	if (digitalRead(context, n, encoder2PinB) == LOW) {
	        	encoder2Pos--;
	      	} else {
	        	encoder2Pos++;
	      	}
	    }
	    encoder2PinALast = encoder2Status;
	    
	    encoder3Status = digitalRead(context, n, encoder3PinA);
	    if ((encoder3PinALast == LOW) && (encoder3Status == HIGH)) {
	      	if (digitalRead(context, n, encoder3PinB) == LOW) {
	        	encoder3Pos--;
	      	} else {
	        	encoder3Pos++;
	      	}
	    }
	    encoder3PinALast = encoder3Status;
		
		if(!(n % gAudioFramesPerAnalogFrame)) {
			xCoordinate = (int)map(analogRead(context, n/gAudioFramesPerAnalogFrame, xCoordinateChannel), 0, 1, 0, 2048);
			removePotFlutter(xCoordinate, lastXCoordinate, 10, 2047);
			yCoordinate = (int)map(analogRead(context, n/gAudioFramesPerAnalogFrame, yCoordinateChannel), 0, 1, 0, 1024);
			removePotFlutter(yCoordinate, lastYCoordinate, 10, 1023);
			indexSelector = (int)map(analogRead(context, n/gAudioFramesPerAnalogFrame, indexSelectorChannel), 0, 1, 0, 2048);
			removePotFlutter(indexSelector, lastIndexSelector, 10, 2047);
		}
		
		gCount++;
		
		// Print a message every second indicating the number of seconds elapsed
		if(gCount % (int)(context->audioSampleRate*gInterval) == 0) {
		    gSecondsElapsed += gInterval;
		    rt_printf("Encoder Status: %d\n", encoder3Pos);
		}
	}
	
	
	for(unsigned int n = 0; n < context->audioFrames; n++) {
		for(unsigned int channel = 0; channel < context->audioOutChannels; channel++) {
			//audioWrite(context, n, channel, out);
		}
	}
}

void cleanup(BelaContext *context, void *userData)
{

}

/**
\example passthrough/render.cpp

Audio and analog passthrough: input to output
-----------------------------------------

This sketch demonstrates how to read from and write to the audio and analog input and output buffers.

In `render()` you'll see a nested for loop structure. You'll see this in all Bela projects. 
The first for loop cycles through `audioFrames`, the second through 
`audioInChannels` (in this case left 0 and right 1).

You can access any information about current audio and sensor settings like this: 
`context->name_of_item`. For example `context->audioInChannels` returns current number of input channels,
`context->audioFrames` returns the current number of audio frames, 
`context->audioSampleRate` returns the audio sample rate.

You can look at all the information you can access in ::BelaContext.

Reading and writing from the audio buffers
------------------------------------------

The simplest way to read samples from the audio input buffer is with
`audioRead()` which we pass three arguments: context, current audio 
frame and current channel. In this example we have 
`audioRead(context, n, ch)` where both `n` and `ch` are provided by 
the nested for loop structure.

We can write samples to the audio output buffer in a similar way using 
`audioWrite()`. This has a fourth argument which is the value of to output.
For example `audioWrite(context, n, ch, value_to_output)`.

Reading and writing from the analog buffers
-------------------------------------------

The same is true for `analogRead()` and `analogWrite()`.

Note that for the analog channels we write to and read from the buffers in a separate set 
of nested for loops. This is because the they are sampled at half audio rate by default. 
The first of these for loops cycles through `analogFrames`, the second through
`analogInChannels`.

By setting `audioWriteFrame(context, n, ch, audioReadFrame(context, n, ch))` and
`analogWrite(context, n, ch, analogReadFrame(context, n, ch))` we have a simple 
passthrough of audio input to output and analog input to output.


It is also possible to address the buffers directly, for example: 
`context->audioOut[n * context->audioOutChannels + ch]`.
*/
