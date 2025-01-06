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
int width, height;

// Structure to represent an organ on the grid
struct Organ {
    int i, j;
    string type;
    int owner;
    int organ_id;
    string organ_dir;
    int organ_parent_id;
    int organ_root_id;

    int subtree_size = -1; // Liczba organów w poddrzewie (domyślnie -1, aby wskazać, że nie jest jeszcze obliczone)
};

// Structure to represent protein stocks
struct ProteinStock {
    int a, b, c, d;
};

//table with costs of basic organ, harvester, tentacle, sporer and root
const ProteinStock organcost [5] = {{1, 0, 0, 0}, {0, 0, 1, 1}, {0, 1, 1, 0}, {0, 1, 0, 1},{1, 1, 1, 1}};

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

//Function to wrap target coordinates to the grid in one-dimensional array
int wrapGrid(Organ target){
    return target.i * width + target.j;
}

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
bool is_valid(int x, int y, const vector<vector<string>>& grid) {
    return x >= 0 && x < width && y >= 0 && y < height && (grid[y][x] == "EMPTY" || grid[y][x] == "A" ||
                                                            grid[y][x] == "B" || grid[y][x] == "C" || grid[y][x] == "D");
}

int how_many(const unordered_map<int, Organ>& organs, Organ& target) {
    // Jeśli już obliczono liczbę organów w poddrzewie, zwróć zapisaną wartość
    if (target.subtree_size != -1) {
        return target.subtree_size;
    }

    int count = 0;

    // Iteruj przez wszystkie organy, aby znaleźć dzieci danego organu
    for (const auto& [organ_id, organ] : organs) {
        if (organ.organ_parent_id == target.organ_id) {
            count += 1 + how_many(organs, const_cast<Organ&>(organ)); // Rekurencja na dzieciach
        }
    }

    target.subtree_size = count; // Zapisz wynik w celu optymalizacji
    return count;
}


