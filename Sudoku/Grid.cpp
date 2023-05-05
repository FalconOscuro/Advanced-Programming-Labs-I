#include "Grid.h"

#include <fstream>
#include <iostream>
#include <algorithm>
using namespace std;

Grid::Grid():
    m_Grid()
{ }

Grid::~Grid()
{ }

/**
 * @brief Load grid from a file
 * 
 * @param filename Read in file
 * @return Load success state
 */
bool Grid::LoadGrid(const char filename[])
{
    ifstream gridFile(filename);
    if (!gridFile)
        return false;

    // Temp array in case read fails due to improperly formatted file
    int grid[9][9];
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
        {
            // Ensure end of file has not been reached yet
            if (gridFile.peek() == EOF)
                return false;
            
            gridFile >> grid[i][j];
        }

    // Copy temp array
    copy(&grid[0][0], &grid[0][0] + (9 * 9), &m_Grid[0][0]);
    return true;
}

/**
 * @brief Saves grid to a file
 * 
 * @param filename Write out file
 */
void Grid::SaveGrid(const char filename[]) const
{
    ofstream outFile(filename);
    if (!outFile)
        return;
    
    // Can just re-use overloaded << operator
    outFile << *this;
}

/**
 * @brief Format grid for output stream
 */
ostream& operator<<(ostream& os, const Grid& grid)
{
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            os << grid.m_Grid[i][j];

            if (j < 8)
                os << ' ';
        }

        if (i < 8)
            os << '\n';
    }
    return os;
}

/**
 * @brief Read grid from input stream
*/
istream& operator>>(istream& is, Grid& grid)
{
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            is >> grid.m_Grid[i][j];
    
    return is;
}

int Grid::Get(size_t x, size_t y) const
{
    return m_Grid[x][y];
}

void Grid::Set(size_t x, size_t y, int n)
{
    m_Grid[x][y] = n;
}