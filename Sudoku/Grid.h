#ifndef GRID_H
#define GRID_H

#include <iostream>

class Grid
{
public:
    Grid();
    ~Grid();

    bool LoadGrid(const char filename[]);
    void SaveGrid(const char filename[]);

    friend std::ostream& operator<<(std::ostream& os, const Grid& grid);
    friend std::istream& operator>>(std::istream& is, Grid& grid);

    int* GetGrid();

private:
    int m_Grid[9][9];
};

#endif // GRID_H