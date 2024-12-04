//---------------------------------------------------------------------
// Name: Corey Rinda
// Email: crr5580@psu.edu
// Class: CMPSC 330
// Homework 3
// Due Date: November 30, 2023
//
// Description: This program solves a given maze with a Player library
// within (10 * empty maze space) iterations or else it will prompt
// the lose condition. You can play single player or also have
// two different solving libraries fight to win in solving the maze.
// This game can be played in singleplayer or multiplayer.
//
// Acknowledgement:
// 1. I used the a* algorithm and I read up on the algorithm here
// https://www.geeksforgeeks.org/a-search-algorithm/#
// which i used to reference my implementation.
// 2. I also looked at overloading operators in cpp
// https://www.tutorialspoint.com/cplusplus/cpp_overloading.htm
// 3. I looked at this site to help me choose what kind of algorithm
// would work best for a fogged maze setup.
// https://www.bogotobogo.com/Algorithms/path_finding_algorithm.php
//---------------------------------------------------------------------
#ifndef __PLAYER_COREYRINDA_crr5580__
#define __PLAYER_COREYRINDA_crr5580__

#include <iostream>
#include <vector>
#include <queue>
#include "common.h"
#include "maze.h"
#include "player.h"

// Node class represents a node in the search tree for pathfinding.
class Node {
public:
    Loc loc;        // Location associated with the node.
    int cost;       // Cost to reach this node from the start.
    int heuristic;  // Heuristic (estimated cost) from this node to the goal.
    Node* parent;   // Pointer to the parent node in the search tree.

    // Constructor for Node class.
    Node(Loc loc, int cost, int heuristic, Node* parent)
        : loc(loc), cost(cost), heuristic(heuristic), parent(parent) {}
    
    // Calculate the score (cost + heuristic) for the node.
    int Score() const { return cost + heuristic; }
};

// CompareNode class is used to compare nodes based on their scores for priority queue.
class CompareNode {
public:
    bool operator()(const Node* a, const Node* b) {
        return a->Score() > b->Score();
    }
};

// CoreyRinda_crr5580 class inherits from IPlayer and represents the player's logic.
class CoreyRinda_crr5580 : public IPlayer {
private:
    MazeElem player;                            // The player's type.
    Loc exitLoc;                                // Location of the maze exit.
    priority_queue<Node*, vector<Node*>, CompareNode> openList; // Priority queue for pathfinding.
    vector<Node*> allNodes;                     // Stores all nodes for cleanup.

    // Private member functions used for pathfinding and logic.
    Loc ReconstructPath(const Loc& startLoc, Node* path);
    Node* FindPath(const Loc& startLoc, const IMaze* maze);
    int CalculateHeuristic(const Loc& loc) const;
    vector<Loc> GetNeighbors(const Loc& loc, const IMaze* maze) const;
    bool IsLocInList(const Loc& loc, const vector<Node*>& list) const;

public:
    // Constructor for CoreyRinda_crr5580 class.
    CoreyRinda_crr5580();
    
    // Destructor for CoreyRinda_crr5580 class.
    ~CoreyRinda_crr5580();

    // Initialization function to set the player's type and maze dimensions.
    void Init(const MazeElem player, const int maze_rows, const int maze_cols);
    
    // Cleanup function (no-op in this implementation).
    void Close();

    // Main function to determine the player's move based on the current iteration, location, and maze state.
    Direct Move(const int iter, const Loc& loc, const IMaze* maze);
};

// Overloaded equality operator for Loc objects.
bool operator==(const Loc& lhs, const Loc& rhs) {
    return lhs.r == rhs.r && lhs.c == rhs.c;
}

// Overloaded inequality operator for Loc objects.
bool operator!=(const Loc& lhs, const Loc& rhs) {
    return !(lhs == rhs);
}

#endif