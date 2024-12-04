#include <iostream>
#include <fstream>
#include <assert.h>
#include "common.h"
#include "maze.h"
#include "player.h"


void ClearConsole();
void SleepInSec(double sleep_sec);
extern void* a_player_libhandle;
extern void* b_player_libhandle;
string LoadPlayer(MazeElem playerelem, IPlayer*& ab_player, void*& ab_player_libhandle, string libpath);
void CloseLibs();
bool PlayerTurn(int iter, Maze& maze, MazeElem playerelem, IPlayer* player, Loc& player_loc);


void PrintMaze(int iter, int maze_emptyarea, const Maze& maze
    , const IPlayer* a_player, const Loc& a_player_loc, string a_player_libpath
    , const IPlayer* b_player, const Loc& b_player_loc, string b_player_libpath
)
{
    ClearConsole();
    cout << maze;
    cout << "iter(" << iter << "), empty area(" << maze_emptyarea << ")" << endl;
    if(a_player != nullptr) cout << "A-loc(" << a_player_loc.r << "," << a_player_loc.c << ") : " << a_player_libpath << endl;
    if(b_player != nullptr) cout << "B-loc(" << b_player_loc.r << "," << b_player_loc.c << ") : " << b_player_libpath << endl;
    cout << endl;
}

void InitPlayers(IPlayer* a_player, IPlayer* b_player, const Maze& maze)
{
    if(a_player != nullptr) a_player->Init(A_PLAYER, maze.GetRows(), maze.GetCols());
    if(b_player != nullptr) b_player->Init(B_PLAYER, maze.GetRows(), maze.GetCols());
}
void ClosePlayers(IPlayer*& a_player, IPlayer*& b_player)
{
    if(a_player != nullptr) { a_player->Close(); a_player = nullptr; }
    if(b_player != nullptr) { b_player->Close(); b_player = nullptr; }
}


