#include "SudokuPuzzle.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

std::string load_program(std::string input) {
  std::ifstream stream(input.c_str());
  if (!stream.is_open()) {
    std::cout << "Cannot open file: " << input << std::endl;
    exit(1);
  }
  return std::string(std::istreambuf_iterator<char>(stream),
                     (std::istreambuf_iterator<char>()));
}

SudokuPuzzle::SudokuPuzzle() :
	_loadTime (std::chrono::duration<double>(0)),
	_solveTime(std::chrono::duration<double>(0))
{
	cl::Platform platform = cl::Platform::getDefault();

	std::cout << "Using Platform: " << platform.getInfo<CL_PLATFORM_NAME>() << std::endl;

	cl::Device device = cl::Device::getDefault();
	std::cout << "Using Device: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;

	m_Context = cl::Context(CL_DEVICE_TYPE_DEFAULT);

	m_Queue = cl::CommandQueue(m_Context);
	
	m_Program = cl::Program(m_Context, load_program("Find.cl"), true);
}

SudokuPuzzle::~SudokuPuzzle() {
	std::cout << "Time to load: " << _loadTime.count() << " microseconds" << std::endl;
	std::cout << "Time to solve: " << _solveTime.count() << " microseconds" << std::endl;
}

void SudokuPuzzle::load(const char filenameIn[]) {
	const auto start = std::chrono::high_resolution_clock::now();

	std::cout << m_Grid.LoadGrid(filenameIn) << std::endl;

	const auto end = std::chrono::high_resolution_clock::now();
	_loadTime = end - start;

	std::cout << m_Grid << std::endl;
}

void SudokuPuzzle::solve() 
{
	unsigned short* optionTable = new unsigned short[81]();

	cl::Buffer d_a = cl::Buffer(m_Context, CL_MEM_READ_WRITE, sizeof(int) * 81);
	cl::Buffer d_b = cl::Buffer(m_Context, CL_MEM_READ_WRITE, sizeof(unsigned short) * 81);
	cl::Buffer d_c = cl::Buffer(m_Context, CL_MEM_READ_WRITE, sizeof(unsigned short) * 81);

	cl::Kernel start_kernel(m_Program, "Start");
	start_kernel.setArg(0, d_a);
	start_kernel.setArg(1, d_b);

	cl::Kernel map_kernel(m_Program, "Map");
	map_kernel.setArg(0, d_a);
	map_kernel.setArg(1, d_b);
	map_kernel.setArg(2, sizeof(unsigned short) * 20, NULL);

	cl::Kernel cell_kernel(m_Program, "CollideCell");
	cell_kernel.setArg(0, d_b);
	cell_kernel.setArg(1, d_c);
	cell_kernel.setArg(2, sizeof(unsigned short) * 8, NULL);

	cl::Kernel col_kernel(m_Program, "CollideCol");
	col_kernel.setArg(0, d_b);
	col_kernel.setArg(1, d_c);
	col_kernel.setArg(2, sizeof(unsigned short) * 8, NULL);

	cl::Kernel row_kernel(m_Program, "CollideRow");
	row_kernel.setArg(0, d_b);
	row_kernel.setArg(1, d_c);
	row_kernel.setArg(2, sizeof(unsigned short) * 8, NULL);

	cl::Kernel write_kernel(m_Program, "WriteBack");
	write_kernel.setArg(0, d_c);
	write_kernel.setArg(1, d_a);
	m_Queue.enqueueWriteBuffer(d_b, CL_FALSE, 0, sizeof(unsigned short) * 81, optionTable);

	const auto start = std::chrono::high_resolution_clock::now();

	m_Queue.enqueueWriteBuffer(d_a, CL_FALSE, 0, sizeof(int) * 81, m_Grid.GetGrid());	

	for (int i = 0; i < 5; i++)
	{
		m_Queue.enqueueNDRangeKernel(start_kernel, cl::NullRange, cl::NDRange(81));
		m_Queue.enqueueNDRangeKernel(map_kernel, cl::NullRange, 
			cl::NDRange(81, 20), cl::NDRange(1, 20));
		m_Queue.enqueueNDRangeKernel(cell_kernel, cl::NullRange, 
			cl::NDRange(81, 8), cl::NDRange(1, 8));
		m_Queue.enqueueNDRangeKernel(col_kernel, cl::NullRange, 
			cl::NDRange(81, 8), cl::NDRange(1, 8));
		m_Queue.enqueueNDRangeKernel(row_kernel, cl::NullRange, 
			cl::NDRange(81, 8), cl::NDRange(1, 8));
		m_Queue.enqueueNDRangeKernel(write_kernel, cl::NullRange, cl::NDRange(81));
	}

	m_Queue.enqueueReadBuffer(d_a, CL_TRUE, 0, sizeof(int) * 81, m_Grid.GetGrid());

	const auto end = std::chrono::high_resolution_clock::now();
	_solveTime = end - start;
	delete optionTable;
}

// This is an example of how you may output the solved puzzle
void SudokuPuzzle::output(const char filenameOut[]) const {
	std::cout << '\n' << m_Grid << std::endl;
	// Add your code here to write your solution to the file filenameOut
}