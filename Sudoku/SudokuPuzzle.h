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

	typedef char16_t Bitmask;
    Bitmask m_CellArray[9][9];

	typedef Bitmask* CellGroup[9];
    CellGroup m_ColumnIndex[9];
    CellGroup m_RowIndex[9];
    CellGroup m_BlockIndex[9];

	static Bitmask FindUnSolved(CellGroup group);
	static Bitmask FindUnique(CellGroup group);
};

#endif // CPUSUDOKU_H