string mazerunner
    (string mazepath, string a_player_libpath, string b_player_libpath, double sleep_sec
    , int& iter, int& maze_totalarea, int& maze_emptyarea
    , int iterlimit_by_mul
)
{
    IPlayer* a_player = nullptr;
    IPlayer* b_player = nullptr;
    if(b_player_libpath == "")
    {
        ////////////////////////////////////////////////////
        // solving mode
        string a_msg = LoadPlayer(A_PLAYER, a_player, a_player_libhandle, a_player_libpath);
        if(a_msg != "")
        {
            cout << "A Player library error: " << a_msg << endl;
            ClosePlayers(a_player, b_player);
            return "not-solve";
        }
        assert(a_player != nullptr);
        assert(a_player_libhandle != nullptr);
    }
    else
    {
        ////////////////////////////////////////////////////
        // match mode
        string a_msg = LoadPlayer(A_PLAYER, a_player, a_player_libhandle, a_player_libpath);
        string b_msg = LoadPlayer(B_PLAYER, b_player, b_player_libhandle, b_player_libpath);

        if(a_msg != "" && b_msg != "")
        {
            cout << "There is no winner!" << endl;
            cout << "A Player Lose by library error: " << a_msg << endl;
            cout << "B Player Lose by library error: " << b_msg << endl;
            ClosePlayers(a_player, b_player);
            return "tie";
        }
        if(a_msg != "")
        {
            cout << "A Player Lose by library error: " << a_msg << endl;
            ClosePlayers(a_player, b_player);
            return "B win";
        }
        if(b_msg != "")
        {
            cout << "B Player Lose by library error: " << b_msg << endl;
            ClosePlayers(a_player, b_player);
            return "A win";
        }
        assert(a_player != nullptr);
        assert(a_player_libhandle != nullptr);
        assert(b_player != nullptr);
        assert(b_player_libhandle != nullptr);
    }

    ///////////////////////////////////////////////////
    // load and initialize maze
    Maze maze;
    try
    {
        ifstream fin;
        fin.open(mazepath);  // open the file object
        fin >> maze;
    }
    catch(MazeFileFormatException* e)
    {
        cerr << "MazeFileFormatException" << endl;
        ClosePlayers(a_player, b_player);
        return "maze format exception";
    }
    Loc a_player_loc = maze.GetAPlayerLoc();
    Loc b_player_loc = maze.GetBPlayerLoc();
    if(b_player == nullptr)
        maze.SetElem(maze.GetBPlayerLoc(), EMPTY);
    maze_totalarea = maze.GetRows() * maze.GetCols();
    maze_emptyarea = maze.CountEmpty();

    ///////////////////////////////////////////////////
    // play game
    iter = 0;
    InitPlayers(a_player, b_player, maze);
    PrintMaze(iter, maze_emptyarea, maze, a_player, a_player_loc, a_player_libpath, b_player, b_player_loc, b_player_libpath);
    while(true)
    {
        iter++;
        if(iter > maze_emptyarea * iterlimit_by_mul)
        {
            PrintMaze(iter, maze_emptyarea, maze, a_player, a_player_loc, a_player_libpath, b_player, b_player_loc, b_player_libpath);
            assert(a_player != nullptr);
            if(b_player != nullptr)
            {
                cout << "There is no winner!" << endl;
                ClosePlayers(a_player, b_player);
                return "tie";
            }
            else
            {
                cout << "A Player do not solve the maze: max iteration ("<< iterlimit_by_mul <<"*empty spaces) is reached." << endl;
                ClosePlayers(a_player, b_player);
                return "not-solve";
            }
        }
        ////////////////////////////////////////////////////////////
        // A player
        try
        {
            cout << "A Player turn begin" << endl;
            bool win = PlayerTurn(iter, maze, A_PLAYER, a_player, a_player_loc);
            if(win)
            {
                PrintMaze(iter, maze_emptyarea, maze, a_player, a_player_loc, a_player_libpath, b_player, b_player_loc, b_player_libpath);
                cout << "A Player (" << a_player_libpath << ") WIN with " << (1.0 * iter / maze_emptyarea) << " iteration-ratio." << endl;
                ClosePlayers(a_player, b_player);
                return "A win";
            }
            cout << "A Player turn end" << endl;
        }
        catch(MazeMoveException*   e) { cout << "A Player Lose by MazeMoveException: " << e->msg << endl; ClosePlayers(a_player, b_player); return "B win"; }
        catch(MazeUpdateException* e) { cout << "A Player Lose by MazeUpdateException."          << endl; ClosePlayers(a_player, b_player); return "B win"; }
        catch(UnknownException*    e) { cout << "A Player Lose by UnknownException."             << endl; ClosePlayers(a_player, b_player); return "B win"; }
        ////////////////////////////////////////////////////////////
        // B player
        if(b_player != nullptr)
        {
            try
            {
                cout << "B Player turn begin" << endl;
                bool win = PlayerTurn(iter, maze, B_PLAYER, b_player, b_player_loc);
                if(win)
                {
                    PrintMaze(iter, maze_emptyarea, maze, a_player, a_player_loc, a_player_libpath, b_player, b_player_loc, b_player_libpath);
                    cout << "B Player (" << b_player_libpath << ") WIN with " << (1.0 * iter / maze_emptyarea) << " iteration-ratio." << endl;
                    ClosePlayers(a_player, b_player);
                    return "B win";
                }
                cout << "B Player turn end" << endl;
            }
            catch(MazeMoveException*   e) { cout << "B Player Lose by MazeMoveException: " << e->msg << endl; ClosePlayers(a_player, b_player); return "A win"; }
            catch(MazeUpdateException* e) { cout << "B Player Lose by MazeUpdateException."          << endl; ClosePlayers(a_player, b_player); return "A win"; }
            catch(UnknownException*    e) { cout << "B Player Lose by UnknownException."             << endl; ClosePlayers(a_player, b_player); return "A win"; }
        }
        ////////////////////////////////////////////////////////////
        // print maze
        PrintMaze(iter, maze_emptyarea, maze, a_player, a_player_loc, a_player_libpath, b_player, b_player_loc, b_player_libpath);
        SleepInSec(sleep_sec);
    }
}

bool PlayerTurn(int iter, Maze& maze, MazeElem playerelem, IPlayer* player, Loc& player_loc)
{
    assert(maze.GetElem(player_loc) == playerelem);
    // create fogged maze
    FoggedMaze fogged(&maze, player_loc, 4);
    // get moving direction from player
    Direct move = player->Move(iter, player_loc, &fogged);

    if(move != UP && move != DOWN && move != LEFT && move == RIGHT && move == STOP)
        throw new MazeMoveException("undefined moving direction.");
    if(move == STOP)
    {
        // do not move
        return false;
    }
    Loc      moved_loc = player_loc.GetMoved(move);
    MazeElem moved_elem = maze.GetElem(moved_loc);
    switch(moved_elem)
    {
    case EXIT:
        // win
        return true;
    case EMPTY:
        // normal move
        maze.SetElem(player_loc, EMPTY);
        maze.SetElem(moved_loc, playerelem);
        player_loc = moved_loc;
        return false;
    case WALL:
        throw new MazeMoveException("try to move to wall");
    case UNDEFINED:
        throw new MazeMoveException("try to move to out of boundary");
    }
    if(playerelem == B_PLAYER && moved_elem == A_PLAYER)
        throw new MazeMoveException("try to move to opponent");
    if(playerelem == A_PLAYER && moved_elem == B_PLAYER)
        throw new MazeMoveException("try to move to opponent");
    throw new UnknownException();
}
