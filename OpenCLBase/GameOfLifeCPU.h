#include "Common.h"

#include "CL\cl.hpp"
#ifndef GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WIN32	
#define GLFW_EXPOSE_NATIVE_WGL	
#endif 
#include "GLFW\glfw3.h"
#include "GLFW\glfw3native.h"	

class GameOfLifeCPU
{
public:
    GameOfLifeCPU(GLFWwindow* window);
    ~GameOfLifeCPU();
    void update();

private:
    Screen screen1;	// buffer 1
    Screen screen2;	// buffer 2
    Screen* front;	// pointer to the visible buffer 
    Screen* back;	// pointer to the back buffer 
    cl::Platform platform;
    cl::Device device;
    cl::Context context;
    cl::Kernel kernel;
    cl::CommandQueue queue;
    cl::Buffer readbuf;
    cl::Buffer writeBuff;
};
