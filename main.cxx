#include <iostream>
#include <fstream>
#include <assert.h>
#include "common.h"
#include "maze.h"
#include "player.h"
using namespace std;

#include <unistd.h>  // usleep()
#include <dlfcn.h>

string mazerunner
    ( string mazepath, string a_player_libpath, string b_player_libpath, double sleep_sec
    , int& iter, int& maze_totalarea, int& maze_emptyarea
    , int iterlimit_by_mul
    );
void CloseLibs();

int main(int argc, char* argv[])
{
    // argv[0] : program name
    // argv[1] : maze file path
    // argv[2] : a player library path
    // argv[3] : b player library path
    if(argc < 3)
        return 0;

    string mazepath = argv[1];
    string a_player_libpath = argv[2];
    string b_player_libpath = "";
    if(argc >= 4)
        b_player_libpath = argv[3];
    double sleep_sec = 0.05;
    int iter, maze_totalarea, maze_emptyarea;
    int iterlimit_by_mul = 10;
    string result = mazerunner(mazepath, a_player_libpath, b_player_libpath, sleep_sec, iter, maze_totalarea, maze_emptyarea, iterlimit_by_mul);
    CloseLibs();
}

void ClearConsole()
{
    system("clear");
}

void SleepInSec(double sleep_sec)
{
    int sleep_microsecond = sleep_sec * 1000000;
    usleep(sleep_microsecond);
}

void* a_player_libhandle = nullptr;
void* b_player_libhandle = nullptr;

typedef IPlayer* (*CreatePlayerType)();
string LoadPlayer(MazeElem playerelem, IPlayer*& ab_player, void*& ab_player_libhandle, string libpath)
{
    ab_player_libhandle = dlopen(libpath.c_str(), RTLD_LAZY);
    if(ab_player_libhandle == nullptr)
    {
        return "cannot load " + libpath;
    }
    void* pfunc = dlsym(ab_player_libhandle, "PlayerFactory");
    if(pfunc == nullptr)
    {
        return "cannot find CreatePlayer() function defined as extern C";
    }
    CreatePlayerType CreatePlayer = (CreatePlayerType)pfunc;
    ab_player = CreatePlayer();
    if(ab_player == nullptr)
    {
        return "cannot create player using CreatePlayer() function";
    }
    return "";
}
void CloseLibs()
{
    if(a_player_libhandle != nullptr) { dlclose(a_player_libhandle); a_player_libhandle == nullptr; }
    if(b_player_libhandle != nullptr) { dlclose(b_player_libhandle); b_player_libhandle == nullptr; }
}

