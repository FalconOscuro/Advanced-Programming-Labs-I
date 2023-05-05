#include "GPUSolver.h"

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

GPUSudokuPuzzle::GPUSudokuPuzzle() :
	_loadTime (std::chrono::duration<double>(0)),
	_solveTime(std::chrono::duration<double>(0))
{ }

GPUSudokuPuzzle::~GPUSudokuPuzzle() {
	std::cout << "Time to load: " << _loadTime.count() << " microseconds" << std::endl;
	std::cout << "Time to solve: " << _solveTime.count() << " microseconds" << std::endl;
}

void GPUSudokuPuzzle::load(const char filenameIn[]) {
	const auto start = std::chrono::high_resolution_clock::now();

	// Load the grid
	m_Grid.LoadGrid(filenameIn);

	// Find the Opencl context
	cl::Platform platform = cl::Platform::getDefault();
	cl::Device device = cl::Device::getDefault();
	m_Context = cl::Context(CL_DEVICE_TYPE_DEFAULT);
	m_Queue = cl::CommandQueue(m_Context);

	// Load cl code
	m_Program = cl::Program(m_Context, load_program("Find.cl"), true);

	m_Buffers.Puzzle = cl::Buffer(m_Context, CL_MEM_READ_WRITE, sizeof(int) * 81);
	m_Buffers.MaskA = cl::Buffer(m_Context, CL_MEM_READ_WRITE, sizeof(unsigned short) * 81);
	m_Buffers.MaskB = cl::Buffer(m_Context, CL_MEM_READ_WRITE, sizeof(unsigned short) * 81);

	m_Queue.enqueueWriteBuffer(m_Buffers.Puzzle, CL_FALSE, 0, sizeof(int) * 81, m_Grid.GetGrid());

	m_Kernels.Start = cl::Kernel(m_Program, "Start");
	m_Kernels.Start.setArg(0, m_Buffers.Puzzle);
	m_Kernels.Start.setArg(1, m_Buffers.MaskA);

	m_Kernels.Map = cl::Kernel(m_Program, "Map");
	m_Kernels.Map.setArg(0, m_Buffers.Puzzle);
	m_Kernels.Map.setArg(1, m_Buffers.MaskA);
	m_Kernels.Map.setArg(2, sizeof(unsigned short) * 20, NULL);

	m_Kernels.Cell = cl::Kernel(m_Program, "CollideCell");
	m_Kernels.Cell.setArg(0, m_Buffers.MaskA);
	m_Kernels.Cell.setArg(1, m_Buffers.MaskB);
	m_Kernels.Cell.setArg(2, sizeof(unsigned short) * 8, NULL);

	m_Kernels.Column = cl::Kernel(m_Program, "CollideCol");
	m_Kernels.Column.setArg(0, m_Buffers.MaskA);
	m_Kernels.Column.setArg(1, m_Buffers.MaskB);
	m_Kernels.Column.setArg(2, sizeof(unsigned short) * 8, NULL);

	m_Kernels.Row = cl::Kernel(m_Program, "CollideRow");
	m_Kernels.Row.setArg(0, m_Buffers.MaskA);
	m_Kernels.Row.setArg(1, m_Buffers.MaskB);
	m_Kernels.Row.setArg(2, sizeof(unsigned short) * 8, NULL);

	m_Kernels.Write = cl::Kernel(m_Program, "WriteBack");
	m_Kernels.Write.setArg(0, m_Buffers.MaskB);
	m_Kernels.Write.setArg(1, m_Buffers.Puzzle);

	m_Queue.finish();
	const auto end = std::chrono::high_resolution_clock::now();
	_loadTime = end - start;

	std::cout << m_Grid << std::endl;
	std::cout << "Using Platform: " << platform.getInfo<CL_PLATFORM_NAME>() << std::endl;
	std::cout << "Using Device: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
}

void GPUSudokuPuzzle::solve() 
{
	const auto start = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < 0; i++)
	{
		m_Queue.enqueueNDRangeKernel(m_Kernels.Start, cl::NullRange, cl::NDRange(81));
		m_Queue.enqueueNDRangeKernel(m_Kernels.Map, cl::NullRange, 
			cl::NDRange(81, 20), cl::NDRange(1, 20));
		m_Queue.enqueueNDRangeKernel(m_Kernels.Cell, cl::NullRange, 
			cl::NDRange(81, 8), cl::NDRange(1, 8));
		m_Queue.enqueueNDRangeKernel(m_Kernels.Column, cl::NullRange, 
			cl::NDRange(81, 8), cl::NDRange(1, 8));
		m_Queue.enqueueNDRangeKernel(m_Kernels.Row, cl::NullRange, 
			cl::NDRange(81, 8), cl::NDRange(1, 8));
		m_Queue.enqueueNDRangeKernel(m_Kernels.Write, cl::NullRange, cl::NDRange(81));
	}

	m_Queue.finish();
	const auto end = std::chrono::high_resolution_clock::now();
	_solveTime = end - start;

	m_Queue.enqueueReadBuffer(m_Buffers.Puzzle, CL_TRUE, 0, sizeof(int) * 81, m_Grid.GetGrid());
}

// This is an example of how you may output the solved puzzle
void GPUSudokuPuzzle::output(const char filenameOut[]) const {
	std::cout << '\n' << m_Grid << std::endl;
	// Add your code here to write your solution to the file filenameOut
}