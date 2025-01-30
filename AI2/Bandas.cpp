#pragma GCC optimize("Ofast,inline,tracer")
#pragma GCC optimize("unroll-loops,vpt,split-loops,unswitch-loops")
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
#include <array>
#include <cassert>

using namespace std;
using State = array<int,64>;
using StateRow = array<int,8>;

#define TURN_LIMIT 200
#define TIME_LIMIT 100 - 10//ms
#define MAX_DEPTH 5
#define MAX_NODES 1024*64
#define Score pair<int, int>

bool DEBUG = false;

int height; // height of the grid
int width;  // width of the grid

// Forward declaration
struct Node;
extern array<Node, MAX_NODES> Tree;
unsigned int allocatedNodes = 0;

enum Field
{
    HOLE = -2,
    EMPTY = -1,
    PLAYER_ZERO = 0,
    PLAYER_ONE = 1,
    OCCUPIED = 2
};

enum Direction
{
    UP = 0,
    DOWN = 1,
    LEFT = 2,
    RIGHT = 3
};

// Węzeł drzewa MCTS
struct Node
{
    unsigned int visits;           // Liczba odwiedzin węzła
    double score;                  // Wynik węzła
    unsigned int children[4];      // Indeksy dzieci węzła w Tree
    int parentIdx;                 // Indeks rodzica węzła (-1 zanczy że jest rootem albo pusty)
    Direction move;                // Ruch, który prowadzi do tego węzła
    bool player_nr;                // Czy ruch który reprezentuje jest gracza 0 czy gracza 1
    bool pruned;                   // Węzeł został obcięty i nie będzie rozwijany
    bool winner;                   // Czy węzeł jest końcowy
    int myIdx;                     // Mój indeks w tablicy Tree (-1 zanczy że jest pusty)

    Node(int parentIdx = -1, Direction move = UP, bool player_nr = 0, int myIdx = -1)
        : visits(0), score(0), parentIdx(parentIdx), move(move), player_nr(player_nr), pruned(false), 
        myIdx(myIdx), winner(false)
    {
        if(myIdx != -1){ // kiedy tworzymy Node nie pusty
            assert(allocatedNodes < MAX_NODES);
            //allocatedNodes++;
        }
        fill(begin(children), end(children), 0); // Inicjalizacja dzieci na 0
    }

    ~Node()
    {
        //allocatedNodes--;
    }

    bool isFullyExpanded()
    {
        for (int i = 0; i < 4; i++)
        {
            if (children[i] == 0)
            {
                return false;
            }
        }
        return true; // 4 możliwe kierunki (UP, DOWN, LEFT, RIGHT)
    }

    int bestChildIdx(double explorationWeight)
    {
        double bestUCT = -numeric_limits<double>::infinity();
        short idx = -1;
        for (int i = 0; i < 4; ++i)
        {
            // if (Tree[children[i]].pruned)
            // {
            //     continue;
            // }
            double UCT = Tree[children[i]].score / (Tree[children[i]].visits + 1) +
                         explorationWeight * sqrt(log(visits + 1) / (Tree[children[i]].visits + 1));
            if (UCT > bestUCT)
            {
                bestUCT = UCT;
                idx = i;
            }
        }
        return idx;
    }

    // void isGameDetermined()
    // {
    //     if (!isFullyExpanded())
    //         return;
    //     bool gameFinished = true;
    //     for (int i = 0; i < 4; ++i)
    //     {
    //         if (children[i] != 0 && !Tree[children[i]].pruned)
    //         {
    //             gameFinished = false;
    //             break;
    //         }
    //     }
    //     if (gameFinished)
    //     {
    //         pruned = true;
    //     }
    // }

    void printNode()
    {
        string direction;
        switch (move)
        {
        case UP:
            direction = "UP";
            break;
        case DOWN:
            direction = "DOWN";
            break;
        case LEFT:
            direction = "LEFT";
            break;
        case RIGHT:
            direction = "RIGHT";
            break;
        }

        cerr << "Node details:\n";
        cerr << "Visits: " << visits << "\n";
        cerr << "Score: " << score << "\n";
        cerr << "Children: [" << children[0] << ", "
             << children[1] << ", " << children[2] << ", "
             << children[3] << "]\n";
        cerr << "Parent Index: " << parentIdx << "\n";
        cerr << "Move: " << direction << "\n";
        cerr << "Player Number: " << (player_nr ? "Player 1" : "Player 0") << "\n";
        cerr << "Pruned: " << (pruned ? "true" : "false") << "\n";
        cerr << "My Index: " << myIdx << "\n";
    }

