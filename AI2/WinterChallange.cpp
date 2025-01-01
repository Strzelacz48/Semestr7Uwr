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

std::pair<int, int> operator-(const std::pair<int, int>& p1, const std::pair<int, int>& p2) {
    return {p1.first - p2.first, p1.second - p2.second};
}

//TODO przepisać grid na bycie pamiętaną w jednowymiarowej talbicy o rozmairze y*x

// Function to print the current state of the grid
void printGrid(const vector<vector<string>>& grid) {
    cerr << "Current grid state:" << endl;
    for (const auto& row : grid) {
        for (const auto& cell : row) {
            cerr << cell << "\t"; // Tab separator for better readability
        }
        cerr << endl;
    }
    cerr << "----------------------------------------" << endl;
}

// Structure to represent an organ on the grid
struct Organ {//TODO dodać relację rodzica do dzieci albo przynajmniej liczbę dzieci
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

// Directions for BFS traversal and their corresponding names
const vector<pair<int, int>> directions = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
const vector<string> direction_names = {"N", "S", "W", "E"};

//set for remembering used proteins
set<pair<int, int>> used_protein_sources;

string getDirectionName(const pair<int, int>& direction) {
    for (size_t i = 0; i < directions.size(); ++i) {
        if (directions[i] == direction) {
            return direction_names[i];
        }
    }
    return "U"; // Zwraca "Unknown", jeśli nie znaleziono kierunku
}

string harvesterOrientation(pair<int, int> protein, pair<int, int> harvester){
    cerr << "protein : ("<< protein.first <<", "<<protein.second<<")\n";
    cerr << "harvester : ("<< harvester.first <<", "<<harvester.second<<")\n";
    pair<int, int> result = protein - harvester;
    cerr << "result : ("<< result.first <<", "<<result.second<<")\n";
    for(int i = 0; i < directions.size(); i++){
        if(directions[i] == result){
            cerr << "direction_names[i] : "<< direction_names[i] << endl;
            return direction_names[i];
        }
    }
}

pair<int, int> getDirection(string directionName) {
    for (size_t i = 0; i < direction_names.size(); i++) {
        if (direction_names[i] == directionName) {
            return directions[i];
        }
    }
    return {-1, -1}; // Zwraca "Unknown", jeśli nie znaleziono kierunku
}

// Function to check if a position is within the grid and valid
bool is_valid(int x, int y, int width, int height, const vector<vector<string>>& grid) {
    return x >= 0 && x < width && y >= 0 && y < height && (grid[y][x] == "EMPTY" || grid[y][x] == "A");
}

int how_many(Organ target){
    return 
}

//Checks every enemy organ that we have access to and picks one that is father to most enemies
Organ best_enemy_target(){
    Organ result;
    return result;
}

// Function to check if a given position is next to a protein source
// Returns the direction ("N", "S", "W", "E") if there is a neighboring protein of the given type
string next_to_protein(int x, int y, const vector<vector<string>>& grid, string protein_type = "A") {
    for (int dir_index = 0; dir_index < directions.size(); ++dir_index) {
        int nx = x + directions[dir_index].first;
        int ny = y + directions[dir_index].second;
        if (nx >= 0 && ny >= 0 && ny < grid.size() && nx < grid[0].size()) {
            if (grid[ny][nx] == protein_type) {
                return direction_names[dir_index];
            }
        }
    }
    return ""; // No neighboring protein found
}

// Function to find the nearest protein source using BFS
pair<pair<int, int>,pair<string, pair<int, pair<int, pair<int, int>>>>> find_nearest_protein(const unordered_map<int, Organ>& organs, const vector<pair<int, int>>& protein_sources, int width, int height, const vector<vector<string>>& grid) {
    int min_distance = INT32_MAX;
    int selected_organ_id = -1;
    pair<int, int> closest_protein = {-1, -1};
    pair<int, int> last_move = {-1, -1};
    pair<int, int> bst_first_mov = {-1, -1};

    for (const auto& [organ_id, organ] : organs) {
        queue<pair<pair<int, int>,pair<int, pair<int, int>>>> q;
        set<pair<int, int>> visited;
        q.push({{-1, -1},{0, {organ.i, organ.j}}});
        visited.insert({organ.i, organ.j});

        //int distance = 0;
        bool found = false;

        while (!q.empty() && !found) {
            int size = q.size();
            pair<int, int> first_mov;
            for (int i = 0; i < size; i++) {
                pair<pair<int, int>, pair<int, pair<int, int>>> pos = q.front();
                auto [cx, cy] = pos.second.second;
                int dist_so_far = pos.second.first;
                pair<int, int> frst_mov = pos.first;
                q.pop();

                for (const auto& [dx, dy] : directions) {
                    int nx = cx + dx;
                    int ny = cy + dy;

                    if (is_valid(nx, ny, width, height, grid) && visited.find({nx, ny}) == visited.end()) {
                        visited.insert({nx, ny});
                        if (dist_so_far == 0){
                            pair<int, int> first_mov = {nx, ny};
                            q.push({first_mov,{dist_so_far +1, {nx, ny}}});
                            frst_mov = first_mov;
                        }
                        else{
                            q.push({frst_mov,{dist_so_far +1, {nx, ny}}});
                        }
                        
                        // Check if this position is a protein source
                        if (grid[ny][nx] == "A") { // Prioritize protein A
                            if (dist_so_far + 1 < min_distance) {
                                min_distance = dist_so_far + 1;
                                selected_organ_id = organ_id;
                                closest_protein = {nx, ny};
                                bst_first_mov = frst_mov;
                                if(dy != 0){
                                    last_move = {-dx, -dy};
                                }
                                else{
                                    last_move = {dx, dy};
                                }
                                //cerr << "last_move : ("<< last_move.first <<", "<<last_move.second<<")\n";
                                //cerr << "last_move : ("<< last_move.first <<", "<<last_move.second<<")\n";
                                found = true;
                                break;
                            }
                        }
                    }
                }
                if (found) break;
            }
            //distance++;
        }
    }
    //TODO zwrócić jakoś pierwsze miejsce

    return {bst_first_mov,{getDirectionName(last_move),{ min_distance, {selected_organ_id, closest_protein}}}};
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

        ProteinStock my_stock;
        ProteinStock opp_stock;

        cin >> my_stock.a >> my_stock.b >> my_stock.c >> my_stock.d; cin.ignore();
        cin >> opp_stock.a >> opp_stock.b >> opp_stock.c >> opp_stock.d; cin.ignore();

        int required_actions_count;
        cin >> required_actions_count; cin.ignore();

        // Find the nearest protein source for my organs
        auto nearest_protein = find_nearest_protein(my_organs, protein_sources, width, height, grid);
        cerr<<"found nearest protein\n";
        pair<int, int> bst_first_mov = nearest_protein.first;
        string direction = nearest_protein.second.first;
        int distance = nearest_protein.second.second.first;
        int selected_organ_id = nearest_protein.second.second.second.first;
        pair<int, int> closest_protein = nearest_protein.second.second.second.second;

        bool unique_organ_built = false;

        for (int i = 0; i < required_actions_count; i++) {
            //Building tentacle logic part
            if(my_stock.b > 0 && my_stock.c > 0 && !unique_organ_built){
                
                cout << "GROW " << selected_organ_id << " " << nx << " " << ny << " TENTACLE " << best_enemy_target();//harvesterOrientation(closest_protein,{nx, ny}) << endl;
            }

            //Building harvester logic part
            cerr << "should i build harvester: " << my_stock.c << " " << my_stock.d << " " << selected_organ_id << " " << closest_protein.first << " " << closest_protein.second << " " << unique_organ_built << endl;
            if (my_stock.c > 0 && my_stock.d > 0 && selected_organ_id != -1 && closest_protein.first != -1 && !unique_organ_built) {
                //for (size_t dir_index = 0; dir_index < directions.size(); ++dir_index) {
                    //int dx = directions[dir_index].first;
                    //int dy = directions[dir_index].second;
                    int nx = bst_first_mov.first;// + dx;
                    int ny = bst_first_mov.second;// + dy;
                    cerr << "trying to build harvester at: " << nx << " " << ny << endl;
                    if (is_valid(nx, ny, width, height, grid) && grid[ny][nx] == "EMPTY" && distance == 2) {
                        
                        if (direction != "U") {
                            used_protein_sources.insert({closest_protein.first, closest_protein.second});
                            
                            cout << "GROW " << selected_organ_id << " " << nx << " " << ny << " HARVESTER " << harvesterOrientation(closest_protein,{nx, ny}) << endl;
                            grid[ny][nx] = "HARVESTER"; // Mark the position as occupied
                            grid[closest_protein.second][closest_protein.first] = "PROTEIN_USED"; // Mark the protein source as exclusively used
                            unique_organ_built = true;
                            break;
                        }
                    }
                //}
            }

            if (!unique_organ_built) {
                if (selected_organ_id != -1 && closest_protein.first != -1 && closest_protein.second != -1 
                && used_protein_sources.find({closest_protein}) == used_protein_sources.end()) {// Prioretise claming proteins
                    cout << "GROW " << selected_organ_id << " " << bst_first_mov.first << " " << bst_first_mov.second << " BASIC" << endl;
                } else if(my_stock.a > 0){ // If we can't reach any protein take as much space as we can
                    for(int i = 0; i < my_organs.size(); i++){
                        Organ candidate = my_organs[i];
                        string dir = next_to_protein(candidate.i, candidate.j, grid, "EMPTY");
                        pair <int, int> coords = getDirection(dir);
                        if(dir != "" && used_protein_sources.find({candidate.i + coords.first, candidate.j + coords.second}) == used_protein_sources.end()){
                            cout << "GROW " << candidate.organ_id << " " << candidate.i + coords.first << " " << candidate.j + coords.second << " BASIC" << endl;
                            break;
                        }
                    }
                }//TODO kiedy wszystkie inne pola są zajęte zbierz tyle protein ile potrzeba by zaklepać wolne miejsca (i pamiętaj że co turę będzie wciąż zbierać), a potem zbuduj na proteinie cokolwiek
                else{// if everything else fails wait
                    cout << "WAIT" << endl;
                }
            }
        }

        // Debug output for internal state
        if (DEBUG) {
            cerr << "My organs: " << my_organs.size() << endl;
            cerr << "Opponent organs: " << opponent_organs_map.size() << endl;
            cerr << "Protein sources: " << protein_sources.size() << endl;
            cerr << "Nearest protein to grow: Organ ID " << selected_organ_id << " at (" << closest_protein.first << ", " << closest_protein.second << ")" << endl;
            //printGrid(grid);
        }
    }
}
