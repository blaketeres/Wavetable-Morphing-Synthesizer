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
float wavetableVoice1[2048];
float wavetableVoice2[2048];
float wavetableVoice3[2048];
float wavetableVoice4[2048];

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

int xBezier;
int yBezier;
int boundaryLeft;
int boundaryRight;

int removePotFlutter(int &potValue, int lastPotValue, int range, int max) {
		if (potValue == 0 || potValue == max) {
			return potValue;
		}
		if (potValue < (lastPotValue + range) && potValue > (lastPotValue - range)) {
			return lastPotValue;
		}
		return potValue;
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
	for(unsigned int n = 0; n < context->audioFrames; n++) {
		addWavetablePoint = digitalRead(context, 0, P8_07);
		removeWavetablePoint = digitalRead(context, 0, P8_08);
		pointSelect = digitalRead(context, 0, P8_09);
		resetBezierCurver = digitalRead(context, 0, P8_10);
		voiceToggle = digitalRead(context, 0, P8_11);
		flipBoundaries = digitalRead(context, 0, P8_12);
		
		if(!(n % gAudioFramesPerAnalogFrame)) {
			int x = (int)map(analogRead(context, n/gAudioFramesPerAnalogFrame, xCoordinateChannel), 0, 1, 0, 2048);
			xCoordinate = removePotFlutter(x, lastXCoordinate, 5, 2047);
			int y = (int)map(analogRead(context, n/gAudioFramesPerAnalogFrame, yCoordinateChannel), 0, 1, 0, 1024);
			yCoordinate = removePotFlutter(y, lastYCoordinate, 5, 1023);
			int index = (int)map(analogRead(context, n/gAudioFramesPerAnalogFrame, indexSelectorChannel), 0, 1, 0, 2048);
			indexSelector = removePotFlutter(index, lastIndexSelector, 5, 2047);
		}
		
		gCount++;
		
		// Print a message every second indicating the number of seconds elapsed
		if(gCount % (int)(context->audioSampleRate*gInterval) == 0) {
		    gSecondsElapsed += gInterval;
		    rt_printf("Pot Status: %d\n", pointSelect);
		
		for(unsigned int ch = 0; ch < context->audioInChannels; ch++){
			audioWrite(context, n, ch, audioRead(context, n, ch));
			}
		}
	}

	// Same with analog channels
	for(unsigned int n = 0; n < context->analogFrames; n++) {
		for(unsigned int ch = 0; ch < context->analogInChannels; ch++) {
		}
	}
	lastXCoordinate = xCoordinate;
	lastYCoordinate = yCoordinate;
	lastIndexSelector = indexSelector;
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
