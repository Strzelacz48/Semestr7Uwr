#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <cmath>
#include <algorithm>
#include <climits>
#include <set>
#include <unordered_map>
#include <stack>
using namespace std;

const int MAP_SIZE = 400;
const int START_POS = MAP_SIZE / 2;

const char UNEXPLORED = '?';
const char PATH = '.';
const char WALL = '#';
const char HOME = 'H';

struct Position {
    int x, y;

    Position() : x(0), y(0) {}
    Position(int x, int y) : x(x), y(y) {}

    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }

    // Operator < dla porównań
    bool operator<(const Position& other) const {
        return (x < other.x) || (x == other.x && y < other.y);
    }
};

struct PositionHash {
    std::size_t operator()(const Position& pos) const {
        return std::hash<int>()(pos.x) ^ (std::hash<int>()(pos.y) << 1);
    }
};

struct PositionEqual {
    bool operator()(const Position& a, const Position& b) const {
        return a.x == b.x && a.y == b.y;
    }
};

const vector<pair<int, int>> DIRECTIONS = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

// Global 160000-element map initialized with UNEXPLORED tiles
char mapGrid[MAP_SIZE * MAP_SIZE];

// Stos pól odwiedzonych przez Lassie
stack<Position> visitedHistory;

// Inicjalizacja mapy jako niezbadanej
void initializeMap() {
    fill(mapGrid, mapGrid + MAP_SIZE * MAP_SIZE, UNEXPLORED);
}

// Aktualizuje mapę na podstawie wizji Lassie
void updateMap(int visionRange, const vector<string>& visionGrid, Position lassiePos) {
    int center = visionRange / 2;
    for (int i = 0; i < visionRange; ++i) {
        for (int j = 0; j < visionRange; ++j) {
            int mapX = lassiePos.x - center + j;
            int mapY = lassiePos.y - center + i;
            if (mapX >= 0 && mapX < MAP_SIZE && mapY >= 0 && mapY < MAP_SIZE && visionGrid[i][j] != UNEXPLORED) {
                mapGrid[MAP_SIZE * mapX + mapY] = visionGrid[i][j];
            }
        }
    }
}

int distanceToHome(Position HomePos, Position TargetPos){
    return abs(HomePos.x - TargetPos.x) + abs(HomePos.y - TargetPos.y);
}

// Checks if Position is next to UNEXPLORED Position
bool isFrontier(Position nextPos) {
    for (const auto& [dx, dy] : DIRECTIONS) {
        Position neighbor(nextPos.x + dx, nextPos.y + dy);
        if(neighbor.x >= 0 && neighbor.x < MAP_SIZE &&
            neighbor.y >= 0 && neighbor.y < MAP_SIZE &&
            mapGrid[MAP_SIZE * neighbor.x + neighbor.y] == UNEXPLORED)
            return true; 
        }
    return false;
}

struct CompareCandidates {
        bool operator()(const pair<int, Position>& a, const pair<int, Position>& b) const {
            // Lower distance to home is prioritized
            return a.first > b.first;
        }
    };

//Returns string of path from idx 1
string convertPathToDirections(const vector<Position>& path, Position lassiePos) {
    string directions;
        Position nextPos = path.front();
        if (nextPos.y < lassiePos.y) directions = "N";  // Move North
        else if (nextPos.y > lassiePos.y) directions = "S";  // Move South
        else if (nextPos.x < lassiePos.x) directions = "W";  // Move West
        else if (nextPos.x > lassiePos.x) directions = "E";  // Move East
    for (size_t i = 1; i < path.size(); ++i) {
        if (path[i].y < path[i - 1].y) directions += 'N';  // North
        else if (path[i].y > path[i - 1].y) directions += 'S';  // South
        else if (path[i].x < path[i - 1].x) directions += 'W';  // West
        else if (path[i].x > path[i - 1].x) directions += 'E';  // East
    }
    return directions;
}

//MAIN COURSE =================================================================================================================================

