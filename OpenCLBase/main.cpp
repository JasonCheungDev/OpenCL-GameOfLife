// opencl
#include <CL\cl.hpp>
// opengl 
#include "glad\glad.h"
// expose native functionality for GL/CL interop (used in GPU mode only) 
#ifndef GLFW_EXPOSE_NATIVE_WIN32
	#define GLFW_EXPOSE_NATIVE_WIN32	
	#define GLFW_EXPOSE_NATIVE_WGL		
#endif 
#include "GLFW\glfw3.h"
#include "GLFW\glfw3native.h"		
// system 
#include <sstream>
#include <iostream>
#include <chrono>
#include <fstream>
// stuff 
#include "Common.h"
#include "Shader.h"
#include "GameOfLifeSerial.h"
#include "GameOfLifeGPU.h"
#include "GameOfLifeCPU.h"
#include "GameOfLifeMulti.h"

GLFWwindow* window;
GLuint quadVAO;

void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

void openglCallbackFunction(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam) {

	std::cout << "---------------OPENGL-ERROR---------------" << std::endl;
	std::cout << "MESSAGE:\t" << message << std::endl;
	std::cout << "TYPE:\t\t";
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		std::cout << "ERROR";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		std::cout << "DEPRECATED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		std::cout << "UNDEFINED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		std::cout << "PORTABILITY";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		std::cout << "PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_OTHER:
		std::cout << "OTHER";
		break;
	}
	std::cout << std::endl;

	std::cout << "ID:\t\t" << id << std::endl;
	std::cout << "SEVERITY:\t";
	switch (severity) {
	case GL_DEBUG_SEVERITY_LOW:
		std::cout << "LOW";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		std::cout << "MEDIUM";
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		std::cout << "HIGH";
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		std::cout << "NOTIFICATION";
		break;
	default:
		std::cout << severity;
	}
	std::cout << std::endl;
	std::cout << "------------------------------------------" << std::endl;
}

void initScreenQuad()
{
	float quadVertices[] = {
		// positions        // texture Coords
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};

	glGenVertexArrays(1, &quadVAO);
	glBindVertexArray(quadVAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);	// remember this syntax only works here b/c we defined the array in the same scope 

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void initGL()
{
	// log glfw errors 
	glfwSetErrorCallback(glfw_error_callback);

	// initiailize glfw 
	if (!glfwInit())
	{
		std::cerr << "OpenGL initialization failed." << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// initialize window 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Conaway's Game of Life", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// initiailize context 
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(openglCallbackFunction, 0);

	// init shader 
	Shader shader("vertex.glsl", "fragment.glsl");
	shader.use();

	// init data 
	initScreenQuad();
}

int main()
{
	initGL();

	//GameOfLifeSerial* simulator1 = new GameOfLifeSerial();
    //GameOfLifeCPU* simulator2 = new GameOfLifeCPU(window);
	//GameOfLifeGPU* simulator3 = new GameOfLifeGPU(window);
    GameOfLifeMulti* simulator4 = new GameOfLifeMulti(window);
	// TODO: CPU + GPU (part 4)
    
	// do stuff every frame 
	while (!glfwWindowShouldClose(window))
	{
		//simulator1->update();
        //simulator2->update();
		//simulator3->update();
        simulator4->update();

		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);

		// Keep running
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// finished, cleanup
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

