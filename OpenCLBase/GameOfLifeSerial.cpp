#include "GameOfLifeSerial.h"

#include <sstream>
#include <iostream>
#include <chrono>
#include <fstream>

GameOfLifeSerial::GameOfLifeSerial()
{
	// generate grid and raw pixel data 
	generateTextureDataPattern3(screen1, 240, 240);
	generateTextureDataPattern3(screen2, 240, 240);
	front = &screen1;
	back = &screen2;
	// create a texture 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)screen1.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glEnable(GL_TEXTURE_2D);
}

GameOfLifeSerial::~GameOfLifeSerial()
{
}

void GameOfLifeSerial::update()
{
	auto startTime = std::chrono::high_resolution_clock::now();

	simulate();
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, back->data);

	auto endTime = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
	std::cout << "\rSimulation Time: " << FormatWithCommas(duration.count()) << std::flush;
}

void GameOfLifeSerial::simulate()
{
	// 1. if a dead cell has exactly 3 alive neighbours, then it becomes alive

	// 2. if an alive cell has 2 or 3 alive neighbours, then it stays alive

	// 3. else it DIES

	for (int y = 0; y < SCREEN_HEIGHT; ++y)
	{
		for (int x = 0; x < SCREEN_WIDTH; ++x)
		{
			int neighbours_alive = sample_alive_count(front, y, x);

			if (front->data[y][x][0] == DEAD)
			{
				// rule 1
				if (neighbours_alive == 3)
				{
					back->data[y][x][0] = back->data[y][x][1] = back->data[y][x][2] = ALIVE;
				}
				else
				{
					back->data[y][x][0] = back->data[y][x][1] = back->data[y][x][2] = DEAD;
				}
			}
			else
			{
				// rule 2 
				if (neighbours_alive == 2 || neighbours_alive == 3)
				{
					back->data[y][x][0] = back->data[y][x][1] = back->data[y][x][2] = ALIVE;
				}
				// rule 3
				else
				{
					back->data[y][x][0] = back->data[y][x][1] = back->data[y][x][2] = DEAD;
				}
			}
		}
	}

	// swap 
	std::swap(front, back);
}

int GameOfLifeSerial::sample_alive_count(const Screen * screen, int y, int x)
{
	{
		int alive = 0;

		for (int yOffset = -1; yOffset <= 1; ++yOffset)
		{
			for (int xOffset = -1; xOffset <= 1; ++xOffset)
			{
				int chkY = y + yOffset;
				int chkX = x + xOffset;

				if (chkY < 0 || chkY >= SCREEN_HEIGHT	         // out of bounds 
					|| chkX < 0 || chkX >= SCREEN_WIDTH	         // out of bounds 
					|| (yOffset == 0 && xOffset == 0)) continue; // current cell 
				if (screen->data[chkY][chkX][0] == ALIVE)
					++alive;
			}
		}

		return alive;
	}
}
