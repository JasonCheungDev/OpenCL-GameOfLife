#pragma once

#include "glad\glad.h"
#include "CL\cl.hpp"
#include <iostream>
#include <string>
#include <sstream>

#define SCREEN_WIDTH 1920	
#define SCREEN_HEIGHT 1080
#define DEAD 0
#define ALIVE 255

// Pixel data for a texture 
struct Screen
{
	GLubyte data[SCREEN_HEIGHT][SCREEN_WIDTH][4];	// rgba
};

void generateTextureDataBlank(Screen& s);
void generateTextureDataPattern1(Screen& s, const unsigned int xOff, const unsigned int yOff);
void generateTextureDataPattern2(Screen& s, const unsigned int xOff, const unsigned int yOff);
void generateTextureDataPattern3(Screen& s, const unsigned int xOff, const unsigned int yOff);

template<class T>
std::string FormatWithCommas(T value)
{
	std::stringstream ss;
	ss.imbue(std::locale(""));
	ss << std::fixed << value;
	return ss.str();
}

void printOpenCLInfo();