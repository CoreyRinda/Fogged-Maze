#ifndef __MAZE__
#define __MAZE__

#include "common.h"
#include <iostream>
using namespace std;

class IMaze
{
public:
    virtual int GetRows() const = 0;
    virtual int GetCols() const = 0;
    virtual Loc GetExitLoc() const = 0;
    virtual MazeElem GetElem(const Loc& loc) const = 0;
};

class Maze : public IMaze
{
private:
    int rows, cols;
    MazeElem** maze;
    Loc exitloc;
    Loc aplayerloc;
    Loc bplayerloc;
    void DeleteMaze();
public:
    Maze();
    virtual ~Maze();
    int GetRows()       const { return rows; }
    int GetCols()       const { return cols; }
    Loc GetExitLoc()    const { return exitloc; }
    Loc GetAPlayerLoc() const { return aplayerloc; }
    Loc GetBPlayerLoc() const { return bplayerloc; }
    int CountEmpty()    const ;
    MazeElem GetElem(const Loc& loc) const;
    void SetElem(const Loc& loc, MazeElem elem);
    friend istream& operator >> (istream& is, Maze& maze);
    friend ostream& operator << (ostream& os, const Maze& maze);
};

class FoggedMaze : public IMaze
{
private:
    const IMaze* maze;
    const Loc    center;
    const int    fogboundary;
public:
    FoggedMaze(const Maze* maze, const Loc& center, int fogboundary);
    int GetRows()    const { return maze->GetRows(); }
    int GetCols()    const { return maze->GetCols(); }
    Loc GetExitLoc() const { return maze->GetExitLoc(); }
    MazeElem GetElem(const Loc& loc) const;
};

#endif
