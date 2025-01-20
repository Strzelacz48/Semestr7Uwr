#pragma GCC optimize("Ofast,inline,tracer")
#pragma GCC optimize("unroll-loops,vpt,split-loops,unswitch-loops")
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <queue>
#include <random>
#include <time.h>
#include <chrono>
#include <ctime>

using namespace std;

#define TURN_LIMIT 200
#define TIME_LIMIT 100 - 10//ms
#define MAX_DEPTH 5
bool DEBUG = true;

int height; // height of the grid
int width; // width of the grid

int allocatedNodes = 0;

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

// Węzeł drzewa MCTS
struct Node {
unsigned int visits;                   // Liczba odwiedzin węzła
    int score;                         // Wynik węzła
    vector<Node*> children;            // Dzieci węzła
    Node* parent;                      // Rodzic węzła
    Direction move;                    // Ruch, który prowadzi do tego węzła
    bool player_nr;                    // Czy ruch który reprezentuje jest gracza 0 czy gracza 1
    bool enemy_won = false;
    bool i_win = false;
    bool pruned = false;               // Węzeł został obcięty i nie będzie rozwijany

    Node(Node* parent, Direction move, bool player_nr)
        : visits(0), score(0), parent(parent), move(move), player_nr(player_nr) {
    ++allocatedNodes;
    }

    ~Node() {
    --allocatedNodes;
    }

    bool isFullyExpanded() const {
        return children.size() == 4; // 4 możliwe kierunki (UP, DOWN, LEFT, RIGHT)
    }

    Node* bestChild(double explorationWeight) {
        return *std::max_element(children.begin(), children.end(), [&](Node* a, Node* b) {
            if (a->pruned) return false;
            if (b->pruned) return true;
            double uctA = a->score / (a->visits + 1) +
                          explorationWeight * sqrt(log(visits + 1) / (a->visits + 1));
            double uctB = b->score / (b->visits + 1) +
                          explorationWeight * sqrt(log(visits + 1) / (b->visits + 1));
            return uctA < uctB;
        });
    }

    void prune() {
        pruned = true;
        if (parent) {
            bool allPruned = true;
            for (const auto& sibling : parent->children) {
                if (!sibling->pruned) {
                    allPruned = false;
                    break;
                }
            }
            if (allPruned) {
                parent->prune(); // Rekurencyjnie oznacz rodziców
            }
        }
    }

    void isGameDetermined(){
        if (!isFullyExpanded())
            return;
        bool gameFinished = true;
        for(int i = 0; i < children.size(); ++i){
            if(!children[i]->enemy_won){
                gameFinished = false;
                break;
            }
        }
        if(gameFinished){
            enemy_won = true;
            return;
        }
        for(int i = 0; i < children.size(); ++i){
            if(!children[i]->i_win){
                gameFinished = false;
                break;
            }
        }
        if(gameFinished){
            i_win = true;
            return;
        }
    }
    
};
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

int wrapPosition(int row, int column){
    return row * width + column;
}

void printMap(const Map& map) {
    for (const auto &row : map) {
        for (int cell : row) {
            if (cell == HOLE) {
                cerr << "\033[40m\033[30mH \033[0m"; // Black background, black text
            } else if (cell == EMPTY) {
                cerr << "\033[40m\033[37m. \033[0m"; // Black background, white text
            } else if (cell == PLAYER_ZERO) {
                cerr << "\033[44m\033[37m0 \033[0m"; // Blue background, white text
            } else if (cell == PLAYER_ONE) {
                cerr << "\033[41m\033[37m1 \033[0m"; // Red background, white text
            }
        }
        cerr << endl;
    }
    cerr << endl;
}

