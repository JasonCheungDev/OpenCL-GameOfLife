#include "GameOfLifeGPU.h"

#include <iostream>
#include <fstream>
#include <chrono>
#include "Common.h"

GameOfLifeGPU::GameOfLifeGPU(GLFWwindow* window)
{
	// ===== OPENCL =====
	printOpenCLInfo();	// not necessary, just to see information

	// platform 
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	int index = -1;
	for (int i = 0; i < platforms.size(); ++i)
	{
		auto name = platforms[i].getInfo<CL_PLATFORM_NAME>();
		if (name.find("NVIDIA") != std::string::npos)
		{
			index = i;
			break;
		}
	}
	if (index == -1)
	{
		std::cerr << "Unable to find a NVIDIA GPU. Aborting." << std::endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		platform = platforms[index];
		std::string info;
		platform.getInfo(CL_PLATFORM_NAME, &info);
		std::cout << "PLATFORM:\t" << info << std::endl;
	}

	// device 
	std::vector<cl::Device> devices;
	platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
	device = devices[0];

	// context 
	cl_context_properties cps[] = {
		CL_GL_CONTEXT_KHR, (cl_context_properties)glfwGetWGLContext(window),
		CL_WGL_HDC_KHR, (cl_context_properties)GetDC(glfwGetWin32Window(window)),
		CL_CONTEXT_PLATFORM, (cl_context_properties)platform(),
		0
	};
	context = cl::Context(device, cps);

	// program 
	std::ifstream kernelFile("game_of_life_gpu.cl");
	std::string src(std::istreambuf_iterator<char>(kernelFile), (std::istreambuf_iterator<char>()));	// brackets are important (not a function prototype)
	cl::Program::Sources sources(1, std::make_pair(src.c_str(), src.length() + 1));
	cl::Program program(context, sources);
	auto err = program.build();
	if (err != 0)
	{
		// Get the build log
		std::string name = device.getInfo<CL_DEVICE_NAME>();
		std::string buildlog = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
		std::cerr << "Build log for " << name << ":" << std::endl
			<< buildlog << std::endl;
	}

	// kernel 
	kernel = cl::Kernel(program, "GameOfLife", &err);

	// queue
	queue = cl::CommandQueue(context, device);

	Screen* initialState = new Screen();
	generateTextureDataPattern2(*initialState, 500, 500);

	// create shared texture
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, initialState->data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	mem1 = cl::ImageGL(context, CL_MEM_READ_WRITE, GL_TEXTURE_2D, 0, texture1, &err);	// opencl interop with opengl 

	// create shared texture again (for double buffering)
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, initialState->data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	mem2 = cl::ImageGL(context, CL_MEM_READ_WRITE, GL_TEXTURE_2D, 0, texture2, &err);	// opencl interop with opengl 

	delete(initialState);

	// track for ownership control 
	mems.push_back(mem1);
	mems.push_back(mem2);
}


GameOfLifeGPU::~GameOfLifeGPU()
{
}

void GameOfLifeGPU::update()
{
	// ===== OPEN CL =====
	auto startTime = std::chrono::high_resolution_clock::now();

	// acquire ownership 
	glFinish();
	queue.enqueueAcquireGLObjects(&mems);
	
	// set arguments
	kernel.setArg(0, mem1);	// read 
	kernel.setArg(1, mem2);	// write 
	queue.enqueueNDRangeKernel(
		kernel,
		cl::NullRange,
		cl::NDRange(SCREEN_WIDTH, SCREEN_HEIGHT),
		cl::NullRange);
	
	// release ownership 
	queue.enqueueReleaseGLObjects(&mems);
	queue.finish();

	// ===== OPEN GL =====
	// glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture2);
	// glEnable(GL_TEXTURE0);

	// ===== SWAP =====
	std::swap(mem1, mem2);
	std::swap(texture1, texture2);

	auto endTime = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
	std::cout << "\rSimulation Time: " << FormatWithCommas(duration.count()) << std::flush;
}
