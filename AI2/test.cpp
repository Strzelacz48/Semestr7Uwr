#include <iostream>
#include <vector>
#include <queue>

using namespace std;

enum Field {
    HOLE = -2,
    EMPTY = -1,
    PLAYER_ZERO = 0,
    PLAYER_ONE = 1
};

typedef vector<vector<int>> Map;

struct Position {
    int x, y;
};

// Directions: {dx, dy}
const vector<Position> directions = {
    {0, -1},  // up
    {0, 1},   // down
    {-1, 0},  // left
    {1, 0}    // right
};

bool isValid(const Map &map, int x, int y) {
    return x >= 0 && x < map[0].size() && y >= 0 && y < map.size();
}

void movePawns(Map &map, int player, int dirIndex) {
    const auto &dir = directions[dirIndex];
    vector<vector<bool>> visited(map.size(), vector<bool>(map[0].size(), false));

    for (int y = 0; y < map.size(); ++y) {
        for (int x = 0; x < map[0].size(); ++x) {
            if (map[y][x] == player && !visited[y][x]) {
                int curX = x, curY = y;
                map[curY][curX] = EMPTY;

                while (true) {
                    int nextX = curX + dir.x;
                    int nextY = curY + dir.y;

                    // Check if next position is valid
                    if (!isValid(map, nextX, nextY) || map[nextY][nextX] == HOLE) {
                        break;  // Pawn is removed
                    } else if (map[nextY][nextX] == EMPTY) {
                        map[nextY][nextX] = player;  // Move pawn to empty space
                        visited[nextY][nextX] = true;
                        break;
                    } else if (map[nextY][nextX] != player) {
                        // Push opponent's pawn
                        int pushX = nextX, pushY = nextY;

                        while (true) {
                            int newX = pushX + dir.x;
                            int newY = pushY + dir.y;

                            if (!isValid(map, newX, newY) || map[newY][newX] == HOLE) {
                                map[pushY][pushX] = EMPTY;  // Opponent's pawn is removed
                                break;
                            } else if (map[newY][newX] == EMPTY) {
                                map[newY][newX] = map[pushY][pushX];
                                map[pushY][pushX] = EMPTY;
                                break;
                            }
                            pushX = newX;
                            pushY = newY;
                        }

                        map[nextY][nextX] = player;
                        visited[nextY][nextX] = true;
                        break;
                    }

                    curX = nextX;
                    curY = nextY;
                }
            }
        }
    }
}

void printMap(const Map &map) {
    for (const auto &row : map) {
        for (int cell : row) {
            if (cell == HOLE) {
                cout << "H ";
            } else if (cell == EMPTY) {
                cout << ". ";
            } else if (cell == PLAYER_ZERO) {
                cout << "0 ";
            } else if (cell == PLAYER_ONE) {
                cout << "1 ";
            }
        }
        cout << endl;
    }
    cout << endl;
}

int main() {
    // Example map
    Map map = {
        {EMPTY, EMPTY, HOLE, EMPTY, EMPTY},
        {EMPTY, PLAYER_ZERO, EMPTY, PLAYER_ONE, EMPTY},
        {HOLE, EMPTY, EMPTY, EMPTY, HOLE},
        {EMPTY, PLAYER_ONE, EMPTY, PLAYER_ZERO, EMPTY},
        {EMPTY, EMPTY, HOLE, EMPTY, EMPTY}
    };

    cout << "Initial map:" << endl;
    printMap(map);

    // Player 0 moves up
    movePawns(map, PLAYER_ZERO, 0);
    cout << "After Player 0 moves up:" << endl;
    printMap(map);

    // Player 1 moves right
    movePawns(map, PLAYER_ONE, 1);
    cout << "After Player 1 moves right:" << endl;
    printMap(map);
    
    // Player 0 moves up
    movePawns(map, PLAYER_ZERO, 0);
    cout << "After Player 0 moves up:" << endl;
    printMap(map);

    // Player 1 moves right
    movePawns(map, PLAYER_ONE, 3);
    cout << "After Player 1 moves right:" << endl;
    printMap(map);

    return 0;
}