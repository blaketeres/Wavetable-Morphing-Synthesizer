/***** morphedWavetable.cpp *****/

#include "morphedWavetable.h"

morphedWavetable::morphedWavetable(wavetable* a, wavetable* b) {
	
	srand (time(NULL));

	container[0] = a;
	container[1] = b;
	
	morphType = backAndForth;
	phaser = 0.0;
	numTables = 2;
	currentTable = 0;
	countUp = true;
	randomInt = 1;
	lastRandomInt = 0;
	backAndForthIndex = 0;
	modPhaser = 0;
}

morphedWavetable::morphedWavetable(wavetable* a, wavetable* b, wavetable* c) {
	
	srand (time(NULL));
	
	container[0] = a;
	container[1] = b;
	container[2] = c;
	
	morphType = backAndForth;
	phaser = 0.0;
	numTables = 3;
	currentTable = 0;
	countUp = true;
	randomInt = 1;
	lastRandomInt = 0;
	backAndForthIndex = 0;
	modPhaser = 0;
}

morphedWavetable::morphedWavetable(wavetable* a, wavetable* b, wavetable* c, wavetable* d) {
	
	srand (time(NULL));
	
	container[0] = a;
	container[1] = b;
	container[2] = c;
	container[3] = d;
	
	morphType = backAndForth;
	phaser = 0.0;
	numTables = 4;
	currentTable = 0;
	countUp = true;
	randomInt = 1;
	lastRandomInt = 0;
	backAndForthIndex = 0;
	modPhaser = 0;
}

float morphedWavetable::outputMorph(int morphType) {
	
	float numSamplesToCrossFade = morphSpeed * 44100;
	phaserInterval = 1.0 / numSamplesToCrossFade;
	
	float outA;
	float outB;
	
	switch(morphType) {
		case backAndForth: {
			if (countUp) {
				outA = container[backAndForthIndex]->getTableOut() * powf(-phaser + 1, 2);
				outB = container[backAndForthIndex + 1]->getTableOut() * powf(phaser, 2);
				break;
			}
			outA = container[backAndForthIndex - 1]->getTableOut() * powf(phaser, 2);
			outB = container[backAndForthIndex]->getTableOut() * powf(-phaser + 1, 2);
			break;
		}
		case fullCircle: {
			outA = container[currentTable]->getTableOut() * powf(-phaser + 1, 2);
			outB = container[(currentTable + 1) % numTables]->getTableOut() * powf(phaser, 2);
			break;
		}
		case random: {
			outA = container[lastRandomInt]->getTableOut() * powf(-phaser + 1, 2);
			outB = container[randomInt]->getTableOut() * powf(phaser, 2);
			break;
		}
	}
	
	phaser += phaserInterval;
	if (phaser >= 1.0 ) {
		phaser = 0.0;
		currentTable += 1;
		currentTable = currentTable % numTables;
		lastRandomInt = randomInt;
		getRandomInt();
		if (countUp) backAndForthIndex += 1;
		else backAndForthIndex -= 1;
		if (backAndForthIndex == numTables - 1 || backAndForthIndex == 0)
			countUp = !countUp;
	}
	return outA + outB;
}

void morphedWavetable::getRandomInt() {
	randomInt = rand() % numTables;
	if (randomInt == lastRandomInt)
		getRandomInt();
}

void morphedWavetable::setMorphSpeed(float potInput) {
	 morphSpeed = powf(potInput, 2) * 5;
}

void morphedWavetable::setMorphMod(float potInput) {
	modInterval = potInput;
	modPhaser += modInterval;
	if (modPhaser >= TWO_PI) modPhaser = modPhaser - TWO_PI;
	mod = sinf(modPhaser);
}

