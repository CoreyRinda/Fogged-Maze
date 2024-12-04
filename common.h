#ifndef __COMMON__
#define __COMMON__

#include <iostream>
#include <string>
using namespace std;

class Exception
{
};

class MazeFileFormatException : public Exception
{
};

class MazeUpdateException : public Exception
{
};

class UnknownException : public Exception
{
};

class MazeMoveException : public Exception
{
public:
    string msg;
    MazeMoveException(string msg) { this->msg = msg; }
};

enum Direct : int
{
    UP    = 0,
    DOWN  = 1,
    LEFT  = 2,
    RIGHT = 3,
    STOP  = 4
};

enum MazeElem : char
{
    EMPTY     = ' ',
    WALL      = '#',
    FOG       = '?',
    EXIT      = 'E',
    UNDEFINED = '_',
    A_PLAYER  = 'A',
    B_PLAYER  = 'B'
};

class Loc
{
public:
    int r, c;
    Loc()                       { r = -1; c = -1; }
    Loc(int nr, int nc)         { r = nr; c = nc; }
    void SetLoc(int nr, int nc) { r = nr; c = nc; }
    void SetInvalid()           { r = -1; c = -1; }
    bool IsInvalid() const      { return (r == -1 || c == -1); }
    Loc GetMoved(Direct move) const
    {
        switch(move)
        {
        case UP   : return Loc(r-1, c  );
        case DOWN : return Loc(r+1, c  );
        case LEFT : return Loc(r  , c-1);
        case RIGHT: return Loc(r  , c+1);
        case STOP : return Loc(r  , c  );
        }
        throw new MazeMoveException("undefined moving direction.");
    }
};

#endif
