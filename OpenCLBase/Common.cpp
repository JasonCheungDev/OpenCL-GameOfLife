#include "Common.h"

void generateTextureDataBlank(Screen& s)
{
	for (int y = 0; y < SCREEN_HEIGHT; ++y)
		for (int x = 0; x < SCREEN_WIDTH; ++x)
			s.data[y][x][0] = s.data[y][x][1] = s.data[y][x][2] = s.data[y][x][3] = DEAD;
}

// Infinite growth patterns: http://www.conwaylife.com/w/index.php?title=Infinite_growth

// infinite growth pattern number 1
void generateTextureDataPattern1(Screen& s, const unsigned int yOff, const unsigned int xOff)
{
	for (int y = 0; y < SCREEN_HEIGHT; ++y)
		for (int x = 0; x < SCREEN_WIDTH; ++x)
			s.data[y][x][0] = s.data[y][x][1] = s.data[y][x][2] = DEAD;
	s.data[1 + xOff][1 + yOff][0] = ALIVE;
	s.data[1 + xOff][3 + yOff][0] = ALIVE;
	s.data[1 + xOff][5 + yOff][0] = ALIVE;
	s.data[2 + xOff][2 + yOff][0] = ALIVE;
	s.data[2 + xOff][3 + yOff][0] = ALIVE;
	s.data[2 + xOff][5 + yOff][0] = ALIVE;
	s.data[3 + xOff][4 + yOff][0] = ALIVE;
	s.data[3 + xOff][5 + yOff][0] = ALIVE;
	s.data[4 + xOff][1 + yOff][0] = ALIVE;
	s.data[5 + xOff][1 + yOff][0] = ALIVE;
	s.data[5 + xOff][2 + yOff][0] = ALIVE;
	s.data[5 + xOff][3 + yOff][0] = ALIVE;
	s.data[5 + xOff][5 + yOff][0] = ALIVE;
}

// infinite growth pattern number 2
void generateTextureDataPattern2(Screen& s, const unsigned int yOff, const unsigned int xOff)
{
	for (int y = 0; y < SCREEN_HEIGHT; ++y)
		for (int x = 0; x < SCREEN_WIDTH; ++x)
			s.data[y][x][0] = s.data[y][x][1] = s.data[y][x][2] = DEAD;
	s.data[1 + xOff][1 + yOff][0] = ALIVE;
	s.data[1 + xOff][2 + yOff][0] = ALIVE;
	s.data[1 + xOff][3 + yOff][0] = ALIVE;
	s.data[1 + xOff][4 + yOff][0] = ALIVE;
	s.data[1 + xOff][5 + yOff][0] = ALIVE;
	s.data[1 + xOff][6 + yOff][0] = ALIVE;
	s.data[1 + xOff][8 + yOff][0] = ALIVE;
	s.data[1 + xOff][9 + yOff][0] = ALIVE;
	s.data[1 + xOff][12 + yOff][0] = ALIVE;
	s.data[2 + xOff][1 + yOff][0] = ALIVE;
	s.data[2 + xOff][4 + yOff][0] = ALIVE;
	s.data[2 + xOff][5 + yOff][0] = ALIVE;
	s.data[2 + xOff][7 + yOff][0] = ALIVE;
	s.data[2 + xOff][8 + yOff][0] = ALIVE;
	s.data[2 + xOff][9 + yOff][0] = ALIVE;
	s.data[2 + xOff][10 + yOff][0] = ALIVE;
	s.data[2 + xOff][12 + yOff][0] = ALIVE;
}

// infinite growth pattern number 3
void generateTextureDataPattern3(Screen& s, const unsigned int yOff, const unsigned int xOff)
{
	for (int y = 0; y < SCREEN_HEIGHT; ++y)
		for (int x = 0; x < SCREEN_WIDTH; ++x)
			s.data[y][x][0] = s.data[y][x][1] = s.data[y][x][2] = DEAD;
	s.data[1 + xOff][yOff + 1][0] = ALIVE;
	s.data[1 + xOff][yOff + 2][0] = ALIVE;
	s.data[1 + xOff][yOff + 3][0] = ALIVE;
	s.data[1 + xOff][yOff + 4][0] = ALIVE;
	s.data[1 + xOff][yOff + 5][0] = ALIVE;
	s.data[1 + xOff][yOff + 6][0] = ALIVE;
	s.data[1 + xOff][yOff + 7][0] = ALIVE;
	s.data[1 + xOff][yOff + 8][0] = ALIVE;
	s.data[1 + xOff][yOff + 10][0] = ALIVE;
	s.data[1 + xOff][yOff + 11][0] = ALIVE;
	s.data[1 + xOff][yOff + 12][0] = ALIVE;
	s.data[1 + xOff][yOff + 13][0] = ALIVE;
	s.data[1 + xOff][yOff + 14][0] = ALIVE;
	s.data[1 + xOff][yOff + 18][0] = ALIVE;
	s.data[1 + xOff][yOff + 19][0] = ALIVE;
	s.data[1 + xOff][yOff + 20][0] = ALIVE;
	s.data[1 + xOff][yOff + 27][0] = ALIVE;
	s.data[1 + xOff][yOff + 28][0] = ALIVE;
	s.data[1 + xOff][yOff + 29][0] = ALIVE;
	s.data[1 + xOff][yOff + 30][0] = ALIVE;
	s.data[1 + xOff][yOff + 31][0] = ALIVE;
	s.data[1 + xOff][yOff + 32][0] = ALIVE;
	s.data[1 + xOff][yOff + 33][0] = ALIVE;
	s.data[1 + xOff][yOff + 35][0] = ALIVE;
	s.data[1 + xOff][yOff + 36][0] = ALIVE;
	s.data[1 + xOff][yOff + 37][0] = ALIVE;
	s.data[1 + xOff][yOff + 38][0] = ALIVE;
	s.data[1 + xOff][yOff + 39][0] = ALIVE;
}

void printOpenCLInfo()
{
	std::cout << "===== OPENCL INFORMATION =====" << std::endl;
	// platform info
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	for (auto& platform : platforms)
	{
		std::string info;
		platform.getInfo(CL_PLATFORM_NAME, &info);
		std::cout << "PLATFORM:\t" << info << std::endl;

		// device info
		std::vector<cl::Device> devices;
		platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
		for (auto& device : devices)
		{
			auto vendor = device.getInfo<CL_DEVICE_VENDOR>();
			auto version = device.getInfo<CL_DEVICE_VERSION>();
			std::cout << "DEVICE:\t" << vendor << "\t" << version << std::endl;
		}

		std::cout << std::endl;
	}
	std::cout << "==============================" << std::endl;
}
