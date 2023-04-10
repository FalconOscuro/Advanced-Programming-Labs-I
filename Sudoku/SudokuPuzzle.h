#pragma once

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
};

