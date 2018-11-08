#include "GameOfLifeMulti.h"
#include <iostream>
#include <fstream>
#include <chrono>

GameOfLifeMulti::GameOfLifeMulti(GLFWwindow* window) {
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
    platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
    device = devices[0];

    cl::Platform::get(&platforms);
    index = -1;
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
        std::cerr << "Unable to find an NVidia graphics card. Aborting." << std::endl;
        exit(EXIT_FAILURE);
    }
    else
    {
        platform = platforms[index];
        std::string info;
        platform.getInfo(CL_PLATFORM_NAME, &info);
        std::cout << "PLATFORM:\t" << info << std::endl;
    }
    

    platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
    gpuDevice = devices[0];

    
    // context 
    CPUcontext = cl::Context(device);
    GPUcontext = cl::Context(gpuDevice);
    

    // program 
    std::ifstream kernelFile("game_of_life_cpu.cl");
    std::string src(std::istreambuf_iterator<char>(kernelFile), (std::istreambuf_iterator<char>()));	// brackets are important (not a function prototype)
    cl::Program::Sources sources(1, std::make_pair(src.c_str(), src.length() + 1));
    
    //make cpu program
    cl::Program program(CPUcontext, sources);
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
    cpuKernel = cl::Kernel(program, "GameOfLife", &err);


    //make gpuProgram

    cl::Program program2(GPUcontext, sources);
    err = program2.build();
    if (err != 0)
    {
        // Get the build log
        std::string name = device.getInfo<CL_DEVICE_NAME>();
        std::string buildlog = program2.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
        std::cerr << "Build log for " << name << ":" << std::endl
            << buildlog << std::endl;
    }
    // kernel 
    gpuKernel = cl::Kernel(program2, "GameOfLife", &err);

    // queues
    queue = cl::CommandQueue(CPUcontext, device);
    Q2 = cl::CommandQueue(GPUcontext, gpuDevice);


    // create a texture 

    // generate grid and raw pixel data 
    generateTextureDataPattern3(screen1, 240, SCREEN_HEIGHT / 2 - 50);
    generateTextureDataPattern3(screen2, 240, SCREEN_HEIGHT / 2 - 50);
    front = &screen1;
    back = &screen2;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)screen1.data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glEnable(GL_TEXTURE_2D);


}

GameOfLifeMulti::~GameOfLifeMulti()
{
}

void GameOfLifeMulti::update()
{
    // ===== OPEN CL =====
    auto startTime = std::chrono::high_resolution_clock::now();

    //set args (CPU)
    readbuf = cl::Buffer(CPUcontext, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_uchar4) * (SCREEN_HEIGHT * SCREEN_WIDTH)/2,front->data);
    writeBuff = cl::Buffer(CPUcontext, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_uchar4) * (SCREEN_HEIGHT * SCREEN_WIDTH)/2, back->data);
    cpuKernel.setArg(0, readbuf);//data to read
    cpuKernel.setArg(1, writeBuff);//buffer for output
    cpuKernel.setArg(2, SCREEN_HEIGHT/2); //bounds
    cpuKernel.setArg(3, SCREEN_WIDTH); //bounds
    cpuKernel.setArg(4, ALIVE); //numerical value that represents life colour

    //set args (GPU)
    readbufG = cl::Buffer(GPUcontext, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_uchar4) * (SCREEN_HEIGHT/2 * SCREEN_WIDTH), front->data[SCREEN_HEIGHT/2]);
    writeBuffG = cl::Buffer(GPUcontext, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_uchar4) * (SCREEN_HEIGHT/2 * SCREEN_WIDTH), back->data[SCREEN_HEIGHT/2]);
    gpuKernel.setArg(0, readbufG);//data to read
    gpuKernel.setArg(1, writeBuffG);//buffer for output
    gpuKernel.setArg(2, SCREEN_HEIGHT/2); //bounds
    gpuKernel.setArg(3, SCREEN_WIDTH); //bounds
    gpuKernel.setArg(4, ALIVE); //numerical value that represents life colour

    //prepare queues
    queue.enqueueNDRangeKernel(cpuKernel, cl::NullRange, cl::NDRange(SCREEN_WIDTH, SCREEN_HEIGHT/2));
    Q2.enqueueNDRangeKernel(gpuKernel, cl::NDRange(SCREEN_WIDTH, SCREEN_HEIGHT / 2), cl::NDRange(SCREEN_WIDTH, SCREEN_HEIGHT/2));
    
    //get results
    queue.enqueueReadBuffer(writeBuff, GL_FALSE, 0, sizeof(cl_uchar4) * (SCREEN_HEIGHT * SCREEN_WIDTH)/2, front->data);
    Q2.enqueueReadBuffer(writeBuffG, GL_FALSE, sizeof(cl_uchar4) * (SCREEN_HEIGHT * SCREEN_WIDTH)/2, sizeof(cl_uchar4) * (SCREEN_HEIGHT * SCREEN_WIDTH)/2, front->data + (SCREEN_HEIGHT * SCREEN_WIDTH)/2);
    
    cl::finish();

    //display data
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, front->data);
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
    std::cout << "\rSimulation Time: " << FormatWithCommas(duration.count()) << std::flush;

}