    bool isRoot()
    {
        return myIdx == 0;
    }

    bool isEmpty(){
        return myIdx == -1;
    }
};

// Globalna tablica drzewo, wypełniona domyślnymi węzłami
array<Node, MAX_NODES> Tree;

void initializeTree()
{
    for (size_t i = 0; i < MAX_NODES; i++)
    {
        Tree[i] = Node();
    }
    allocatedNodes = 0;
}

int lastmove = -1;
State lastmap;

// Definicja globalnej tablicy
//Node* tree[MAX_NODES] = {nullptr};
// ~100MB

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
//==========================================================================================================================
//                                                    GAME LOGIC
//==========================================================================================================================

int wrapPosition(int row, int column){
    return row * width + column;
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
StateRow moveRow(int size, StateRow row, bool activeID){
    StateRow newRow;
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
State trimMap(const State map) {
    State newMap = map;

    for(int i = 0; i < height - 1; i++){// cięcie od góry
        int row_state = HOLE;
        for(int j = 0; j < width; j++){
            if(map[wrapPosition(i, j)] != EMPTY && map[wrapPosition(i, j)] != HOLE){
        for(int j = 0; j < width; j++){
            if(map[wrapPosition(i, j)] != EMPTY && map[wrapPosition(i, j)] != HOLE){
                row_state = OCCUPIED;
                break;
            }
            else if(map[wrapPosition(i, j)] == EMPTY){
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
            for(int j = 0; j < width; j++){
                newMap[wrapPosition(i, j)] = HOLE;
            }
            //break;
        }
    }
    for(int i = height - 1; i >= 0; i--){// cięcie od dołu
    for(int i = height - 1; i >= 0; i--){// cięcie od dołu
        int row_state = HOLE;
        for(int j = 0; j < width; j++){
            if(map[wrapPosition(i, j)] != EMPTY && map[wrapPosition(i, j)] != HOLE){
        for(int j = 0; j < width; j++){
            if(map[wrapPosition(i, j)] != EMPTY && map[wrapPosition(i, j)] != HOLE){
                row_state = OCCUPIED;
                break;
            }
            else if(map[wrapPosition(i, j)] == EMPTY){
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
            for(int j = 0; j < width; j++){
                newMap[wrapPosition(i, j)] = HOLE;
            }
            //break;
        }
    }
    for(int i = 0; i < width - 1; i++){// cięcie od lewej
    for(int i = 0; i < width - 1; i++){// cięcie od lewej
        int col_state = HOLE;
        for(int j = 0; j < height; j++){
            if(map[wrapPosition(j, i)] != EMPTY && map[wrapPosition(j, i)] != HOLE){
        for(int j = 0; j < height; j++){
            if(map[wrapPosition(j, i)] != EMPTY && map[wrapPosition(j, i)] != HOLE){
                col_state = OCCUPIED;
                break;
            }
            else if(map[wrapPosition(j, i)] == EMPTY){
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
            for(int j = 0; j < height; j++){
                newMap[wrapPosition(j, i)] = HOLE;
            }
            //break;
        }
    }
    for(int i = width - 1; i >= 0; i--){// cięcie od prawej
    for(int i = width - 1; i >= 0; i--){// cięcie od prawej
        int col_state = HOLE;
        for(int j = 0; j < height; j++){
            if(map[wrapPosition(j, i)] != EMPTY && map[wrapPosition(j, i)] != HOLE){
        for(int j = 0; j < height; j++){
            if(map[wrapPosition(j, i)] != EMPTY && map[wrapPosition(j, i)] != HOLE){
                col_state = OCCUPIED;
                break;
            }
            else if(map[wrapPosition(j, i)] == EMPTY){
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
            for(int j = 0; j < height; j++){
                newMap[wrapPosition(j, i)] = HOLE;
            }
            //break;
        }
    }
    return newMap;
}

//pomocnicza do debugu
void BprintRow(StateRow newRow, int i){
    cerr<<"\nRow : "<<i<<endl;
    cerr<<"Before moveRow"<<endl;
    for(int j = 0; j < 8; j++){
        cerr<<newRow[j]<<" ";
    }
    cerr<<endl;
}

//pomocnicza do debugu
void AprintRow(StateRow newRow, int i){
    cerr<<"\nAfter moveRow"<<endl;
    for(int j = 0; j < 8; j++){
        cerr<<newRow[j]<<" ";
    }
    cerr<<endl;
}

//Funkcja przesuwa mapę w kierunku dirIndex 
//gdzie active = true oznacza że P1 się rusza a active = false oznacza że P0 się rusza
State move(const State map, int dirIndex, bool active){
    State newMap = map;
    StateRow newRow;

    if(dirIndex == UP){//w górę
        for(int i = 0; i < width; i++){
            for(int j = 0; j < height; j++){
                newRow[j] = map[wrapPosition(j, i)];
            }
            //BprintRow(newRow, i,map.size());
            newRow = moveRow(height, newRow, active);
            newRow = moveRow(height, newRow, active);
            //AprintRow(newRow, i,map.size());
            for(int j = 0; j < height; j++){
                newMap[wrapPosition(j, i)] = newRow[j];
            for(int j = 0; j < height; j++){
                newMap[wrapPosition(j, i)] = newRow[j];
            }
        }
    }
    else if(dirIndex == DOWN){//w dół
        for(int i = 0; i < width; i++){
            for(int j = height - 1; j >= 0; j--){
                newRow[height - 1 - j] = map[wrapPosition(j, i)];
            }
            //BprintRow(newRow, i,map.size());
            newRow = moveRow(height, newRow, active);
            newRow = moveRow(height, newRow, active);
            //AprintRow(newRow, i,map.size());
            for(int j = 0; j < height; j++){
                newMap[wrapPosition(height - 1 - j, i)] = newRow[j];  
            for(int j = 0; j < height; j++){
                newMap[wrapPosition(height - 1 - j, i)] = newRow[j];  
            }
        }
    }
    else if(dirIndex == LEFT){//w lewo
        for(int i = 0; i < height; i++){
            for(int j = 0; j < width; j++){
                newRow[j] = map[wrapPosition(i, j)];
            }
            //BprintRow(newRow, i,map.size());
            newRow = moveRow(width, newRow, active);
            newRow = moveRow(width, newRow, active);
            //AprintRow(newRow, i,map.size());
            for(int j = 0; j < width; j++){
                newMap[wrapPosition(i, j)] = newRow[j];
            for(int j = 0; j < width; j++){
                newMap[wrapPosition(i, j)] = newRow[j];
            }
        }
    }
    else if(dirIndex == RIGHT){// w prawo
        for(int i = 0; i < height; i++){
            for(int j = width - 1; j >= 0; j--){
                newRow[width - 1 - j] = map[wrapPosition(i, j)];
            }
            //BprintRow(newRow, i,map[0].size());
            newRow = moveRow(width, newRow, active);
            newRow = moveRow(width, newRow, active);
            //AprintRow(newRow, i,map[0].size());
            for(int j = 0; j < width; j++){
                newMap[wrapPosition(i, width - 1 - j)] = newRow[j];
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

//Funkcja zwraca wynik gry dla gracza 0 i gracza 1
pair<int, int> getScore(const State map) {
    int P0Score = 0;
    int P1Score = 0;
    for (int i = 0; i < width * height; ++i) {
        if (map[i] == 0) {
            P0Score++;
        } else if (map[i] == 1) {
            P1Score++;
        }
    }
    return {P0Score, P1Score};
}

bool is_game_over(const State map) {
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
    return true; // Gra zakończona
}

//=======================================================================================================
//                                                FlatMC
//                                                FlatMC
//=======================================================================================================

// Helper to calculate random direction
Direction getRandomDirection() {
    return static_cast<Direction>(rand() % 4);
}

//==================================================================================================
//                                              MCTS
//==================================================================================================

//Returns -1 if game is ongoing, 0 if player 0 has won, and 1 if player 1 has won
int isGameTerminal(const State map) {
    bool playerOneExists = false;
    bool playerZeroExists = false;

    for (int i = 0; i < width * height; ++i) {
        if (map[i] == PLAYER_ONE) {
            playerOneExists = true;
        } else if (map[i] == PLAYER_ZERO) {
            playerZeroExists = true;
        }

        if (playerOneExists && playerZeroExists) {
            return -1; // Gra trwa
        }
    }
    if(playerOneExists){
        return 1;
    }
    else{
        return 0;
    }
}

//Funkcja do usuwania z pamięci wszystkich dzieci roota poza safeChild
void cleanOldBranches(Node root, int safeChild){
    if (root.isEmpty()) return; // Jeśli węzeł jest nullptr, zakończ

    // Rekurencyjnie zwolnij pamięć dla dzieci
    for (int i = 0; i < 4; ++i) {
        if(!Tree[root.children[i]].isEmpty() && root.children[i] != 0 && i!= safeChild){
            cleanOldBranches(Tree[root.children[i]], -1);
        }
    }

    // Zwolnij pamięć dla bieżącego węzła
    Tree[root.myIdx] = Node();
    allocatedNodes--;
}

// //Funkcja do ucinania branchy prowadzących do porażki
// void pruneTree(Node *root){
//     for(int i = 0; i < 4; ++i){
//         if(root->pruned && root->children[i] != 0){
//             pruneTree(tree[root->children[i]]);
//             delete tree[root->children[i]];
//             tree[root->children[i]] = nullptr;
//             root->children[i] = 0;
//         }
//     }
//     return;
// }

// //Funkcja do całkowitego czyszczenia drzewa
// void freeTree(Node* root) {
//     if (!root) return; // Jeśli węzeł jest nullptr, zakończ
    
//     // Rekurencyjnie zwolnij pamięć dla dzieci
//     for (int i = 0; i < 4; ++i) {
//         if(root->children[i] != 0){
//             freeTree(tree[root->children[i]]);
//         }
//     }

//     // Zwolnij pamięć dla bieżącego węzła jeżeli jesteś dzieckiem węzła który jest do ucięcia
//     if(root->pruned && tree[root->parentIdx]->pruned){
//         tree[root->myIdx] = nullptr;
//         delete root;
//     }
// }

//Naprawia puste miejsca w tree przesuwając wszystko do lewej
void fixTree(){
    unsigned int currentIdx = 0;
    for(int i = 0; i < allocatedNodes; ++i){
        assert(currentIdx < MAX_NODES);
        while(!Tree[i].isEmpty()){
            i++;
        }
        while(Tree[currentIdx].isEmpty()){
            currentIdx++;
        }
        //przeniesliśmy się na pierwsze puste miejsce
        Tree[i] = Tree[currentIdx];
        Tree[currentIdx] = Node();
        Tree[i].myIdx = i;
        if(i == 0){
            Tree[i].parentIdx = -1;
        }
        //poprawiamy w rodzicu indeks tego dziecka które przenieśliśmy
        if(Tree[i].parentIdx != -1){
            for(int j = 0; j < 4; ++j){
                if(Tree[Tree[i].parentIdx].children[j] == currentIdx){
                    Tree[Tree[i].parentIdx].children[j] = i;
                }
            }
        }
        //poprawiamy indeksy dzieci
        for(int j = 0; j < 4; ++j){
            if(Tree[i].children[j] != 0){//jeżeli dziecko istnieje (bo tylko root ma 0)
                Tree[Tree[i].children[j]].parentIdx = i;
            }
        }
    }
}

// Funkcja sprawdzająca limit czasu
bool check_time(const chrono::steady_clock::time_point& startTime) {
    auto currentTime = chrono::steady_clock::now();
    auto elapsedTime = chrono::duration_cast<chrono::milliseconds>(currentTime - startTime).count();
    return elapsedTime >= TIME_LIMIT;
}

//Rozgrywa grę od stanu map zaczynając od gracza first_move i zwraca wynik gry (P0, P1)
pair<int, int> MCTSsim(State map, bool first_move, int maxDepth, const chrono::steady_clock::time_point& startTime) {

    State currMap = map;
    bool active = first_move;
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
    }

    return getScore(map);
}


pair<Node, State> Selection(Node root, State map){
    assert(!root.isEmpty());

    while(root.isFullyExpanded()){
        root = Tree[root.bestChildIdx(0.7)];
        move(map, root.move, root.player_nr);
    }

    assert(!root.isFullyExpanded());
    return {root, map};
}

Node Expansion(Node current){
    assert(!current.isFullyExpanded());

    for(int i = 0; i < 4; ++i){
        if(current.children[i] == 0){
            assert(allocatedNodes < MAX_NODES);
            assert(allocatedNodes > 0);
            assert(Tree[allocatedNodes].isEmpty());
            Tree[allocatedNodes] = 
                Node(current.myIdx, static_cast<Direction>(i), current.player_nr ? 0 : 1, allocatedNodes);
            Node child = Tree[allocatedNodes];
            assert(!child.isEmpty());
            assert(!child.isRoot());
            allocatedNodes++;
            Tree[current.myIdx].children[i] = child.myIdx;
            assert(allocatedNodes <= MAX_NODES);
            return child;
        }
    }
    return Node();
}

Score Simulation(Node leaf, State map, std::chrono::_V2::steady_clock::time_point startTime){
    assert(!leaf.isFullyExpanded());
    assert(!leaf.isEmpty());
    assert(!leaf.isRoot());

    map = move(map, leaf.move, leaf.player_nr);
    if(is_game_over(map)){
        pair<int, int> score = getScore(map);
        if(score.first > score.second){
            return {100, -100};
        }
        else{
            return {-100, 100};
        }
    }

    assert(!is_game_over(map));
    pair<int,int> simResult = MCTSsim(map, leaf.player_nr, MAX_DEPTH, startTime);
    return simResult;
}

void Backpropagation(Node current, pair<int,int> score, bool myId){
    assert(!current.isEmpty());
    assert(!current.isRoot());

    int myScore, opponentScore;
    if(myId){
        myScore = score.second;
        opponentScore = score.first;
    }
    else{
        myScore = score.first;
        opponentScore = score.second;
    }

    while(!current.isRoot()){
        Tree[current.myIdx].visits++;
        Tree[current.myIdx].score += myScore - opponentScore;
        current = Tree[current.parentIdx];
    }
    // current.visits++;
    // current.score += myScore - opponentScore;
}

// Główna funkcja MCTS
Direction MCTS(State map, int myID, int opponentID, int maxDepth) {
    // Tworzenie drzewa MCTS
    assert(allocatedNodes < MAX_NODES);
    State mapCopy = map;
    Node Root = Tree[0];
    //Root.printNode();
    Node child;
    // Start czasu
    auto startTime = chrono::steady_clock::now();
    //root->printNode();
    cerr << "MCTS\n";
    int i = 0;
    // Główna pętla MCTS
    while(true && i < 6500){
        Node current = Root;
        State currentMapCopy = mapCopy;

        //1. Selection
        pair<Node, State> selectionResult = Selection(current, mapCopy);
        current = selectionResult.first;
        currentMapCopy = selectionResult.second;
        // Sprawdzenie czasu
        if (check_time(startTime)) break;

        //2. Expansion
        child = Expansion(current);
        assert(!child.isEmpty());
        current = child;
        // Sprawdzenie czasu
        if (check_time(startTime)) break;

        //3. Simulation
        Score simulationResult = Simulation(current, currentMapCopy, startTime);
        // Sprawdzenie czasu
        if (check_time(startTime)) break;

        //4. Backpropagation
        Backpropagation(current, simulationResult, myID == 1);
        // Sprawdzenie czasu
        if (check_time(startTime)) break;
    }
    int bestChildIdx = Root.bestChildIdx(0);
    Node bestChild = Tree[Root.children[bestChildIdx]];
    assert(!bestChild.isEmpty());
    cleanOldBranches(Root, bestChildIdx);
    fixTree();

    if(DEBUG){//Print 6 first nodes of the tree
        for(int i = 0; i < 6; i++){
            cerr << "Node " << i << endl;
            Tree[i].printNode();
        }

    }
    lastmap = move(map, bestChild.move, myID);
    return bestChild.move;
}

void Reparenting(State gamemap, bool my_id){
    cerr << "Starting Reparenting\n";
    assert(Tree[0].player_nr == my_id);
    assert(lastmove != -1);
    assert(!Tree[0].isEmpty());
    assert(Tree[0].isRoot());
    for(int i = 0; i < 4; i++){
        State lastMapCopy = lastmap;
        lastMapCopy = move(lastMapCopy, i, my_id == 0 ? 1 : 0);
        assert(lastMapCopy != lastmap);
        bool theSame = true;
        for(int j = 0; j < 64; ++j){
            if(gamemap[j] != lastMapCopy[j]){
                theSame = false;
                break;
            }
        }
        if(theSame){
            for(int j = 0; j < 4; j++){
                if(Tree[0].children[j] != 0 && Tree[Tree[0].children[j]].move == i){
                    cleanOldBranches(Tree[0], j);
                    fixTree();
                    assert(Tree[0].isRoot());
                    assert(Tree[0].player_nr != my_id);
                    return;
                }
            }
        }
    }
}

bool hardcode = false;

int main() {//ios_base.sync(0)
    initializeTree();
    int my_id; // Your id, 0 or 1
    if(!hardcode){
        cin >> my_id;
        cin >> height;
        cin >> width;
    }
    else{
        my_id = 0;
        height = 8;
        width = 8;
    }

    Tree[0] = Node(-1, UP, !my_id, 0);
    allocatedNodes = 1;

    State gamemap;
    for(int i = 0; i < width * height; ++i){
        gamemap[i] = EMPTY;
    }
    // game loop
    while (1) {
        //array input
        if(!hardcode){
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
        }
        else{
            gamemap = {0, 0, 0, 0, 1, 1, 1, 0, 
                        0, 1, 0, 1, 1, 0, 1, 1, 
                        0, 0, 0, 0, 0, 0, 1, 0, 
                        1, 1, 1, 1, 1, 0, 0, 1, 
                        0, 1, 1, 1, 0, 1, 0, 1, 
                        0, 0, 1, 1, 0, 0, 1, 1, 
                        1, 0, 1, 1, 0, 1, 1, 0, 
                        0, 1, 0, 0, 1, 0, 1, 0};
        }
        if(DEBUG){// print cały input
            cerr << "my_id: " << my_id << endl;
            cerr << "height: " << height << endl;
            cerr << "width: " << width << endl;
            cerr << "gamemap: " << endl;
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    //print cin input of game map
                    if(gamemap[i * width + j] == EMPTY){
                        cerr << "- ";
                    }
                    else if(gamemap[i * width + j] == PLAYER_ZERO){
                        cerr << "0 ";
                    }
                    else if(gamemap[i * width + j] == PLAYER_ONE){
                        cerr << "1 ";
                    }
                    else if(gamemap[i * width + j] == HOLE){
                        cerr << "x ";
                    }
                }
                cerr << endl;
            }      
        }

        // cerr<<"about to clean tree[0]->player_nr : "<<tree[0]->player_nr<<endl;
        
        cerr << "Allocated nodes BEFORE REPARENTING: " << allocatedNodes << endl;
        cerr << "Root player_nr BEFORE REPARENTING: " << Tree[0].player_nr << endl;
        //Sprawdź jaki ruch zrobił przeciwnik i odetnij niepotrzebne Nody (powien zmienić player_nr roota)
        if(lastmove != -1){
            cerr << "Reparenting\n";
            Reparenting(gamemap, my_id);
        }
        cerr<<"Root player_nr: "<<Tree[0].player_nr<<endl;
        cerr << "Allocated nodes BEFORE: " << allocatedNodes << endl;
        bool lastPLR = Tree[0].player_nr;
        Direction move = MCTS(gamemap, my_id, my_id == 0 ? 1 : 0, MAX_DEPTH);//wchodząc do MCTS root powinien być wroga
        assert(lastPLR != Tree[0].player_nr);
        cerr << "Allocated nodes AFTER: " << allocatedNodes << endl;
        cerr<<"Root player_nr: "<<Tree[0].player_nr<<endl;
        switch (move)
        {
        case UP:
            cout << "UP" << endl;
            lastmove = 0;
            break;
        
        case DOWN:
            cout << "DOWN" << endl;
            lastmove = 1;
            break;

        case LEFT:
            cout << "LEFT" << endl;
            lastmove = 2;
            break;

        case RIGHT:
            cout << "RIGHT" << endl;
            lastmove = 3;
            break;

        default:
            cout << "UP" << endl;
            lastmove = 0;
            break;
        }
    }
}
    }
}