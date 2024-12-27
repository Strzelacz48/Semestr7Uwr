/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.

int main()
{
    for (int i = 0; i < 10; i++) {
        string line;
        getline(cin, line);
    }
    int robot_count;
    cin >> robot_count; cin.ignore();
    for (int i = 0; i < robot_count; i++) {
        int x;
        int y;
        string direction;
        cin >> x >> y >> direction; cin.ignore();
    }

    // Write an action using cout. DON'T FORGET THE "<< endl"
    // To debug: cerr << "Debug messages..." << endl;

    cout << "0 0 U 1 1 R 2 2 D 3 3 L" << endl;
}*/
#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>

#define TIMELIMIT 0.9
using namespace std;

// Directions and their coordinate changes
const pair<int, int> directions[4] = {
    {0, -1},  // U
    {1, 0},   // R
    {0, 1},   // D
    {-1, 0}   // L
};

const char arrow_turn[4] = {'U', 'R', 'D', 'L'};

vector<vector<char>> board;
int robot_count = 0;
vector<tuple<int, int, char>> robots;

void parse_input() {
    board.clear();
    for (int i = 0; i < 10; ++i) {
        string line;
        getline(cin, line);
        board.push_back(vector<char>(line.begin(), line.end()));
    }

    cin >> robot_count;
    robots.clear();
    for (int i = 0; i < robot_count; ++i) {
        int x, y;
        char direction;
        cin >> x >> y >> direction;
        robots.emplace_back(x, y, direction);
    }
}

pair<int, int> wrap_position(int x, int y) {
    return {x >= 19 ? x - 19 : x, y >= 10 ? y - 10 : y };
}

void print_board() {
    for (int y = 0; y < 10; ++y) {
        for (int x = 0; x < 19; ++x) {
            cerr << board[y][x] << " ";
        }
        cerr << endl;
    }
}

int simulate_robot(int x, int y, char direction) {
    set<tuple<int, int, char>> visited;
    int moves = 0;

    while (true) {
        if (find(begin(arrow_turn), end(arrow_turn), board[y][x]) != end(arrow_turn)) {
            direction = board[y][x];
        }

        // New position based on direction
        int dx = directions[direction == 'U' ? 0 : direction == 'R' ? 1 : direction == 'D' ? 2 : 3].first;
        int dy = directions[direction == 'U' ? 0 : direction == 'R' ? 1 : direction == 'D' ? 2 : 3].second;
        tie(x, y) = wrap_position(x + dx, y + dy);

        if (board[y][x] == '#' || visited.count({x, y, direction})) {
            break;
        }

        visited.insert({x, y, direction});
        moves++;
    }

    return moves;
}

int evaluate_board() {
    int total_moves = 0;
    for (const auto& [x, y, direction] : robots) {
        total_moves += simulate_robot(x, y, direction);
    }
    return total_moves;
}

pair<vector<vector<char>>, vector<tuple<int, int, char>>> randomize_arrows() {
    vector<vector<char>> new_board = board;
    vector<tuple<int, int, char>> arrow_positions;

    for (int y = 0; y < 10; y++) {
        for (int x = 0; x < 19; x++) {
            if (board[y][x] == '.') {
                char direction = "URDL."[rand() % 5];
                if (direction != '.') {
                    int dx = directions[direction == 'U' ? 0 : direction == 'R' ? 1 : direction == 'D' ? 2 : 3].first;
                    int dy = directions[direction == 'U' ? 0 : direction == 'R' ? 1 : direction == 'D' ? 2 : 3].second;
                    auto [tx, ty] = wrap_position(x + dx, y + dy);
                    print_board();
                    if (new_board[ty][tx] == '#') {
                        continue;
                    }
                    new_board[y][x] = direction;
                    arrow_positions.emplace_back(x, y, direction);
                }
            }
        }
    }
    return {new_board, arrow_positions};
}

int main() {
    srand(static_cast<unsigned int>(time(0)));
    parse_input();

    vector<vector<char>> best_board;
    int best_score = 0;
    vector<tuple<int, int, char>> best_arrows;

    clock_t start_time = clock();
    double time_limit = TIMELIMIT;
    while (static_cast<double>(clock() - start_time) / CLOCKS_PER_SEC < time_limit) {
        auto [randomized_board, arrow_positions] = randomize_arrows();
        int score = evaluate_board();

        // Save the best score
        if (score > best_score) {
            best_score = score;
            best_board = randomized_board;
            best_arrows = arrow_positions;
        }
    }

    // Output result in format X Y DIR
    for (const auto& [x, y, direction] : best_arrows) {
        cout << x << " " << y << " " << direction << " ";
    }
    cout << endl;

    return 0;
}
