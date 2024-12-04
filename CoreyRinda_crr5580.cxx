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
#include "CoreyRinda_crr5580.h"
#include <cmath>
#include <unordered_set>
#include <queue>
#include <iostream>

// The PlayerFactory function is used to create an instance of CoreyRinda_crr5580.
extern "C" IPlayer* PlayerFactory() {
    return new CoreyRinda_crr5580();
}

// Define a hash function for Loc objects to be used in unordered_set.
struct LocHash {
    std::size_t operator()(const Loc& loc) const {
        return std::hash<int>()(loc.r) ^ (std::hash<int>()(loc.c) << 1);
    }
};

// Constructor for CoreyRinda_crr5580 class.
CoreyRinda_crr5580::CoreyRinda_crr5580() {}

// Destructor for CoreyRinda_crr5580 class.
CoreyRinda_crr5580::~CoreyRinda_crr5580() {
    // Clean up dynamically allocated Node objects.
    for (Node* node : allNodes) {
        delete node;
    }
}

// Initialize the player with its type and maze dimensions.
void CoreyRinda_crr5580::Init(const MazeElem player, const int maze_rows, const int maze_cols) {
    this->player = player;
}

// Close the player (no-op in this implementation).
void CoreyRinda_crr5580::Close() {}

// Move the player in the maze based on the current iteration, location, and maze state.
Direct CoreyRinda_crr5580::Move(const int iter, const Loc& loc, const IMaze* maze) {
    if (iter == 1) {
        // If it's the first iteration, get and print the exit location.
        exitLoc = maze->GetExitLoc();
        //std::cout << "Exit location: " << exitLoc.r << ", " << exitLoc.c << std::endl; ----DEBUG----
    }

    // Print the current location.
    //std::cout << "Current location: " << loc.r << ", " << loc.c << std::endl; ----DEBUG----

    // Find a path to the exit using the FindPath function.
    Node* path = FindPath(loc, maze);
    if (path == nullptr) {
        //std::cout << "No path found" << std::endl; ----DEBUG----
        return STOP;
    }

    // Print that a path has been found and reconstruct the next move.
    //std::cout << "Path found. Reconstructing path..." << std::endl; ----DEBUG----

    Loc nextLoc = ReconstructPath(loc, path);
    //std::cout << "Next move to location: " << nextLoc.r << ", " << nextLoc.c << std::endl; ----DEBUG----

    // Determine the next move direction based on the next location.
    if (nextLoc.r < loc.r) return UP;
    if (nextLoc.r > loc.r) return DOWN;
    if (nextLoc.c < loc.c) return LEFT;
    if (nextLoc.c > loc.c) return RIGHT;

    return STOP;
}

// Find a path from the startLoc to the exitLoc in the maze.
Node* CoreyRinda_crr5580::FindPath(const Loc& startLoc, const IMaze* maze) {
    while (!openList.empty()) openList.pop();
    for (Node* node : allNodes) {
        delete node;
    }
    allNodes.clear();

     // Create a starting node and add it to the openList.
    Node* startNode = new Node(startLoc, 0, CalculateHeuristic(startLoc), nullptr);
    allNodes.push_back(startNode);
    openList.push(startNode);

     // Create a closedSet to keep track of visited locations.
    std::unordered_set<Loc, LocHash> closedSet;

    while (!openList.empty()) {
        // Pop the node with the lowest cost from the openList.
        Node* currentNode = openList.top();
        openList.pop();

        // If the current node's location is the exit, the path is found.
        if (currentNode->loc == exitLoc) {
            //std::cout << "Exit found in pathfinding" << std::endl; ----DEBUG----
            return currentNode;
        }

         // Add the current location to the closedSet.
        closedSet.insert(currentNode->loc);

        // Explore neighboring locations.
        for (const Direct direction : {UP, DOWN, LEFT, RIGHT}) {
            Loc newLoc = currentNode->loc.GetMoved(direction);
            if (maze->GetElem(newLoc) == WALL || maze->GetElem(newLoc) == UNDEFINED) {
                continue;
            }

            if (closedSet.find(newLoc) != closedSet.end()) {
                continue;
            }

            int newCost = currentNode->cost + 1;
            Node* neighborNode = new Node(newLoc, newCost, CalculateHeuristic(newLoc), currentNode);
            openList.push(neighborNode);
            allNodes.push_back(neighborNode);
        }
    }

    //std::cout << "No path found in pathfinding" << std::endl; ----DEBUG----
    return nullptr;
}

// Calculate the heuristic (Manhattan distance) between a location and the exit.
int CoreyRinda_crr5580::CalculateHeuristic(const Loc& loc) const {
    return std::abs(loc.r - exitLoc.r) + std::abs(loc.c - exitLoc.c);
}

// Get neighboring locations of a given location in the maze.
vector<Loc> CoreyRinda_crr5580::GetNeighbors(const Loc& loc, const IMaze* maze) const {
    vector<Loc> neighbors;
    for (const Direct direction : {UP, DOWN, LEFT, RIGHT}) {
        Loc newLoc = loc.GetMoved(direction);
        MazeElem elem = maze->GetElem(newLoc);
        if (elem == EMPTY || elem == EXIT) {
            neighbors.push_back(newLoc);
        }
    }
    return neighbors;
}

// Reconstruct the path by following parent pointers from the end node to the start.
Loc CoreyRinda_crr5580::ReconstructPath(const Loc& startLoc, Node* path) {
    Node* current = path;
    while (current->parent != nullptr && current->parent->loc != startLoc) {
        current = current->parent;
    }
    return current->loc;
}
