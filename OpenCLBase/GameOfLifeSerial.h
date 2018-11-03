#pragma once

#include "Common.h"

class GameOfLifeSerial
{
public:
	GameOfLifeSerial();
	~GameOfLifeSerial();
	void update();

private: 
	Screen screen1;	// buffer 1
	Screen screen2;	// buffer 2
	Screen* front;	// pointer to the visible buffer 
	Screen* back;	// pointer to the back buffer 

	void simulate();
	int sample_alive_count(const Screen* screen, int y, int x);
};