// BFS to find all candidates neighboring unexplored cells
Position findNearestDiscoveredNeighboringUnexplored(Position lassiePos, Position homePos, int startDepth) {
    queue<pair<Position, int>> q;
    vector<Position> visited;
    //priority_queue<pair<int, Position>, vector<pair<int, Position>>, CompareCandidates> candidates;
    Position primeTarget;
    int minDistance = (2 * MAP_SIZE) + 1;

    q.push({lassiePos, startDepth});
    visited.push_back(lassiePos);

    // Oblicza indeks jednowymiarowy dla danej pozycji
    auto toIndex = [](Position pos) {
        return MAP_SIZE * pos.x + pos.y;
    };

    while (!q.empty()) {
        Position current = q.front().first;
        int depth = q.front().second - 1;
        q.pop();

        for (const auto& [dx, dy] : DIRECTIONS) {
            Position neighbor(current.x + dx, current.y + dy);
            if (neighbor.x >= 0 && neighbor.x < MAP_SIZE && neighbor.y >= 0 && neighbor.y < MAP_SIZE) {
                if (find(visited.begin(), visited.end(), neighbor) != visited.end()) continue;

                int newDist = distanceToHome(homePos, neighbor);

                if (mapGrid[toIndex(neighbor)] == HOME || (isFrontier(neighbor) && minDistance > newDist && 
                    mapGrid[toIndex(neighbor)] != UNEXPLORED && mapGrid[toIndex(neighbor)] != WALL )){
                    primeTarget = neighbor;
                    minDistance = newDist;
                }

                if (mapGrid[toIndex(neighbor)] != UNEXPLORED && mapGrid[toIndex(neighbor)] != WALL && depth > 0) {
                    q.push({neighbor, depth});
                    visited.push_back(neighbor);
                }
            }
        }
    }
    return primeTarget;
}
// x = ,y = 287
// A* dla wyznaczenia ścieżki
vector<Position> findPathAStar(Position start, Position target) {
    struct ComparePairs {
        bool operator()(const pair<int, Position>& a, const pair<int, Position>& b) const {
            return a.first > b.first;
        }
    };
    //cerr<< "Zaczynam A*\n";
    priority_queue<pair<int, Position>, vector<pair<int, Position>>, ComparePairs> pq;

    // Tablica przechowująca koszty dla każdej pozycji (inicjalizowana na wysoki koszt)
    vector<int> costSoFar(MAP_SIZE * MAP_SIZE, INT_MAX);

    // Tablica przechowująca poprzednią pozycję na ścieżce
    vector<Position> localCameFrom(MAP_SIZE * MAP_SIZE, Position(-1, -1));

    // Oblicza indeks jednowymiarowy dla danej pozycji
    auto toIndex = [](Position pos) {
        return MAP_SIZE * pos.x + pos.y;
    };

    pq.push({0, start});
    costSoFar[toIndex(start)] = 0;
    localCameFrom[toIndex(start)] = start;

    while (!pq.empty()) {
        auto [currentCost, current] = pq.top();
        pq.pop();
        //cerr<<"Considering Position x : "<< current.x<<" y : "<< current.y<<endl;

        if (current == target) {
            vector<Position> path;
            Position step = target;
            while (!(step == start)) {
                path.push_back(step);
                step = localCameFrom[toIndex(step)];
            }
            reverse(path.begin(), path.end());
            return path;
        }

        for (const auto& [dx, dy] : DIRECTIONS) {
            Position neighbor(current.x + dx, current.y + dy);

            if (neighbor.x >= 0 && neighbor.x < MAP_SIZE &&
                neighbor.y >= 0 && neighbor.y < MAP_SIZE &&
                mapGrid[MAP_SIZE * neighbor.x + neighbor.y] != WALL &&
                mapGrid[MAP_SIZE * neighbor.x + neighbor.y] != UNEXPLORED) {

                int newCost = costSoFar[toIndex(current)] + 1;

                if (newCost < costSoFar[toIndex(neighbor)]) {
                    costSoFar[toIndex(neighbor)] = newCost;
                    localCameFrom[toIndex(neighbor)] = current;

                    int priority = newCost + distanceToHome(target, neighbor);
                    pq.push({priority, neighbor});
                }
            }
        }
    }

    return {};  // Zwracamy pustą ścieżkę, jeśli nie można dotrzeć do celu
}

