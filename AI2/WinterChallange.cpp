#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <limits>
#include <queue>
#include <set>

using namespace std;

bool DEBUG = true;

// Structure to represent an organ on the grid
struct Organ {
    int i, j;
    string type;
    int owner;
    int organ_id;
    string organ_dir;
    int organ_parent_id;
    int organ_root_id;
};

// Structure to represent protein stocks
struct ProteinStock {
    int a, b, c, d;
};

// Directions for BFS traversal
const vector<pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

// Function to check if a position is within the grid and valid
bool is_valid(int x, int y, int width, int height, const vector<vector<string>>& grid) {
    return x >= 0 && x < width && y >= 0 && y < height && (grid[y][x] == "EMPTY" || grid[y][x] == "PROTEIN");
}

// Function to find the nearest protein source using BFS
pair<int, pair<int, int>> find_nearest_protein(const unordered_map<int, Organ>& organs, const vector<pair<int, int>>& protein_sources, int width, int height, const vector<vector<string>>& grid) {
    int min_distance = INT32_MAX;
    int selected_organ_id = -1;
    pair<int, int> closest_protein = {-1, -1};
    //TODO poprawić kod aby po znalezieniu nieskońcczonego źródła zasobów zajmował jak najwięcej pól
    for (const auto& [organ_id, organ] : organs) {
        queue<pair<int, int>> q;
        set<pair<int, int>> visited;
        q.push({organ.i, organ.j});
        visited.insert({organ.i, organ.j});

        int distance = 0;
        bool found = false;

        while (!q.empty() && !found) {
            int size = q.size();
            for (int i = 0; i < size; i++) {
                auto [cx, cy] = q.front();
                q.pop();

                for (const auto& [dx, dy] : directions) {
                    int nx = cx + dx;
                    int ny = cy + dy;

                    if (is_valid(nx, ny, width, height, grid) && visited.find({nx, ny}) == visited.end()) {
                        visited.insert({nx, ny});
                        q.push({nx, ny});

                        // Check if this position is a protein source
                        if (grid[ny][nx] == "PROTEIN") {
                            if (distance + 1 < min_distance) {
                                min_distance = distance + 1;
                                selected_organ_id = organ_id;
                                closest_protein = {nx, ny};
                                found = true;
                                break;
                            }
                        }
                    }
                }
                if (found) break;
            }
            distance++;
        }
    }

    return {selected_organ_id, closest_protein};
}

int main() {
    int width, height;
    cin >> width >> height; cin.ignore();

    vector<vector<string>> grid(height, vector<string>(width, "EMPTY"));

    // Game loop
    while (1) {
        int organ_count;
        cin >> organ_count; cin.ignore();

        vector<Organ> entities;
        unordered_map<int, Organ> my_organs;
        unordered_map<int, Organ> opponent_organs_map;
        vector<pair<int, int>> protein_sources;

        for (auto& row : grid) {
            fill(row.begin(), row.end(), "EMPTY");
        }

        for (int i = 0; i < organ_count; i++) {
            Organ organ;
            cin >> organ.i >> organ.j >> organ.type >> organ.owner >> organ.organ_id >> organ.organ_dir >> organ.organ_parent_id >> organ.organ_root_id; cin.ignore();

            entities.push_back(organ);

            if (organ.type == "ROOT" || organ.type == "BASIC" || organ.type == "HARVESTER") {
                if (organ.owner == 1) {
                    my_organs[organ.organ_id] = organ;
                    grid[organ.j][organ.i] = organ.type;
                } else if (organ.owner == 0) {
                    opponent_organs_map[organ.organ_id] = organ;
                    grid[organ.j][organ.i] = "OPPONENT";
                }
            } else if (organ.type == "A" || organ.type == "B" || organ.type == "C" || organ.type == "D") {
                protein_sources.push_back({organ.i, organ.j});
                grid[organ.j][organ.i] = organ.type;
            } else if (organ.type == "WALL") {
                grid[organ.j][organ.i] = organ.type;
            }
        }

        for (const auto& [organ_id, organ] : my_organs) {
            grid[organ.j][organ.i] = "MY_ORGAN";
        }

        ProteinStock my_stock;
        ProteinStock opp_stock;

        cin >> my_stock.a >> my_stock.b >> my_stock.c >> my_stock.d; cin.ignore();
        cin >> opp_stock.a >> opp_stock.b >> opp_stock.c >> opp_stock.d; cin.ignore();

        int required_actions_count;
        cin >> required_actions_count; cin.ignore();

        // Find the nearest protein source for my organs
        auto nearest_protein = find_nearest_protein(my_organs, protein_sources, width, height, grid);
        int selected_organ_id = nearest_protein.first;
        pair<int, int> closest_protein = nearest_protein.second;

        for (int i = 0; i < required_actions_count; i++) {
            bool harvester = false;

            if(my_stock.c > 0 && my_stock.d > 0){
                for (const auto& [dx, dy] : directions) {
                    int nx = my_organs[selected_organ_id].i + dx;
                    int ny = my_organs[selected_organ_id].j + dy;
                    if (grid[nx][ny] == "A"){
                        //TODO napisać logikę dla harvestera i upewnić się że nie zostanie nadpisany
                    }
                }
            }
            if (selected_organ_id != -1 && closest_protein.first != -1) {
                cout << "GROW " << selected_organ_id << " " << closest_protein.first << " " << closest_protein.second <<" BASIC\n";
            } else {
                cout << "WAIT" << endl;
            }
        }

        // Debug output for internal state
        if(DEBUG){
            cerr << "My organs: " << my_organs.size() << endl;
            cerr << "Opponent organs: " << opponent_organs_map.size() << endl;
            cerr << "Protein sources: " << protein_sources.size() << endl;
            cerr << "Nearest protein to grow: Organ ID " << selected_organ_id << " at (" << closest_protein.first << ", " << closest_protein.second << ")" << endl;
        }
    }
}
