#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <queue>

#pragma GCC optimize("Ofast,inline,tracer")
#pragma GCC optimize("unroll-loops,vpt,split-loops,unswitch-loops")

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

void printMap(const Map &map) {
    for (const auto &row : map) {
        for (int cell : row) {
            if (cell == HOLE) {
                cerr << "H ";
            } else if (cell == EMPTY) {
                cerr << ". ";
            } else if (cell == PLAYER_ZERO) {
                cerr << "0 ";
            } else if (cell == PLAYER_ONE) {
                cerr << "1 ";
            }
        }
        cerr << endl;
    }
    cerr << endl;
}

//funkcja która powinna decydować czy dane pole się rusza czy nie
int willIMove(int myID, int neighbor, bool activeID){
    if(myID == EMPTY || myID == HOLE){
        return 1;//nie rusz sie
    }
    if(activeID == myID){
        return 3;//aktywny gracz się rusza(zamień się na puste pole)
    }
    if(myID == neighbor){
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
int* moveRow(int size, int* row, int dir, bool activeID){
    int* newRow = new int[size];
    int move;
    if(dir == 0){
        for(int i = 0; i < size; i++){
            if(i == size - 1){
                if(row[i] == activeID){
                    newRow[i] = EMPTY;
                }
                else{
                    newRow[i] = row[i];
                }
                break;
            }
            else{
                move = willIMove(row[i], row[i + 1], activeID);
                int ii = i;
                while(move == 0){
                    ii++;
                    if(ii >= size - 2){
                        // cerr<<" i = "<<i<<endl;
                        // cerr<<"row[i] = "<<row[i]<<endl;
                        // cerr<<"row[i + 1] = "<<row[i + 1]<<endl;
                        // cerr<<"activeID = "<<activeID<<endl;
                        // move = 4;
                        // if(row[i] == activeID){
                        //     newRow[i] = activeID;
                        //     newRow[i + 1] = EMPTY;
                        // }
                        // else{
                        //     newRow[i] = row[i];
                        //     newRow[i + 1] = row[i + 1];
                        //     cerr<<"newRow[i] = "<<newRow[i]<<endl;
                        //     cerr<<"newRow[i + 1] = "<<newRow[i + 1]<<endl;

                        // }
                        break;
                    }
                    move = willIMove(row[ii], row[ii + 1], activeID);
                    
                }
                if(move == 1){
                    newRow[i] = row[i];
                    continue;
                }
                else if(move == 2){
                    if(i > 0 && row[i - 1] != HOLE){
                        newRow[i - 1] = row[i];
                    }
                    newRow[i] = row[i + 1];   
                }
                else if(move == 3){
                    if(i > 0 && row[i - 1] != HOLE){
                        newRow[i - 1] = row[i];
                    }
                    newRow[i] = EMPTY;
                }
            }
        }
        
    }
    else if(dir == 1){
        for(int i = size; i > 0; i--){
            int move = willIMove(row[i], row[i - 1], activeID);
            int ii = i;
            while(move == 0){
                ii--;
                if(ii <= 1){
                    move = 1;
                    break;
                }
                move = willIMove(row[ii], row[ii - 1], activeID);
            }
            if(move == 1){
                newRow[i] = row[i];
                continue;
            }
            else if(move == 2){
                if(i < size - 1 && row[i + 1] != HOLE){
                    newRow[i + 1] = row[i];
                }
                newRow[i] = row[i - 1];
            }
            else if(move == 3){
                if(i < size - 1 && row[i + 1] != HOLE){
                    newRow[i + 1] = row[i];
                }
                newRow[i] = EMPTY;
            }
        }
    }

    return newRow;
}

//Funkcja przycina mapę puste krawędzie z każdej strony (raz na koniec tury) (odcięte pola są zastępowane przez HOLE)
Map trimMap(const Map &map) {
    Map newMap = map;
    for(int i = 0; i < map.size() - 1; i++){// cięcie od góry
        bool empty = true;
        for(int j = 0; j < map[i].size(); j++){
            if(map[i][j] != EMPTY && map[i][j] != HOLE){
                empty = false;
                break;
            }
        }
        if(empty){
            for(int j = 0; j < map[i].size(); j++){
                newMap[i][j] = HOLE;
            }
            break;
        }
    }
    for(int i = map.size() - 1; i >= 0; i--){// cięcie od dołu
        bool empty = true;
        for(int j = 0; j < map[i].size(); j++){
            if(map[i][j] != EMPTY && map[i][j] != HOLE){
                empty = false;
                break;
            }
        }
        if(empty){
            for(int j = 0; j < map[i].size(); j++){
                newMap[i][j] = HOLE;
            }
            break;
        }
    }
    for(int i = 0; i < map[0].size() - 1; i++){// cięcie od lewej
        bool empty = true;
        for(int j = 0; j < map.size(); j++){
            if(map[j][i] != EMPTY && map[j][i] != HOLE){
                empty = false;
                break;
            }
        }
        if(empty){
            for(int j = 0; j < map.size(); j++){
                newMap[j][i] = HOLE;
            }
            break;
        }
    }
    for(int i = map[0].size() - 1; i >= 0; i--){// cięcie od prawej
        bool empty = true;
        for(int j = 0; j < map.size(); j++){
            if(map[j][i] != EMPTY && map[j][i] != HOLE){
                empty = false;
                break;
            }
        }
        if(empty){
            for(int j = 0; j < map.size(); j++){
                newMap[j][i] = HOLE;
            }
            break;
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
    if(dirIndex == 0){//w górę
        for(int i = 0; i < map[0].size(); i++){
            int* newRow = new int[map[0].size()];
            for(int j = 0; j < map.size(); j++){
                newRow[j] = map[j][i];
            }
            BprintRow(newRow, i,map.size());
            newRow = moveRow(map.size(), newRow, 0, active);
            AprintRow(newRow, i,map.size());
            for(int j = 0; j < map.size(); j++){
                newMap[j][i] = newRow[j];
            }
        }
    }
    else if(dirIndex == 1){//w dół
        for(int i = 0; i < map[0].size(); i++){
            int* newRow = new int[map[0].size()];
            for(int j = 0; j < map.size(); j++){
                newRow[map.size() - 1 - j] = map[j][i];//newRow[j] = map[j][i];
            }
            BprintRow(newRow, i,map.size());
            newRow = moveRow(map.size(), newRow, 0, active);
            AprintRow(newRow, i,map.size());
            for(int j = 0; j < map.size(); j++){
                newMap[j][i] = newRow[map.size() - 1 - j];
            }
        }
    }
    else if(dirIndex == 2){//w lewo
        for(int i = 0; i < map.size(); i++){
            int* newRow = new int[map.size()];
            for(int j = 0; j < map[0].size(); j++){
                newRow[j] = map[i][j];
            }
            BprintRow(newRow, i,map.size());
            newRow = moveRow(map[0].size(), newRow, 0, active);
            AprintRow(newRow, i,map.size());
            for(int j = 0; j < map[0].size(); j++){
                newMap[i][j] = newRow[j];
            }
        }
    }
    else if(dirIndex == 3){// w prawo
        for(int i = 0; i < map.size(); i++){
            int* newRow = new int[map.size()];
            for(int j = 0; j < map[0].size(); j++){
                newRow[j] = map[i][j];
            }
            BprintRow(newRow, i,map.size());
            newRow = moveRow(map[0].size(), newRow, 1, active);
            AprintRow(newRow, i,map.size());
            for(int j = 0; j < map[0].size(); j++){
                newMap[i][j] = newRow[j];
            }
        }
    }
    cerr << "After Player moves:" << endl;
    printMap(newMap);
    //Trim empty sides
    newMap = trimMap(newMap);

    return newMap;
}

int main() {
    //Zakomentowany kod do interakcji z API gry
    /*int my_id; // Your id, 0 or 1
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
        printMap(gameMap);*/
        // Example map
        Map map = {
            {1, 1, 1, 1, 0, 1, 1, 0},
            {1, 0, 0, 0, 0, 0, 0, 0},
            {1, 1, 1, 0, 1, 0, 0, 1},
            {1, 0, 1, 0, 0, 1, 0, 1},
            {1, 0, 1, 1, 1, 0, 0, 1},
            {0, 0, 1, 1, 1, 1, 0, 1},
            {0, 0, 0, 0, 0, 1, 0, 0}//,
            //{1, 1, 1, 1, 1, 1, 1, 1}
        };
        Map tempMap = map;
        cerr << "Initial map:" << endl;
        printMap(map);

        // Player 0 moves up
        map = move(map, 0, PLAYER_ZERO);
        cerr << "Before Player 0 moves up:" << endl;
        printMap(tempMap);
        cerr << "After Player 0 moves up:" << endl;
        printMap(map);
        
        // Player 1 moves right
        tempMap = map;
        map = move(map, 1, PLAYER_ONE);
        cerr << "Before Player 1 moves down:" << endl;
        printMap(tempMap);
        cerr << "After Player 1 moves down:" << endl;
        printMap(map);
        
        // // Player 0 moves up
        // map = move(map, 2, PLAYER_ONE);
        // cerr << "After Player 0 moves left:" << endl;
        // printMap(map);

        // // Player 1 moves right
        // map = move(map, 3, PLAYER_ZERO);
        // cerr << "After Player 1 moves right:" << endl;
        // printMap(map);

    //}
}


/**
 * Try to survive by not falling off
 **/