// Funkcja do wypisania zawartości tablicy gamemap jako prostokąt
void printGamemap(const int gamemap[]) {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            cerr << gamemap[y * width + x] << " ";
        }
        cerr << "\n"; // Nowa linia po każdym wierszu
    }
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
int* trimMap(const int* map) {
    int* newMap = new int[width * height];
    for(int i = 0; i < height * width; ++i){
        newMap[i] = map[i];
    }
    for(int i = 0; i < height - 1; i++){// cięcie od góry
        int row_state = HOLE;
        for(int j = 0; j < width; j++){
            if(map[wrapPosition(i, j)] != EMPTY && map[wrapPosition(i, j)] != HOLE){
                row_state = OCCUPIED;
                break;
            }
            else if(map[wrapPosition(i, j)] == EMPTY){
                row_state = EMPTY;
            }
        }
        if(row_state == OCCUPIED){
            break;
        }
        else if(row_state == EMPTY){
            for(int j = 0; j < width; j++){
                newMap[wrapPosition(i, j)] = HOLE;
            }
            //break;
        }
    }
    for(int i = height - 1; i >= 0; i--){// cięcie od dołu
        int row_state = HOLE;
        for(int j = 0; j < width; j++){
            if(map[wrapPosition(i, j)] != EMPTY && map[wrapPosition(i, j)] != HOLE){
                row_state = OCCUPIED;
                break;
            }
            else if(map[wrapPosition(i, j)] == EMPTY){
                row_state = EMPTY;
            }
        }
        if(row_state == OCCUPIED){
            break;
        }
        else if(row_state == EMPTY){
            for(int j = 0; j < width; j++){
                newMap[wrapPosition(i, j)] = HOLE;
            }
            //break;
        }
    }
    for(int i = 0; i < width - 1; i++){// cięcie od lewej
        int col_state = HOLE;
        for(int j = 0; j < height; j++){
            if(map[wrapPosition(j, i)] != EMPTY && map[wrapPosition(j, i)] != HOLE){
                col_state = OCCUPIED;
                break;
            }
            else if(map[wrapPosition(j, i)] == EMPTY){
                col_state = EMPTY;
            }
        }
        if(col_state == OCCUPIED){
            break;
        }
        else if(col_state == EMPTY){
            for(int j = 0; j < height; j++){
                newMap[wrapPosition(j, i)] = HOLE;
            }
            //break;
        }
    }
    for(int i = width - 1; i >= 0; i--){// cięcie od prawej
        int col_state = HOLE;
        for(int j = 0; j < height; j++){
            if(map[wrapPosition(j, i)] != EMPTY && map[wrapPosition(j, i)] != HOLE){
                col_state = OCCUPIED;
                break;
            }
            else if(map[wrapPosition(j, i)] == EMPTY){
                col_state = EMPTY;
            }
        }
        if(col_state == OCCUPIED){
            break;
        }
        else if(col_state == EMPTY){
            for(int j = 0; j < height; j++){
                newMap[wrapPosition(j, i)] = HOLE;
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
int* move(const int* map, int dirIndex, bool active){
    const auto &dir = directions[dirIndex];
    int* newMap = new int[height * width];
    for(int i = 0; i < height * width; ++i){
        newMap[i] = map[i];
    }
    if(dirIndex == UP){//w górę
        for(int i = 0; i < width; i++){
            int* newRow = new int[width];
            for(int j = 0; j < height; j++){
                newRow[j] = map[wrapPosition(j, i)];
            }
            //BprintRow(newRow, i,map.size());
            newRow = moveRow(height, newRow, active);
            //AprintRow(newRow, i,map.size());
            for(int j = 0; j < height; j++){
                newMap[wrapPosition(j, i)] = newRow[j];
            }
        }
    }
    else if(dirIndex == DOWN){//w dół
        for(int i = 0; i < width; i++){
            int* newRow = new int[width];
            for(int j = height - 1; j >= 0; j--){
                newRow[height - 1 - j] = map[wrapPosition(j, i)];
            }
            //BprintRow(newRow, i,map.size());
            newRow = moveRow(height, newRow, active);
            //AprintRow(newRow, i,map.size());
            for(int j = 0; j < height; j++){
                newMap[wrapPosition(height - 1 - j, i)] = newRow[j];  
            }
        }
    }
    else if(dirIndex == LEFT){//w lewo
        for(int i = 0; i < height; i++){
            int* newRow = new int[width];
            for(int j = 0; j < width; j++){
                newRow[j] = map[wrapPosition(i, j)];
            }
            //BprintRow(newRow, i,map.size());
            newRow = moveRow(width, newRow, active);
            //AprintRow(newRow, i,map.size());
            for(int j = 0; j < width; j++){
                newMap[wrapPosition(i, j)] = newRow[j];
            }
        }
    }
    else if(dirIndex == RIGHT){// w prawo
        for(int i = 0; i < height; i++){
            int* newRow = new int[width];
            for(int j = width - 1; j >= 0; j--){
                newRow[width - 1 - j] = map[wrapPosition(i, j)];
            }
            //BprintRow(newRow, i,map[0].size());
            newRow = moveRow(width, newRow, active);
            //AprintRow(newRow, i,map[0].size());
            for(int j = 0; j < width; j++){
                newMap[wrapPosition(i, width - 1 - j)] = newRow[j];
            }
        }
    }
    //cerr << "After Player moves:" << endl;
    //printMap(newMap);
    //Trim empty sides
    newMap = trimMap(newMap);

    return newMap;
}

pair<int, int> getScore(const int* map, int myID, int opponentID) {
    int myScore = 0;
    int opponentScore = 0;
    for (int i = 0; i < width * height; ++i) {
        if (map[i] == myID) {
            myScore++;
        } else if (map[i] == opponentID) {
            opponentScore++;
        }
    }
    return {myScore, opponentScore};
}

bool is_game_over(const int* map) {
    bool playerOneExists = false;
    bool playerZeroExists = false;

    for (int i = 0; i < width * height; ++i) {
        if (map[i] == PLAYER_ONE) {
            playerOneExists = true;
        } else if (map[i] == PLAYER_ZERO) {
            playerZeroExists = true;
        }

        if (playerOneExists && playerZeroExists) {
            return false; // Gra trwa
        }
    }
    return true; // Gra zakończona
}

//=======================================================================================================
//                                                FlatMC
//=======================================================================================================

// Helper to calculate random direction
Direction getRandomDirection() {
    return static_cast<Direction>(rand() % 4);
}

// Perform a single simulation from the given map state
pair<int, int> simulateGame(const int* map, int myID, int opponentID, int maxDepth) {
    int* currMap = new int[width * height];
    copy(map, map + width * height, currMap);
    
    bool active = myID == 1 ? true : false;

    for (int depth = 0; depth < maxDepth; ++depth) {
        Direction randomMove = getRandomDirection();
        currMap = move(currMap, randomMove, active);

        if (is_game_over(currMap)) {
            break;
        }

        active = !active;
        randomMove = getRandomDirection();
        currMap = move(currMap, randomMove, active);

        if (is_game_over(currMap)) {
            break;
        }
        active = !active;
    }
    return getScore(map, myID, opponentID);
}

Direction FlatMC(const int *map, int myID, int opponentID, int depth, int maxDepth, int maxTime){//timeout
    using namespace std::chrono;
    // Pobieramy czas startu
    clock_t start = std::clock();

    vector<int> scores(4, 0); // Scores for each direction
    vector<int> visits(4, 0); // Number of simulations for each direction
    int* tempMap = new int[width * height];
    for(int i = 0; i < width * height; i++){
        tempMap[i] = map[i];
    }
    int nrOfSimulations = 0;
    int* simulatedMap = new int[height * width];
    while (double(std::clock() - start) / CLOCKS_PER_SEC * 1000.0 < maxTime - 2) {//trzeba poprawić na czas realny nie procesu (chrono)
        Direction new_move = getRandomDirection();
        simulatedMap = move(tempMap, new_move, true);

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
//==================================================================================================
//                                              MCTS
//==================================================================================================
void freeTree(Node* root) {
    if (!root) return; // Jeśli węzeł jest nullptr, zakończ

    // Rekurencyjnie zwolnij pamięć dla dzieci
    for (Node* child : root->children) {
        freeTree(child);
    }

    // Zwolnij pamięć dla bieżącego węzła
    delete root;
}

// Funkcja sprawdzająca limit czasu
bool check_time(const chrono::steady_clock::time_point& startTime) {
    auto currentTime = chrono::steady_clock::now();
    auto elapsedTime = chrono::duration_cast<chrono::milliseconds>(currentTime - startTime).count();
    return elapsedTime >= TIME_LIMIT;
}

//simulate one turn for MCTS
pair<bool, pair<int, int>> MCTSsim(const int* map, int myID, int opponentID, int maxDepth, const chrono::steady_clock::time_point& startTime) {
    int* currMap = new int[width * height];
    copy(map, map + width * height, currMap);
    int Turn = 0;
    bool active = myID == 1 ? true : false;
    clock_t currentTime;

    for (int depth = 0; depth < maxDepth; ++depth) {
        if (is_game_over(currMap)) {
            break;
        }

        Direction randomMove = getRandomDirection();
        currMap = move(currMap, randomMove, active);

        // Sprawdzenie czasu
        if (check_time(startTime)) break;

        if (is_game_over(currMap)) {
            break;
        }

        active = !active;
        randomMove = getRandomDirection();
        currMap = move(currMap, randomMove, active);

        // Sprawdzenie czasu
        if (check_time(startTime)) break;

        if (is_game_over(currMap)) {
            break;
        }
        active = !active;
        Turn++;
    }
    delete[] currMap;

    return {Turn == 0, getScore(map, myID, opponentID)};
}

// Główna funkcja MCTS
Direction MCTS(int* map, int myID, int opponentID, int depth, int maxDepth, int maxTime) {
    // Tworzenie drzewa MCTS
    Node* root = new Node(nullptr, UP, !myID); // ruch nie powinine się liczyć bo wykorzystujemy dziecko (rootem jest wróg bo wybieram najlepszy ruch z dzieci)
    int* mapCopy = new int[height * width];

    // Start czasu
    auto startTime = chrono::steady_clock::now();
    cerr << "MCTS\n";
    // Główna pętla MCTS
    while (true) {
        // Sprawdzenie czasu
        if (check_time(startTime)) break;
        
        copy(map, map + width * height, mapCopy);
        if(DEBUG && false){
            cerr << "Selection phase\n";
        }
        // 1. Selection: Wybór najlepszego węzła do rozwinięcia
        Node* current = root;
        while (!current->children.empty() && current->isFullyExpanded()) {
            current = current->bestChild(1.41); // Exploration weight (sqrt(2))
            if (current->pruned) break; // Jeśli węzeł jest obcięty, przerywamy rozwijanie
            mapCopy = move(mapCopy, current->move, current->player_nr);
        }

        // Sprawdzenie czasu
        if (check_time(startTime)) break;

        
        if(DEBUG && false){
            cerr << "Expansion phase\n";
        }

        // 2. Expansion: Rozwinięcie węzła
        if (!current->isFullyExpanded()) {
            for (int i = 0; i < 4; ++i) {
                Direction dir = static_cast<Direction>(i);
                bool alreadyExpanded = false;
                for (auto child : current->children) {
                    if (child->move == dir || child->pruned) {
                        alreadyExpanded = true;
                        break;
                    }
                }
                if (!alreadyExpanded) {
                    Node* child = new Node(current, dir, current->player_nr ? 0 : 1);
                    
                    current->children.push_back(child);
                    current = child;
                    
                    mapCopy = move(mapCopy, current->move, current->player_nr);
                    break;
                }
            }
        }

        // Sprawdzenie czasu
        if (check_time(startTime)) break;

        if(DEBUG && false){
            cerr << "Simulation phase\n";
        }

        // 3. Simulation: Symulacja gry od aktualnego węzła //TODO upewnić się że odpowiednie węzły są symulowane i napisać symulację dla gracza danego w argumencie
        pair<bool, pair<int, int>> resultpair = MCTSsim(mapCopy, myID, opponentID, maxDepth, startTime);
        int result = resultpair.second.first - resultpair.second.second;
        if (resultpair.second.first == 0 && resultpair.first){
            current->enemy_won = true;
        }
        else if(resultpair.second.second == 0){
            current->i_win = true;
        }

        // Sprawdzenie czasu
        if (check_time(startTime)) break;
        
        if(DEBUG && false){
            cerr << "Backpropagation phase\n";
        }
        // 4. Backpropagation: Aktualizacja drzewa
        while (current != nullptr) {
            current->visits++;
            current->score += result;
            current->isGameDetermined();

            if (current->enemy_won) {
                current->prune(); // Oznacz węzeł jako pruned
            }

            current = current->parent;
        }
        if(allocatedNodes >= 6500){
            break;
        }
    }
    cerr << "Pick best child\n";
    std::cerr << "Allocated nodes: " << allocatedNodes << std::endl;
    // Wybór najlepszego ruchu z korzenia
    Node* bestMove = root->bestChild(0.0); // Exploration weight = 0 (tylko exploitation)
    if(bestMove->enemy_won){
        for(int i = 0; i < root->children.size(); ++i){
            if(!root->children[i]->enemy_won){
                bestMove = root->children[i];
            }
        }
    }
    Direction bestDirection = bestMove->move;

    //cerr<< "About to clear memory";

    freeTree(root);
    std::cerr << "Allocated nodes: " << allocatedNodes << std::endl;
    return bestDirection;
}

int main() {//ios_base.sync(0)
    //Zakomentowany kod do interakcji z API gry
    int my_id; // Your id, 0 or 1
    cin >> my_id;
    cin >> height;
    cin >> width;

    //vector<int> game_map(width * height, -1); // Mapa gry
    Map gameMap = vector<vector<int>>(height, vector<int>(width, -1));

    int* gamemap = new int[width * height];
    for(int i = 0; i < width * height; ++i){
        gamemap[i] = -1;
    }

    // game loop
    while (1) {
        //array input
        for (int i = 0; i < height * width; i++) {
            char letter;
            cin>>letter; // string describing tiles of a line of the grid containing values: 0 or 1: player with this id, '-': empty, 'x': hole
           
            if(letter == ' '){
                cin>>letter;
            }
            if (letter == '-'){
                //cerr<<"EMPTY"<<endl;
                gamemap[i] = EMPTY;
            }
            else if (letter == '0'){
                //cerr<<"PLAYER_ZERO"<<endl;
                gamemap[i] = PLAYER_ZERO;
            }
            else if (letter == '1'){
                //cerr<<"PLAYER_ONE"<<endl;
                gamemap[i] = PLAYER_ONE;
            }
            else if (letter == 'x'){
                //cerr<<"HOLE"<<endl;
                gamemap[i] = HOLE;
            }
        }
        /*if(DEBUG){
            printGamemap(gamemap);
        }*/

        Direction move = MCTS(gamemap, my_id, my_id == 0 ? 1 : 0, 0, MAX_DEPTH, TIME_LIMIT);
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
    }
}