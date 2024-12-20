#pragma GCC unroll X
#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <algorithm>
#include <cstdlib>  // For random exploration
#include <random>   // For default_random_engine

using namespace std;

const int MAP_SIZE = 400;
const int START_POS = MAP_SIZE / 2;

const char UNEXPLORED = '?';
const char PATH = '.';
const char WALL = '#';
const char HOME = 'H';

struct Position {
    int x, y;
    
    // Konstruktor domyślny, aby umożliwić tworzenie "pustych" instancji.
    Position() : x(0), y(0) {}

    Position(int x, int y) : x(x), y(y) {}
    
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
};


// Hash function for Position to use in unordered_map
struct PositionHash {
    size_t operator()(const Position& pos) const {
        return hash<int>()(pos.x) ^ hash<int>()(pos.y);
    }
};

const vector<pair<int, int>> DIRECTIONS = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
const vector<char> DIR_NAMES = {'N', 'S', 'W', 'E'};

// Global 400x400 map initialized with UNEXPLORED tiles
char mapGrid[MAP_SIZE][MAP_SIZE];

// Store previous positions for backtracking
unordered_map<Position, Position, PositionHash> cameFrom;

void initializeMap() {
    for (int i = 0; i < MAP_SIZE; ++i)
        for (int j = 0; j < MAP_SIZE; ++j)
            mapGrid[i][j] = UNEXPLORED;
}

// Updates the map based on Lassie's vision
void updateMap(int visionRange, const vector<string>& visionGrid, Position lassiePos) {
    int center = visionRange / 2;

    for (int i = 0; i < visionRange; ++i) {
        for (int j = 0; j < visionRange; ++j) {
            int mapX = lassiePos.x - center + i;
            int mapY = lassiePos.y - center + j;

            if (mapX >= 0 && mapX < MAP_SIZE && mapY >= 0 && mapY < MAP_SIZE) {
                mapGrid[mapX][mapY] = visionGrid[i][j]; // Update map with known values
            }
        }
    }
}

// A* Pathfinding for known map
Position findNextPositionAStar(Position start, Position target) {
    auto isInsideMap = [](int x, int y) {
        return x >= 0 && x < MAP_SIZE && y >= 0 && y < MAP_SIZE;
    };
    // Komparator do porównywania na podstawie kosztu
    struct ComparePairs {
        bool operator()(const pair<int, Position>& a, const pair<int, Position>& b) const {
            return a.first > b.first;  // Mniejsze koszty mają wyższy priorytet
        }
    };

    // Kolejka priorytetowa dla A* z użyciem niestandardowego komparatora
    priority_queue<pair<int, Position>, vector<pair<int, Position>>, ComparePairs> pq;

    unordered_map<Position, int, PositionHash> costSoFar;
    unordered_map<Position, Position, PositionHash> localCameFrom;  // Track for current path

    pq.push({0, start});
    costSoFar[start] = 0;
    localCameFrom[start] = start;

    while (!pq.empty()) {
        auto [currentCost, current] = pq.top();
        pq.pop();

        // If Lassie reached the target
        if (current == target) {
            Position nextStep = current;
            while (localCameFrom[nextStep] != start) {
                nextStep = localCameFrom[nextStep];
            }
            cameFrom[nextStep] = start;  // Update global path to allow backtracking
            return nextStep;
        }

        for (int i = 0; i < 4; ++i) {
            int newX = current.x + DIRECTIONS[i].first;
            int newY = current.y + DIRECTIONS[i].second;
            Position neighbor(newX, newY);

            if (isInsideMap(newX, newY) && mapGrid[newX][newY] != WALL) {
                int newCost = costSoFar[current] + 1;
                if (!costSoFar.count(neighbor) || newCost < costSoFar[neighbor]) {
                    costSoFar[neighbor] = newCost;
                    localCameFrom[neighbor] = current;
                    int priority = newCost + abs(target.x - newX) + abs(target.y - newY);
                    pq.push({priority, neighbor});
                }
            }
        }
    }

    // If path is not found, return start itself indicating no move
    return start;
}

int main() {
    initializeMap();

    int visionRange, homeDistX, homeDistY;
    cin >> visionRange >> homeDistX >> homeDistY;

    Position lassiePos(START_POS, START_POS);
    Position targetPos(START_POS + homeDistY, START_POS + homeDistX);
    
    // Set the home position on the map
    mapGrid[targetPos.x][targetPos.y] = HOME;

    // Main loop until Lassie reaches home
    while (!(lassiePos == targetPos)) {
        vector<string> visionGrid(visionRange);

        // Read the vision grid for the current turn
        for (int i = 0; i < visionRange; ++i) {
            cin >> visionGrid[i];
        }

        // Update the global map with the vision
        updateMap(visionRange, visionGrid, lassiePos);

        // Use A* to get the next position towards the target
        Position nextPos = findNextPositionAStar(lassiePos, targetPos);

        // Check if Lassie is stuck
        if (nextPos == lassiePos) {
            cerr << "No path found. Lassie is stuck." << endl;
            return 1;
        }

        // Determine direction of move
        if (nextPos.x < lassiePos.x) cout << 'N' << endl;
        else if (nextPos.x > lassiePos.x) cout << 'S' << endl;
        else if (nextPos.y < lassiePos.y) cout << 'W' << endl;
        else if (nextPos.y > lassiePos.y) cout << 'E' << endl;

        // Update Lassie's position
        lassiePos = nextPos;
    }

    return 0;
}
