#ifndef CPUSUDOKU_H
#define CPUSUDOKU_H

#include "Grid.h"

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

    unsigned short int m_BitMask[9][9];

    unsigned short int* m_ColumnIndex[9][9];
    unsigned short int* m_RowIndex[9][9];
    unsigned short int* m_BlockIndex[9][9];
};

#endif // CPUSUDOKU_H