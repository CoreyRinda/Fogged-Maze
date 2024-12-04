#include "common.h"
#include "maze.h"

/////////////////////////////////////////////////////////////////////////
// class MazeFull

void Maze::DeleteMaze()
{
    if(maze == nullptr)
        return;
    for(int r = 0; r < rows; r++)
        delete[] maze[r];
    delete[] maze;
    maze = nullptr;
}
Maze::Maze()
{
    rows = cols = 0;
    maze = nullptr;
}
Maze::~Maze()
{
    DeleteMaze();
}
int Maze::CountEmpty() const
{
    int cnt = 0;
    for(int r=0; r<rows; r++)
        for(int c=0; c<cols; c++)
            if(maze[r][c] == EMPTY)
                cnt ++;
    return cnt;
}
MazeElem Maze::GetElem(const Loc& loc) const
{
    if(loc.r < 0 || loc.r >= GetRows()) return UNDEFINED;
    if(loc.c < 0 || loc.c >= GetCols()) return UNDEFINED;
    return maze[loc.r][loc.c];
}
void Maze::SetElem(const Loc& loc, MazeElem elem)
{
    if(loc.r < 0 || loc.r >= GetRows())
        throw new MazeUpdateException();
    if(loc.c < 0 || loc.c >= GetCols())
        throw new MazeUpdateException();
    MazeElem oldelem = GetElem(loc);
    if     (oldelem == EMPTY    && elem == A_PLAYER) maze[loc.r][loc.c] = A_PLAYER;
    else if(oldelem == EMPTY    && elem == B_PLAYER) maze[loc.r][loc.c] = B_PLAYER;
    else if(oldelem == A_PLAYER && elem == EMPTY   ) maze[loc.r][loc.c] = EMPTY;
    else if(oldelem == B_PLAYER && elem == EMPTY   ) maze[loc.r][loc.c] = EMPTY;
    else if(oldelem == EXIT     && elem == A_PLAYER) maze[loc.r][loc.c] = A_PLAYER;
    else if(oldelem == EXIT     && elem == B_PLAYER) maze[loc.r][loc.c] = B_PLAYER;
    else
        throw new MazeUpdateException();
}

/////////////////////////////////////////////////////////////////////////
// istream& operator >> (istream& is, MazeFull& maze)

istream& operator >> (istream& is, Maze& maze)
{
    maze.DeleteMaze();
    is >> maze.rows;
    is >> maze.cols;
    maze.maze = new MazeElem * [maze.rows];
    for(int r = 0; r < maze.rows; r++)
        maze.maze[r] = new MazeElem[maze.cols];
    maze.exitloc.SetInvalid();
    maze.aplayerloc.SetInvalid();
    maze.bplayerloc.SetInvalid();
    for(int r = 0; r < maze.rows; r++)
        for(int c = 0; c < maze.cols; c++)
        {
            char ch;
            is >> ch;
            if(ch == '#') { maze.maze[r][c] = WALL    ;                               continue; }
            if(ch == '.') { maze.maze[r][c] = EMPTY   ;                               continue; }
            if(ch == 'E') { maze.maze[r][c] = EXIT    ; maze.exitloc   .SetLoc(r, c); continue; }
            if(ch == 'A') { maze.maze[r][c] = A_PLAYER; maze.aplayerloc.SetLoc(r, c); continue; }
            if(ch == 'B') { maze.maze[r][c] = B_PLAYER; maze.bplayerloc.SetLoc(r, c); continue; }
            throw new MazeFileFormatException();
        }
    // exit location is not in maze file
    if(maze.exitloc.IsInvalid())
        throw new MazeFileFormatException();
    // at least, a-player location should be in maze file
    if(maze.aplayerloc.IsInvalid())
        throw new MazeFileFormatException();
    return is;
}

ostream& operator << (ostream& os, const Maze& maze)
{
    // print index
    cout << " ";
    for(int c=0; c<maze.cols; c++)
        cout << (c % 10);
    cout << endl;

    for(int r=0; r<maze.rows; r++)
    {
        cout << (r % 10);
        for(int c=0; c<maze.cols; c++)
        {
            cout << maze.maze[r][c];
        }
        cout << endl;
    }
    return os;
}

/////////////////////////////////////////////////////////////////////////
// class MazeFoged

FoggedMaze::FoggedMaze(const Maze* maze, const Loc& center, int fogboundary)
    : maze(maze), center(center), fogboundary(fogboundary)
{
}
MazeElem FoggedMaze::GetElem(const Loc& loc) const
{
    if(loc.r < 0 || loc.r >= GetRows()) return UNDEFINED;
    if(loc.c < 0 || loc.c >= GetCols()) return UNDEFINED;
    if(loc.c < center.c - fogboundary) return FOG;
    if(loc.r < center.r - fogboundary) return FOG;
    if(loc.c > center.c + fogboundary) return FOG;
    if(loc.r > center.r + fogboundary) return FOG;
    return maze->GetElem(loc);
}