//Checks every enemy organ that we have access to and picks one that is father to most enemies
Organ best_enemy_target(const unordered_map<int, Organ>& opponent_organs) {
    Organ result;
    int max_subtree_size = INT32_MIN;
    /*unordered_map<int, Organ> opponent_organs;
    for (const auto& [organ_id, pair] : opponent_organs_and_my_organ) {
        opponent_organs[organ_id] = pair.second;
    }*/

    for (const auto& [organ_id, organ] : opponent_organs) {
        int subtree_size = how_many(opponent_organs, const_cast<Organ&>(organ));
        if (subtree_size > max_subtree_size) {
            max_subtree_size = subtree_size;
            result = organ;
        }
    }

    cerr << "Best enemy target ID: " << result.organ_id << " with subtree size: " << max_subtree_size << endl;
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
pair<pair<int, int>,pair<string, pair<int, pair<int, pair<int, int>>>>> find_nearest_protein(const unordered_map<int, Organ>& organs, const vector<pair<int, int>>& protein_sources, const vector<vector<string>>& grid) {
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

                    if (is_valid(nx, ny, grid) && visited.find({nx, ny}) == visited.end()) {
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

//TODO
bool sporer_logic(){
    bool sporer_built = false;
    return sporer_built;
}

//TODO
bool tentacle_logic(ProteinStock& my_stock, unordered_map<int, Organ>& my_organs, unordered_map<int, Organ>& opponent_organs_map, vector<vector<string>>& grid, string& last_command,
const unordered_map<int, Organ>& reachable_opponent_organs){
    
    bool tentacle_built = false;

    if (my_stock.b > 0 && my_stock.c > 0 && !reachable_opponent_organs.empty()) {
        cerr <<"Candidate organs for tentacle: " << reachable_opponent_organs.size() << endl;
        Organ best_target = best_enemy_target(reachable_opponent_organs); // Znajdź najlepszy cel wroga
        cerr << "Yes, I should build tentacle attacking : " << best_target.organ_id << " at "<< best_target.i << " " << best_target.j << endl;

        bool tentacle_built = false;

        // Przejdź przez moje organy
        for (const auto& [organ_id, organ] : my_organs) {
            for (const auto& [dx, dy] : directions) {
                int nx = organ.i + dx;
                int ny = organ.j + dy;
                //cerr<< "Checking from organ: " << organ_id << " at: " << nx << " " << ny << endl;//Works
                // Sprawdź, czy pole sąsiaduje z wrogim organem
                if (is_valid(nx, ny, grid) && grid[ny][nx] == "EMPTY") {
                    for (const auto& [dx2, dy2] : directions) {
                        int nx2 = nx + dx2;
                        int ny2 = ny + dy2;

                        if (nx2 == best_target.i && ny2 == best_target.j) {
                            // Znaleziono pole, które sąsiaduje z moim organem i wrogim organem
                            string tentacle_orientation = harvesterOrientation({best_target.i, best_target.j}, {nx, ny});
                            last_command = "GROW " + to_string(organ_id) + " " + to_string(nx) + " " + to_string(ny) + " TENTACLE " + tentacle_orientation;
                            cout << "GROW " << organ_id << " " << nx << " " << ny << " TENTACLE " << tentacle_orientation << endl;
                            grid[ny][nx] = "TENTACLE"; // Oznacz pole jako zajęte
                            //unique_organ_built = true;
                            tentacle_built = true;
                            break;
                        }
                    }
                }
                if (tentacle_built) break;
            }
            if (tentacle_built) break;
        }
    }
    return tentacle_built;
}

//TODO
bool harvester_logic( ProteinStock& my_stock, unordered_map<int, Organ>& my_organs,
                     unordered_map<int, Organ>& opponent_organs_map, vector<vector<string>>& grid, 
                     string& last_command, int selected_organ_id, pair<int, int> closest_protein, string direction, int distance, pair<int, int> bst_first_mov){

    bool harvester_built = false;
    if (my_stock.c > 0 && my_stock.d > 0 && selected_organ_id != -1 && closest_protein.first != -1 ){//&& !unique_organ_built) {
    //for (size_t dir_index = 0; dir_index < directions.size(); ++dir_index) {
        //int dx = directions[dir_index].first;
        //int dy = directions[dir_index].second;
        int nx = bst_first_mov.first;// + dx;
        int ny = bst_first_mov.second;// + dy;
        cerr << "trying to build harvester at: " << nx << " " << ny << endl;
        if (is_valid(nx, ny, grid) && grid[ny][nx] == "EMPTY" && distance == 2) {
            
            if (direction != "U") {
                used_protein_sources.insert({closest_protein.first, closest_protein.second});
                last_command = "GROW " + to_string(selected_organ_id) + " " + to_string(nx) + " " + to_string(ny) + " HARVESTER " + direction;
                cout << "GROW " << selected_organ_id << " " << nx << " " << ny << " HARVESTER " << harvesterOrientation(closest_protein,{nx, ny}) << endl;
                grid[ny][nx] = "HARVESTER"; // Mark the position as occupied
                grid[closest_protein.second][closest_protein.first] = "PROTEIN_USED"; // Mark the protein source as exclusively used
                //unique_organ_built = true;
                //break;
            }
        }
    //}
    }
    return harvester_built;
}

//TODO
bool basic_logic( ProteinStock& my_stock, unordered_map<int, Organ>& my_organs,
                     unordered_map<int, Organ>& opponent_organs_map, vector<vector<string>>& grid, 
                     string& last_command, int selected_organ_id, pair<int, int> closest_protein, pair<int, int> bst_first_mov){
    bool basic_built = false;
    if (selected_organ_id != -1 && closest_protein.first != -1 && closest_protein.second != -1 
    && used_protein_sources.find({closest_protein}) == used_protein_sources.end()) {// Prioretise claming proteins
        last_command = "GROW " + to_string(selected_organ_id) + " " + to_string(closest_protein.first) + " " + to_string(closest_protein.second) + " BASIC";
        cout << "GROW " << selected_organ_id << " " << bst_first_mov.first << " " << bst_first_mov.second << " BASIC" << endl;
        basic_built = true;
    } else if(my_stock.a > 0){ // If we can't reach any protein take as much space as we can
        for(int i = 0; i < my_organs.size(); i++){
            Organ candidate = my_organs[i];
            string dir = next_to_protein(candidate.i, candidate.j, grid, "EMPTY");
            pair <int, int> coords = getDirection(dir);
            if(dir != "" && used_protein_sources.find({candidate.i + coords.first, candidate.j + coords.second}) == used_protein_sources.end()){
                last_command = "GROW " + to_string(candidate.organ_id) + " " + to_string(candidate.i + coords.first) + " " + to_string(candidate.j + coords.second) + " BASIC";
                cout << "GROW " << candidate.organ_id << " " << candidate.i + coords.first << " " << candidate.j + coords.second << " BASIC" << endl;
                basic_built = true;
                break;
            }
        }
    }
    return basic_built;
}

int main() {
    cin >> width >> height; cin.ignore();
    if (DEBUG){
        cerr << "Width: " << width << " Height: " << height << endl;
    }
    vector<vector<string>> grid(height, vector<string>(width, "EMPTY"));
    string last_command = "WAIT";
    // Game loop
    while (1) {
        int organ_count;
        cin >> organ_count; cin.ignore();
        if (DEBUG){
            cerr << "Organ count: " << organ_count << endl;
        }
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

            if(DEBUG){
                cerr << "Organ: " << organ.i << " " << organ.j << " " << organ.type << " " << organ.owner << " " << organ.organ_id << " " << organ.organ_dir << " " << organ.organ_parent_id << " " << organ.organ_root_id << endl;
            }
            //organ = {organ.i, organ.j, organ.type, organ.owner, organ.organ_id, organ.organ_dir, organ.organ_parent_id, organ.organ_root_id};
            entities.push_back(organ);

            if (organ.type == "ROOT" || organ.type == "BASIC" || organ.type == "HARVESTER" || organ.type == "TENTACLE" || organ.type == "SPORER") {
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
        if (DEBUG){
            cerr << "My stock: " << my_stock.a << " " << my_stock.b << " " << my_stock.c << " " << my_stock.d << endl;
        }
        cin >> opp_stock.a >> opp_stock.b >> opp_stock.c >> opp_stock.d; cin.ignore();
        if (DEBUG){
            cerr << "Opponent stock: " << opp_stock.a << " " << opp_stock.b << " " << opp_stock.c << " " << opp_stock.d << endl;
        }
        int required_actions_count;
        cin >> required_actions_count; cin.ignore();
        if (DEBUG){
            cerr << "Required actions: " << required_actions_count << endl;
        }
        // Find the nearest protein source for my organs
        auto nearest_protein = find_nearest_protein(my_organs, protein_sources, grid);
        cerr<<"found nearest protein\n";
        pair<int, int> bst_first_mov = nearest_protein.first;
        string direction = nearest_protein.second.first;
        int distance = nearest_protein.second.second.first;
        int selected_organ_id = nearest_protein.second.second.second.first;
        pair<int, int> closest_protein = nearest_protein.second.second.second.second;


        for (int i = 0; i < required_actions_count; i++) {
            //Building tentacle logic part
            
            bool unique_organ_built = false;

            //enemy organs that are one empty space away from our organs
            unordered_map<int, Organ> reachable_opponent_organs;
            
            int nx;
            int ny;
            for (const auto& [organ_id, organ] : my_organs) {
                for (const auto& [dx, dy] : directions) {
                    int nx = organ.i + dx;
                    int ny = organ.j + dy;

                    for (const auto& [dx2, dy2] : directions) {
                        int nx2 = nx + dx2;
                        int ny2 = ny + dy2;

                        if (is_valid(nx, ny, grid) && grid[ny2][nx2] == "OPPONENT") {
                            for(const auto& [opponent_organ_id, opponent_organ] : opponent_organs_map){
                                if(opponent_organ.i == nx2 && opponent_organ.j == ny2){//Dodaj jakiś sposób na zapamiętanie skąd możemy w dwóch krokach dojść do wroga i jaką trasą
                                    reachable_opponent_organs[organ_id] = opponent_organ;
                                    //origin_organs[organ_id] = organ;
                                    cerr<< "found reachable opponent organ: " << opponent_organ.organ_id << endl;
                                }
                            }
                        }
                    }
                }
            }

            cerr<< "Should I build tentacle: " << my_stock.b << " " << my_stock.c << " " << !unique_organ_built << " " << !reachable_opponent_organs.empty() << endl;
            // Logika budowania macek
            unique_organ_built &= tentacle_logic(my_stock, my_organs, opponent_organs_map, grid, last_command, reachable_opponent_organs);

            cerr << "should i build harvester: " << my_stock.c << " " << my_stock.d << " " << selected_organ_id << " " << closest_protein.first << " " << closest_protein.second << " " << unique_organ_built << endl;
            
            //Building harvester logic part
            unique_organ_built &= harvester_logic(my_stock, my_organs, opponent_organs_map, grid, last_command, selected_organ_id, closest_protein, direction, distance, bst_first_mov);

            //in a seperate function
            unique_organ_built &= basic_logic(my_stock, my_organs, opponent_organs_map, grid, last_command, selected_organ_id, closest_protein, bst_first_mov);

            if (!unique_organ_built) {// if everything else fails wait
                cout << "WAIT" << endl;
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