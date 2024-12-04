#ifndef __PLAYER__
#define __PLAYER__

#include <iostream>
#include "common.h"
#include "maze.h"
using namespace std;

class IPlayer
{
public:
    // Init(const int,const int) will be called before playing with the maze
    virtual void Init
        ( const MazeElem player // player piece type
        , const int maze_rows   // the size of maze
        , const int maze_cols   // the size of maze
        ) = 0;
    // Close() will be called after finishing playing with the maze
    virtual void Close() = 0;
    // Move(const int, const Loc&, const FoggedMaze&) will be called
    // to determine where the player's piece should move
    virtual Direct Move
        ( const int iter        // current iteration
        , const Loc& loc        // player piece location
        , const IMaze* maze     // the fogged maze
        ) = 0;                  // pure virtual function
};

#endif
