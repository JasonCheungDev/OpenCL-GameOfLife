#pragma once

#include "CL\cl.hpp"

#include "glad\glad.h"
#ifndef GLFW_EXPOSE_NATIVE_WIN32
	#define GLFW_EXPOSE_NATIVE_WIN32	
	#define GLFW_EXPOSE_NATIVE_WGL	
#endif 
#include "GLFW\glfw3.h"
#include "GLFW\glfw3native.h"		

class GameOfLifeGPU
{
public:
	GameOfLifeGPU(GLFWwindow* window);
	~GameOfLifeGPU();
	void update();
private: 
	cl::Platform platform;
	cl::Device device;
	cl::Context context;
	// cl::Program program;
	cl::Kernel kernel;
	cl::CommandQueue queue;
	cl::ImageGL mem1, mem2;
	std::vector<cl::Memory> mems;
	unsigned int texture1, texture2;
	bool flip = false;
};

