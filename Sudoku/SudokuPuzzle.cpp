#include "SudokuPuzzle.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

SudokuPuzzle::SudokuPuzzle() :
	_loadTime (std::chrono::duration<double>(0)),
	_solveTime(std::chrono::duration<double>(0))
{ }

SudokuPuzzle::~SudokuPuzzle() {
	std::cout << "Time to load: " << _loadTime.count() << " microseconds" << std::endl;
	std::cout << "Time to solve: " << _solveTime.count() << " microseconds" << std::endl;
}

void SudokuPuzzle::load(const char filenameIn[]) {
	const auto start = std::chrono::high_resolution_clock::now();

	// Load the grid
	m_Grid.LoadGrid(filenameIn);

    // Load bitmask
    for (size_t i = 0; i < 9; i++)
        for (size_t j = 0; j < 9; j++)
        {
            unsigned short int mask = (0x1 << m_Grid.Get(i, j)) >> 1;

            m_BitMask[i][j] = mask;

            unsigned short int* addr = &m_BitMask[i][j];
            m_ColumnIndex[j][i] = addr;
            m_RowIndex[i][j] = addr;
            m_BlockIndex[((i / 3) * 3) + (j / 3)][((i % 3) * 3) + (j % 3)] = addr;
        }


	const auto end = std::chrono::high_resolution_clock::now();
	_loadTime = end - start;

	std::cout << m_Grid << std::endl;
}

// Create bitmask of unsolved bits for singular set
unsigned short int FindUnSolved(unsigned short int* set[9])
{
    unsigned short int solvedMask = 0;

    for (size_t i = 0; i < 9; i++)
        solvedMask |= *set[i];

    return (~solvedMask) & 0x1FF;
}

// Create bitmask of unique bits for singular set
unsigned short int FindUnique(unsigned short int* set[9])
{
    unsigned short int uniqueMask = 0;

    for (size_t i = 0; i < 9; i++)
        for (size_t j = i+1; j < 9; j++)
            uniqueMask |= (*set[i]) & (*set[j]);

    return (~uniqueMask) & 0x1FF;
}  

void SudokuPuzzle::solve() 
{
	const auto start = std::chrono::high_resolution_clock::now();
    int loops = 0;

    while (true)
    {
        unsigned short int combinedCol[9];
        unsigned short int combinedRow[9];
        unsigned short int combinedBlk[9];

        // Find all unsolved bits per row/column/block
        for (size_t i = 0; i < 9; i++)
        {
            combinedCol[i] = FindUnSolved(m_ColumnIndex[i]);
            combinedRow[i] = FindUnSolved(m_RowIndex[i]);
            combinedBlk[i] = FindUnSolved(m_BlockIndex[i]);
        }

        bool solved = true;

        // Apply bitmask to unsolved cells
        for (size_t i = 0; i < 9; i++)
            for (size_t j = 0; j < 9; j++)
            {
                if (!m_BitMask[i][j])
                {
                    solved = false;
                    m_BitMask[i][j] = combinedCol[j] & combinedRow[i] & combinedBlk[((i / 3) * 3) + (j / 3)];
                    continue;
                }
            }
        
        // If no unsolved cells exist, puzzle is solved
        if (solved)
            break;

        // Find all unique bits per row/column/block
        for (size_t i = 0; i < 9; i++)
        {
            combinedCol[i] = FindUnique(m_ColumnIndex[i]);
            combinedRow[i] = FindUnique(m_RowIndex[i]);
            combinedBlk[i] = FindUnique(m_BlockIndex[i]);
        }

        // Apply unique bitmask per cell
        for (size_t i = 0; i < 9; i++)
            for(size_t j = 0; j < 9; j++)
                m_BitMask[i][j] &= (combinedCol[j] | combinedRow[i] | combinedBlk[((i / 3) * 3) + (j / 3)]);
        
        loops++;
    }

    // Read back to grid
    for (size_t i = 0; i < 9; i++)
        for (size_t j = 0; j < 9; j++)
            {
                int num = 0;

                for (size_t k = 0; k < 9; k++)
                    num += ((m_BitMask[i][j] >> k) & 0x1) * (k + 1);

                m_Grid.Set(i, j, num);
            }

	const auto end = std::chrono::high_resolution_clock::now();
	_solveTime = end - start;
}

// This is an example of how you may output the solved puzzle
void SudokuPuzzle::output(const char filenameOut[]) const {
	std::cout << '\n' << m_Grid << std::endl;
	
    m_Grid.SaveGrid(filenameOut);
}