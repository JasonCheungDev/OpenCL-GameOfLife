#include "GameOfLifeCPU.h"
#include <iostream>
#include <fstream>
#include <chrono>

GameOfLifeCPU::GameOfLifeCPU(GLFWwindow* window) {
    // ===== OPENCL =====
    printOpenCLInfo();	// not necessary, just to see information

                        // platform 
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    int index = -1;
    for (int i = 0; i < platforms.size(); ++i)
    {
        auto name = platforms[i].getInfo<CL_PLATFORM_NAME>();
        if (name.find("Intel") != std::string::npos)
        {
            index = i;
            break;
        }
    }
    if (index == -1)
    {
        std::cerr << "Unable to find an Intel CPU. Aborting." << std::endl;
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
    platform.getDevices(CL_DEVICE_TYPE_CPU, &devices);
    device = devices[0];

    // context 
    context = cl::Context(device);

    

    // program 
    std::ifstream kernelFile("game_of_life_cpu.cl");
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

    //somthing to do with the rendering here?
    // create a texture 

    // generate grid and raw pixel data 
    generateTextureDataPattern2(screen1, 240, 240);
    generateTextureDataPattern2(screen2, 240, 240);
    front = &screen1;
    back = &screen2;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)screen1.data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glEnable(GL_TEXTURE_2D);


}

GameOfLifeCPU::~GameOfLifeCPU()
{
}

void GameOfLifeCPU::update()
{
    // ===== OPEN CL =====
    auto startTime = std::chrono::high_resolution_clock::now();

    //set args
    readbuf = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_uchar4) * (SCREEN_HEIGHT * SCREEN_WIDTH),front->data);
    writeBuff = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_uchar4) * (SCREEN_HEIGHT * SCREEN_WIDTH), back->data);
    kernel.setArg(0, readbuf);//data to read
    kernel.setArg(1, writeBuff);//buffer for output
    kernel.setArg(2, SCREEN_HEIGHT); //bounds
    kernel.setArg(3, SCREEN_WIDTH); //bounds
    kernel.setArg(4, ALIVE); //numerical value that represents life colour

    //prepare queue
    queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(SCREEN_WIDTH, SCREEN_HEIGHT), cl::NullRange);
    queue.enqueueReadBuffer(writeBuff, GL_TRUE, 0, sizeof(cl_uchar4) * (SCREEN_HEIGHT * SCREEN_WIDTH), front->data);
    queue.finish();
    // swap 
    
    //std::swap(front, back);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, front->data);
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
    std::cout << "\rSimulation Time: " << FormatWithCommas(duration.count()) << std::flush;

}
