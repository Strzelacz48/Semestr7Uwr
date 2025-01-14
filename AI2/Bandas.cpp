#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <queue>
#include <random>
#include <time.h>

#pragma GCC optimize("Ofast,inline,tracer")
#pragma GCC optimize("unroll-loops,vpt,split-loops,unswitch-loops")

using namespace std;

#define TURN_LIMIT 200
#define TIME_LIMIT 100 //ms
#define MAX_DEPTH 25

enum Field {
    HOLE = -2,
    EMPTY = -1,
    PLAYER_ZERO = 0,
    PLAYER_ONE = 1,
    OCCUPIED = 2
};

enum Direction {
    UP = 0,
    DOWN = 1,
    LEFT = 2,
    RIGHT = 3
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

void printMap(const Map& map) {
    for (const auto &row : map) {
        for (int cell : row) {
            if (cell == HOLE) {
                std::cerr << "\033[40m\033[30mH \033[0m"; // Black background, black text
            } else if (cell == EMPTY) {
                std::cerr << "\033[40m\033[37m. \033[0m"; // Black background, white text
            } else if (cell == PLAYER_ZERO) {
                std::cerr << "\033[44m\033[37m0 \033[0m"; // Blue background, white text
            } else if (cell == PLAYER_ONE) {
                std::cerr << "\033[41m\033[37m1 \033[0m"; // Red background, white text
            }
        }
        std::cerr << std::endl;
    }
    std::cerr << std::endl;
}
//Basic Game Logic
//=======================================================================================================
//funkcja która powinna decydować czy dane pole się rusza czy nie
int willIMove(int myID, int neighbor, bool activeID){
    if(activeID != myID && myID == EMPTY && neighbor != EMPTY){
        return 0;
    }
    else if(myID == EMPTY || myID == HOLE){
        return 1;//nie rusz sie
    }
    else if(activeID == myID){
        return 3;//aktywny gracz się rusza(zamień się na puste pole)
    }
    else if(myID == neighbor){
        return 0;//kontynuuj rekurencje
    }
    else if(neighbor == EMPTY || neighbor == HOLE){
        return 1;//nie rusz sie
    }
    else{
        return 2;//nadpisz swoją pozycję prawą stroną
    }
}

//dir == 0 przesuwa od lewej do prawej, dir == 1 od prawej do lewej (przesuwanie z dir == 1 nie działa)
int* moveRow(int size, int* row, bool activeID){
    int* newRow = new int[size];
    int move;
    for(int i = 0; i < size; i++){
        if(i == size - 1){
            move = activeID == row[i] ? 3 : 1;
        }
        else{
            move = willIMove(row[i], row[i + 1], activeID);
            int ii = i;
            while(move == 0){
                ii++;
                if(ii >= size - 1){
                    move = 1;
                    break;
                }
                move = willIMove(row[ii], row[ii + 1], activeID);
            }
        }
        if(move == 1){
            newRow[i] = row[i];
        }
        else if(move == 2){
            if(i > 0 && row[i - 1] != HOLE){
                newRow[i - 1] = row[i];
            }
            newRow[i] = row[i + 1];
        }
        else if(move == 3){
            if(i > 0 && row[i - 1] != HOLE && row[i] != EMPTY){
                newRow[i - 1] = row[i];
            }
            newRow[i] = EMPTY;
        }
    }
    return newRow;
}

//Funkcja przycina mapę puste krawędzie z każdej strony (raz na koniec tury) (odcięte pola są zastępowane przez HOLE)
Map trimMap(const Map &map) {
    Map newMap = map;
    for(int i = 0; i < map.size() - 1; i++){// cięcie od góry
        int row_state = HOLE;
        for(int j = 0; j < map[i].size(); j++){
            if(map[i][j] != EMPTY && map[i][j] != HOLE){
                row_state = OCCUPIED;
                break;
            }
            else if(map[i][j] == EMPTY){
                row_state = EMPTY;
            }
        }
        if(row_state == OCCUPIED){
            break;
        }
        else if(row_state == EMPTY){
            for(int j = 0; j < map[i].size(); j++){
                newMap[i][j] = HOLE;
            }
            //break;
        }
    }
    for(int i = map.size() - 1; i >= 0; i--){// cięcie od dołu
        int row_state = HOLE;
        for(int j = 0; j < map[i].size(); j++){
            if(map[i][j] != EMPTY && map[i][j] != HOLE){
                row_state = OCCUPIED;
                break;
            }
            else if(map[i][j] == EMPTY){
                row_state = EMPTY;
            }
        }
        if(row_state == OCCUPIED){
            break;
        }
        else if(row_state == EMPTY){
            for(int j = 0; j < map[i].size(); j++){
                newMap[i][j] = HOLE;
            }
            //break;
        }
    }
    for(int i = 0; i < map[0].size() - 1; i++){// cięcie od lewej
        int col_state = HOLE;
        for(int j = 0; j < map.size(); j++){
            if(map[j][i] != EMPTY && map[j][i] != HOLE){
                col_state = OCCUPIED;
                break;
            }
            else if(map[j][i] == EMPTY){
                col_state = EMPTY;
            }
        }
        if(col_state == OCCUPIED){
            break;
        }
        else if(col_state == EMPTY){
            for(int j = 0; j < map.size(); j++){
                newMap[j][i] = HOLE;
            }
            //break;
        }
    }
    for(int i = map[0].size() - 1; i >= 0; i--){// cięcie od prawej
        int col_state = HOLE;
        for(int j = 0; j < map.size(); j++){
            if(map[j][i] != EMPTY && map[j][i] != HOLE){
                col_state = OCCUPIED;
                break;
            }
            else if(map[j][i] == EMPTY){
                col_state = EMPTY;
            }
        }
        if(col_state == OCCUPIED){
            break;
        }
        else if(col_state == EMPTY){
            for(int j = 0; j < map.size(); j++){
                newMap[j][i] = HOLE;
            }
            //break;
        }
    }
    return newMap;
}

//pomocnicza do debugu
void BprintRow(int* newRow, int i, int size){
    cerr<<"\nRow : "<<i<<endl;
    cerr<<"Before moveRow"<<endl;
    for(int j = 0; j < size; j++){
        cerr<<newRow[j]<<" ";
    }
    cerr<<endl;
}

//pomocnicza do debugu
void AprintRow(int* newRow, int i, int size){
    cerr<<"\nAfter moveRow"<<endl;
    for(int j = 0; j < size; j++){
        cerr<<newRow[j]<<" ";
    }
    cerr<<endl;
}

//Funkcja przesuwa mapę w kierunku dirIndex 
//gdzie active true dla jedynek false dla zer (aktywny gracz)
Map move(Map &map, int dirIndex, bool active){
    const auto &dir = directions[dirIndex];
    Map newMap = map;
    if(dirIndex == UP){//w górę
        for(int i = 0; i < map[0].size(); i++){
            int* newRow = new int[map[0].size()];
            for(int j = 0; j < map.size(); j++){
                newRow[j] = map[j][i];
            }
            //BprintRow(newRow, i,map.size());
            newRow = moveRow(map.size(), newRow, active);
            //AprintRow(newRow, i,map.size());
            for(int j = 0; j < map.size(); j++){
                newMap[j][i] = newRow[j];
            }
        }
    }
    else if(dirIndex == DOWN){//w dół
        for(int i = 0; i < map[0].size(); i++){
            int* newRow = new int[map[0].size()];
            for(int j = map.size() - 1; j >= 0; j--){
                newRow[map.size() - 1 - j] = map[j][i];
            }
            //BprintRow(newRow, i,map.size());
            newRow = moveRow(map.size(), newRow, active);
            //AprintRow(newRow, i,map.size());
            for(int j = 0; j < map.size(); j++){
                newMap[map.size() - 1 - j][i] = newRow[j];  
            }
        }
    }
    else if(dirIndex == LEFT){//w lewo
        for(int i = 0; i < map.size(); i++){
            int* newRow = new int[map[0].size()];
            for(int j = 0; j < map[0].size(); j++){
                newRow[j] = map[i][j];
            }
            //BprintRow(newRow, i,map.size());
            newRow = moveRow(map[0].size(), newRow, active);
            //AprintRow(newRow, i,map.size());
            for(int j = 0; j < map[0].size(); j++){
                newMap[i][j] = newRow[j];
            }
        }
    }
    else if(dirIndex == RIGHT){// w prawo
        for(int i = 0; i < map.size(); i++){
            int* newRow = new int[map[0].size()];
            for(int j = map[0].size() - 1; j >= 0; j--){
                newRow[map[0].size() - 1 - j] = map[i][j];
            }
            //BprintRow(newRow, i,map[0].size());
            newRow = moveRow(map[0].size(), newRow, active);
            //AprintRow(newRow, i,map[0].size());
            for(int j = 0; j < map[0].size(); j++){
                newMap[i][map[0].size() - 1 - j] = newRow[j];
            }
        }
    }
    //cerr << "After Player moves:" << endl;
    //printMap(newMap);
    //Trim empty sides
    newMap = trimMap(newMap);

    return newMap;
}

pair<int, int> getScore(const Map &map, int myID, int opponentID){
    int myScore = 0;
    int opponentScore = 0;
    for(int i = 0; i < map.size(); i++){
        for(int j = 0; j < map[0].size(); j++){
            if(map[i][j] == myID){
                myScore++;
            }
            else if(map[i][j] == opponentID){
                opponentScore++;
            }
        }
    }
    return {myScore, opponentScore};
}

bool is_game_over(const Map &map) {
    bool plONE = false;
    bool plZERO = false;
    for (const auto &row : map) {
        for (int cell : row) {
            if (cell == PLAYER_ONE) {
                plONE = true;
            } else if (cell == PLAYER_ZERO) {
                plZERO = true;
            }
            if(plONE && plZERO){
                return false;
            }
        }
    }
    return true;
}

//=======================================================================================================
//MCTS
//=======================================================================================================

// Helper to calculate random direction
Direction getRandomDirection() {
    static random_device rd;
    static mt19937 gen(rd());
    static uniform_int_distribution<> dis(0, 3);
    return static_cast<Direction>(dis(gen));
}

// Perform a single simulation from the given map state
pair<int, int> simulateGame(Map map, int myID, int opponentID, int maxDepth) {
    bool active = myID == 1 ? true : false;
    for (int depth = 0; depth < maxDepth; ++depth) {
        Direction randomMove = getRandomDirection();
        map = move(map, randomMove, active);
        if (is_game_over(map)) {
            break;
        }
        active = !active;
        randomMove = getRandomDirection();
        map = move(map, randomMove, active);
        if (is_game_over(map)) {
            break;
        }
        active = !active;
    }
    return getScore(map, myID, opponentID);
}

Direction MCTS(const Map &map, int myID, int opponentID, int depth, int maxDepth, int maxTime){
    clock_t startTime = clock();
    vector<int> scores(4, 0); // Scores for each direction
    vector<int> visits(4, 0); // Number of simulations for each direction
    Map tempMap = map;
    int nrOfSimulations = 0;
    while ((clock() - startTime) * 1000 / CLOCKS_PER_SEC < maxTime) {
        Direction new_move = getRandomDirection();
        Map simulatedMap = move(tempMap, new_move, true);

        pair<int, int> score = simulateGame(simulatedMap, myID, opponentID, maxDepth);
        int myScore = score.first;
        int opponentScore = score.second;

        int scoreDiff = myScore - opponentScore;
        scores[new_move] += scoreDiff;
        visits[new_move]++;
        nrOfSimulations++;
    }

    // Select the direction with the best average score
    double bestAvg = -1e9;
    Direction bestMove = UP;
    for (int i = 0; i < 4; ++i) {
        if (visits[i] > 0) {
            double avgScore = static_cast<double>(scores[i]) / visits[i];
            if (avgScore > bestAvg) {
                bestAvg = avgScore;
                bestMove = static_cast<Direction>(i);
            }
        }
    }
    cerr << "Number of simulations: " << nrOfSimulations << endl;
    return bestMove;
}

int main() {
    //Zakomentowany kod do interakcji z API gry
    int my_id; // Your id, 0 or 1
    cin >> my_id; cin.ignore();
    int height; // height of the grid
    cin >> height; cin.ignore();
    int width; // width of the grid
    cin >> width; cin.ignore();

    //vector<int> game_map(width * height, -1); // Mapa gry
    Map gameMap = vector<vector<int>>(height, vector<int>(width, -1));
    // game loop
    while (1) {
        for (int i = 0; i < height; i++) {
            string line;
            getline(cin, line); // string describing tiles of a line of the grid containing values: 0 or 1: player with this id, '-': empty, 'x': hole
           // cerr<<line<<endl;
            for (int j = 0; j < line.size(); j++) {
                if (line[j] == '-'){
                    //cerr<<"EMPTY"<<endl;
                    gameMap[i][j/2] = EMPTY;
                }
                else if (line[j] == '0'){
                    //cerr<<"PLAYER_ZERO"<<endl;
                    gameMap[i][j/2] = PLAYER_ZERO;
                }
                else if (line[j] == '1'){
                    //cerr<<"PLAYER_ONE"<<endl;
                    gameMap[i][j/2] = PLAYER_ONE;
                }
                else if (line[j] == 'x'){
                    //cerr<<"HOLE"<<endl;
                    gameMap[i][j/2] = HOLE;
                }
            }
        }
        //printMap(gameMap);

        Direction move = MCTS(gameMap, my_id, my_id == 0 ? 1 : 0, 0, MAX_DEPTH, TIME_LIMIT);
        switch (move)
        {
        case UP:
            cout << "UP" << endl;
            break;
        
        case DOWN:
            cout << "DOWN" << endl;
            break;

        case LEFT:
            cout << "LEFT" << endl;
            break;

        case RIGHT:
            cout << "RIGHT" << endl;
            break;

        default:
            cout << "UP" << endl;
            break;
        }
        /* DEBUG
        // Example map
        Map map = {
            {1, 1, 1, 1, 0, 1, 1, 0},
            {1, 0, 0, 0, 0, 0, 0, 0},
            {1, 1, 1, 0, 1, 0, 0, 1},
            {1, 0, 1, 0, 0, 1, 0, 1},
            {1, 0, 1, 1, 1, 0, 0, 1},
            {0, 0, 1, 1, 1, 1, 0, 1},
            {0, 0, 0, 0, 0, 1, 0, 0},
            {1, 1, 1, 1, 1, 1, 1, 1}
        };
        Map tempMap = map;
        cerr << "Initial map:" << endl;
        printMap(map);

        // Player 0 moves up
        //map = move(map, UP, PLAYER_ZERO);
        // //cerr << "Before Player 0 moves up:" << endl;
        // //printMap(tempMap);
        // cerr << "After Player 0 moves up:" << endl;
        // printMap(map);
        
        // // Player 1 moves down
        // tempMap = map;
        // map = move(map, DOWN, PLAYER_ONE);
        // //printMap(tempMap);
        // cerr << "After Player 1 moves down:" << endl;
        // //printMap(map);
        
        //cerr << "After Player 0 moves left:" << endl;
        
        // // Player 0 moves right
        // map = move(map, RIGHT, PLAYER_ZERO);
        // cerr << "After Player 0 moves right:" << endl;
        // printMap(map);
        
        // // Player 1 moves right
        // map = move(map, RIGHT, PLAYER_ONE);
        // cerr << "After Player 1 moves right:" << endl;
        // printMap(map);

        //make 6 random moves
        for(int i = 0; i < 40; i++){
            bool isEmpty = true;
            //check is map empty
            for(int j = 0; j < map.size(); j++){
                for(int k = 0; k < map[0].size(); k++){
                    if(map[j][k] != HOLE){
                        isEmpty = false;
                        break;
                    }
                }
                if(!isEmpty){
                    break;
                }
            }
            if(isEmpty){
                cerr<<"Map is empty"<<endl;
                break;
            }
            int random = rand() % 4;
            map = move(map, random, i % 2);
            cerr << "After player "<<i % 2<<" moves ";
            if(Direction(random) == UP){
                cerr<<"UP"<<endl;
            }
            else if(Direction(random) == DOWN){
                cerr<<"DOWN"<<endl;
            }
            else if(Direction(random) == LEFT){
                cerr<<"LEFT"<<endl;
            }
            else if(Direction(random) == RIGHT){
                cerr<<"RIGHT"<<endl;
            }
            printMap(map);
        }*/
    }
}


/**
 * Try to survive by not falling off
 **/
