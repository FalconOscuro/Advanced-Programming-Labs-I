#ifndef SUDOKU_H
#define SUDOKU_H

#include "Grid.h"

#include <CL/opencl.hpp>
#include <chrono>

class SudokuPuzzle
{
public:
	SudokuPuzzle();
	~SudokuPuzzle();

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
};

#endif // SUDOKU_H