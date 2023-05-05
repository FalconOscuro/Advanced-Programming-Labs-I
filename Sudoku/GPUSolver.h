#ifndef GPUSUDOKU_H
#define GPUSUDOKU_H

#include "Grid.h"

#include <CL/opencl.hpp>
#include <chrono>

class GPUSudokuPuzzle
{
public:
	GPUSudokuPuzzle();
	~GPUSudokuPuzzle();

	void load(const char filenameIn[]);
	void solve();
	void output(const char filenameOut[]) const;

private:
	std::chrono::duration<double, std::micro> _loadTime;
	std::chrono::duration<double, std::micro> _solveTime;

	Grid m_Grid;

	cl::Context m_Context;
	cl::CommandQueue m_Queue;
	cl::Program m_FindProg;
	
	cl::Program m_Program;

	struct Buffers
	{
		cl::Buffer Puzzle;
		cl::Buffer MaskA;
		cl::Buffer MaskB;
	};

	Buffers m_Buffers;

	struct Kernels
	{
		cl::Kernel Start;
		cl::Kernel Map;
		cl::Kernel Cell;
		cl::Kernel Column;
		cl::Kernel Row;
		cl::Kernel Write;
	};

	Kernels m_Kernels;
};

#endif // GPUSUDOKU_H