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
}

float morphedWavetable::outputMorph(float timeInSeconds, int morphType) {
	
	int numSamplesToCrossFade = (int)(timeInSeconds * 44100);
	phaserInterval = 1.0 / numSamplesToCrossFade;
	
	float outA;
	float outB;
	
	switch(morphType) {
		case backAndForth: {
			if (countUp) {
				outA = container[backAndForthIndex]->getTableOut() * (-phaser + 1);
				outB = container[backAndForthIndex + 1]->getTableOut() * phaser;
				break;
			}
			outA = container[backAndForthIndex - 1]->getTableOut() * phaser;
			outB = container[backAndForthIndex]->getTableOut() * (-phaser + 1);
			break;
		}
		case fullCircle: {
			outA = container[currentTable]->getTableOut() * (-phaser + 1);
			outB = container[(currentTable + 1) % numTables]->getTableOut() * phaser;
			break;
		}
		case random: {
			outA = container[lastRandomInt]->getTableOut() * (-phaser + 1);
			outB = container[randomInt]->getTableOut() * phaser;
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

int morphedWavetable::positiveModulo(int i, int n) {
    return (i % n + n) % n;
}

void morphedWavetable::setMorphSpeed(float potInput) {
	 morphSpeed = powf_neon((potInput + 0.1), 2.0) * 2.5;
}