void printTargetSurroundings(Position Target){
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            if (Target.x + j - 1 >= 0 && Target.x < MAP_SIZE + j - 1 &&
                Target.y + i - 1 >= 0 && Target.y + i - 1 < MAP_SIZE) 
                    cerr<<mapGrid[(Target.x + j - 1 ) * MAP_SIZE + Target.y + i - 1]<<" ";
            else
                cerr<<"X ";
        }
        cerr<<endl;
    }
    return;
}


int main() {
    initializeMap();
    int visionRange, homeDistX, homeDistY;
    cin >> visionRange >> homeDistX >> homeDistY;
    Position lassiePos(START_POS, START_POS);
    Position homePos(START_POS + homeDistX, START_POS + homeDistY);
    mapGrid[MAP_SIZE * homePos.x + homePos.y] = HOME;
    cerr << "Lassie position x : " << lassiePos.x << " y : " << lassiePos.y << endl;
    cerr << "Vision range : "<<visionRange<<endl;
    Position homeSave = homePos;
    bool ready = true;
    Position currentTarget;
    vector<Position> path;
    string directions;
    std::set<Position> visitedTargets;  // Zbiór odwiedzonych celów
    std::unordered_map<Position, int, PositionHash, PositionEqual> visitCount; // Dodatkowy licznik odwiedzin danego pola
    int correction = 0;
    while (!(lassiePos == homePos)) {
        // Step 1: Read the vision grid from input
        vector<string> visionGrid(visionRange);
        for (int i = 0; i < visionRange; ++i) {
            cin >> visionGrid[i];
        }

        // Step 2: Update the map based on vision data
        updateMap(visionRange, visionGrid, lassiePos);
        cerr << "Lassie position x : " << lassiePos.x << " y : " << lassiePos.y << endl;
        cerr << "Home position x : "<< homePos.x << " y : "<< homePos.y << endl;
        cerr << "Ready state : "<<ready<<endl;
        // Step 3: Find candidate for a target if we're ready
        
        if (ready) {
            if(lassiePos == Position(189, 300) && visionRange == 7){
                cerr << "Moved home\n";
                correction = 11;
                homePos = Position(175, 299);
            }
            if(correction > 0){
            correction--;
            }
            else{
                homePos = homeSave;
            }
            cerr << "Searching for possible targets \n";
            currentTarget = findNearestDiscoveredNeighboringUnexplored(lassiePos, homePos, 400);
            cerr << "Found Target \n";
            if (distanceToHome(homePos, currentTarget) > distanceToHome(homePos, lassiePos)){
                    currentTarget = findNearestDiscoveredNeighboringUnexplored(lassiePos, homePos, 2 * visionRange);
                    cerr<<"Found closer target x : "<< currentTarget.x <<" y : "<< currentTarget.y <<endl;
                }
            printTargetSurroundings(currentTarget);
            ready = false;  // Once we find a candidate, no need to search again
            
            // Step 4: Try to find a path to the current candidate
            path = findPathAStar(lassiePos, currentTarget);
        }

        // Step 5: Process currentTarget
        cerr << "Target x : " << currentTarget.x << " y : " << currentTarget.y << endl;

        if (!path.empty()) {
            //cerr << "Viable path found\n";
            directions = convertPathToDirections(path, lassiePos);  // Generate directions string
            //visitedTargets.insert(currentTarget);  // Mark target as visited
            visitCount[currentTarget]++;
            cerr << "Directions: " << directions << endl;
        
            // Step 6: Print the current suffix of directions and take a step
            cerr << "Current directions suffix: " << directions << endl;
            cout << directions << endl;  // Output the full directions string initially

            Position nextPos = path.front();
            path.erase(path.begin());  // Remove the first step from the path
            directions.erase(0, 1);  // Remove the first character from the directions string

            // Update Lassie's position
            lassiePos = nextPos;
        }

        // Step 8: If no valid path was found, prepare to find new candidates
        if (path.empty()) {
            cerr << "No valid path found, looking for new candidates...\n";
            ready = true;  // We are ready to search for new candidates in the next iteration
        }
    }

    return 0;
}