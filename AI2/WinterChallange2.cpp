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

    return {bst_first_mov,{getDirectionName(last_move),{ min_distance, {selected_organ_id, closest_protein}}}};
}

//LOGIC================================================================WIP
bool basic_logic( ProteinStock& my_stock, unordered_map<int, Organ>& my_organs,
                     unordered_map<int, Organ>& opponent_organs_map, vector<vector<string>>& grid, 
                     string& last_command, int selected_organ_id, pair<int, int> closest_protein, pair<int, int> bst_first_mov){
    bool built_basic = false;
    if (selected_organ_id != -1 && closest_protein.first != -1 && closest_protein.second != -1 
    && used_protein_sources.find({closest_protein}) == used_protein_sources.end()) {// Prioretise claming proteins
        last_command = "GROW " + to_string(selected_organ_id) + " " + to_string(closest_protein.first) + " " + to_string(closest_protein.second) + " BASIC";
        cout << "GROW " << selected_organ_id << " " << bst_first_mov.first << " " << bst_first_mov.second << " BASIC" << endl;
    } else if(my_stock.a > 0){ // If we can't reach any protein take as much space as we can
        for(int i = 0; i < my_organs.size(); i++){
            Organ candidate = my_organs[i];
            string dir = next_to_protein(candidate.i, candidate.j, grid, "EMPTY");
            pair <int, int> coords = getDirection(dir);
            if(dir != "" && used_protein_sources.find({candidate.i + coords.first, candidate.j + coords.second}) == used_protein_sources.end()){
                last_command = "GROW " + to_string(candidate.organ_id) + " " + to_string(candidate.i + coords.first) + " " + to_string(candidate.j + coords.second) + " BASIC";
                cout << "GROW " << candidate.organ_id << " " << candidate.i + coords.first << " " << candidate.j + coords.second << " BASIC" << endl;
                break;
            }
        }
    }
    return built_basic;
}

bool hasLineOfSight (const pair<int, int>& start, const pair<int, int>& end, const vector<vector<string>>& grid) {
    int dx = end.first - start.first;
    int dy = end.second - start.second;
    int steps = max(abs(dx), abs(dy));
    if (steps == 0) {
        return true;
    }
    dx /= steps;
    dy /= steps;
    int x = start.first;
    int y = start.second;
    for (int i = 0; i < steps; i++) {
        x += dx;
        y += dy;
        if (grid[y][x] != "EMPTY" && grid[y][x] != "A" && grid[y][x] != "B" && grid[y][x] != "C" && grid[y][x] != "D") {
            return false;
        }
    }
    return true;
}

//Function given ideal root position returns sporer position that is connected to my organ
pair<int, pair <int, int>> SporerPlacement(const pair<int, int>& root_position, const unordered_map<int, Organ>& my_organs, const vector<vector<string>>& grid) {
    pair<int, int> sporer_position = {-1, -1};
    int parent_id = -1;
    for (const auto& [organ_id, organ] : my_organs) {
        if (hasLineOfSight(root_position, {organ.i, organ.j}, grid)) {
            for (const auto& [dx, dy] : directions) {
                int nx = organ.i + dx;
                int ny = organ.j + dy;
                if (is_valid(nx, ny, grid[0].size(), grid.size(), grid) && grid[ny][nx] == "EMPTY" && hasLineOfSight({nx, ny}, root_position, grid)) {
                    sporer_position = {nx, ny};
                    parent_id = organ_id;
                    break;
                }
            }
        }
    }
    return {parent_id, sporer_position};  
}

//Function to find 
pair<pair<int, int>, pair<int, int>> bestRootPlacement(const vector<pair<int, int>>& protein_sources, 
                                 const vector<vector<string>>& grid, 
                                 int width, int height, 
                                 const unordered_map<int, Organ>& my_organs,
                                 const unordered_map<int, Organ>& opponent_organs_map){
    pair<int, int> best_position = {-1, -1};
    pair<int, int> best_sporer_position = {-1, -1};
    int best_value = INT32_MIN;

    pair<int, int> directions[5] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}, {0, 0}};
    string sporer_orientation = "N";
    string best_sporer_orientation = "N";
    int best_sporer_parent_id = -1; // ID organu, który będzie rodzicem sporera

    for (const auto& protein : protein_sources) {
        for (const auto& [dx, dy] : directions) {
            int nx = protein.first + dx;
            int ny = protein.second + dy;

            if (!is_valid(nx, ny, width, height, grid)) {
                continue;
            }

            for (const auto& [dx2, dy2] : directions) {
                int nx2 = nx + dx2;
                int ny2 = ny + dy2;

                if (is_valid(nx2, ny2, width, height, grid)) {
                    int value = 0;
                    pair<int, int> sporer_position = {-1, -1};
                    int sporer_parent_id = -1;

                    if (grid[ny2][nx2] == "EMPTY") {
                        for (const auto& [organ_id, organ] : my_organs) {
                            int dist = abs(organ.i - nx2) + abs(organ.j - ny2);
                            value += 30 - dist;
                            if (hasLineOfSight({nx2, ny2}, {organ.i, organ.j}, grid)) {
                                value += 50;

                                // Sporer musi być obok organu i na linii widzenia
                                for (const auto& [dx3, dy3] : directions) {
                                    int nx3 = organ.i + dx3;
                                    int ny3 = organ.j + dy3;

                                    if (is_valid(nx3, ny3, width, height, grid) && grid[ny3][nx3] == "EMPTY" && 
                                        hasLineOfSight({nx2, ny2}, {nx3, ny3}, grid)) {
                                        sporer_position = {nx3, ny3};
                                        sporer_parent_id = organ_id;
                                        
                                        // Ustal orientację sporera względem pola
                                        if (nx3 == nx2) {
                                            sporer_orientation = (ny3 > ny2) ? "N" : "S";
                                        } else {
                                            sporer_orientation = (nx3 > nx2) ? "W" : "E";
                                        }

                                        // Przerwij pętlę po znalezieniu odpowiedniego miejsca
                                        break;
                                    }
                                }
                            } else {
                                value -= 100;
                            }
                        }

                        // Dodaj wartość w zależności od odległości od proteiny
                        int dist_to_prot = abs(nx2 - protein.first) + abs(ny2 - protein.second);
                        if (dist_to_prot == 2) {
                            value += 100;
                        } else if (dist_to_prot == 1) {
                            value += 50;
                        }

                        // Dodaj wartość w zależności od odległości od organów przeciwnika
                        /*int dist_to_opponent = INT32_MAX;
                        for (const auto& [organ_id, organ] : opponent_organs_map) {
                            int dist = abs(organ.i - nx2) + abs(organ.j - ny2);
                            dist_to_opponent = min(dist_to_opponent, dist);
                        }
                        value += 30 - dist_to_opponent;*/

                        if (value > best_value) {
                            best_value = value;
                            best_position = {nx2, ny2};

                            pair<int, pair<int, int>> sporerpl = SporerPlacement(best_position, my_organs, grid);
                            best_sporer_position = sporerpl.second;
                            best_sporer_parent_id = sporerpl.first;
                            best_sporer_orientation = sporer_orientation;
                        }
                    }
                }
            }
        }
    }

    if (DEBUG) {
        cerr << "Best root placement: " << best_position.first << " " << best_position.second << endl;
        cerr << "Best sporer placement: " << best_sporer_position.first << " " << best_sporer_position.second << endl;
    }

    if (best_position.first != -1) { // Buduj sporer do późniejszego budowania root
        cout << "GROW " << best_sporer_parent_id << " " << best_sporer_position.first << " " << best_sporer_position.second 
             << " SPORER " << best_sporer_orientation << endl;
    }

    return {best_sporer_position, best_position};
}


// ====================================================================
int main() {
    int width, height;
    cin >> width >> height; cin.ignore();

    vector<vector<string>> grid(height, vector<string>(width, "EMPTY"));
    string last_command = "WAIT";
    pair<int, int> new_root_position = {-1, -1};
    pair<int, int> sporer_coords = {-1, -1};
    pair<bool, int> builtSporer = {false, -1};

    // Game loop
    while (1) {
        int organ_count;
        cin >> organ_count; cin.ignore();

        vector<Organ> entities = {};
        unordered_map<int, Organ> my_organs = {};
        unordered_map<int, Organ> opponent_organs_map = {};
        vector<pair<int, int>> protein_sources = {};

        for (auto& row : grid) {
            fill(row.begin(), row.end(), "EMPTY");
        }

        for (int i = 0; i < organ_count; i++) {
            Organ organ;
            cin >> organ.i >> organ.j >> organ.type >> organ.owner >> organ.organ_id >> organ.organ_dir >> organ.organ_parent_id >> organ.organ_root_id; cin.ignore();

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


        for (int i = 0; i < required_actions_count; i++) {
            //Building tentacle logic part
            
            bool unique_organ_built = false;

            //enemy organs that are one empty space away from our organs
            unordered_map<int, Organ> reachable_opponent_organs;
            //unordered_map<int, Organ> origin_organs;
            
            int nx;
            int ny;
            for (const auto& [organ_id, organ] : my_organs) {
                for (const auto& [dx, dy] : directions) {
                    int nx = organ.i + dx;
                    int ny = organ.j + dy;

                    for (const auto& [dx2, dy2] : directions) {
                        int nx2 = nx + dx2;
                        int ny2 = ny + dy2;

                        if (is_valid(nx, ny, width, height, grid) && grid[ny2][nx2] == "OPPONENT") {
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
            cerr << "Should i build a root: " << my_stock.a << " " << my_stock.b << " " << my_stock.c << " " << my_stock.d << " " << !unique_organ_built << endl;
            //Logika budowania roota
            if (my_stock.a > 0 && my_stock.b > 0 && my_stock.c > 0 && my_stock.d > 0 &&!unique_organ_built 
                && new_root_position.first != -1) {
                for(const auto& [organ_id, organ] : my_organs){
                    if(organ.type == "SPORER" && organ.i == sporer_coords.first && organ.j == sporer_coords.second){
                        cout << "SPORE " << organ_id << " " << new_root_position.first << " " << new_root_position.second << endl;
                        grid[organ.j][organ.i] = "ROOT"; // Mark the position as occupied
                        unique_organ_built = true;
                    }
                }
                grid[new_root_position.second][new_root_position.first] = "ROOT"; // Mark the position as occupied
                unique_organ_built = true;
            }
            cerr << "Should I build sporer: " << my_stock.a << " " << my_stock.d << " " << !unique_organ_built << endl;
            
            // Logika budowania sporerów - organ służący do wystrzelenia nowego roota potrzebuje kooordynatów i kierunku (może stworzyć nowy root na dowolnym wolnym polu które ma w postej linii z sobą w kierunku kierunku kosztem a = 1, b = 1,c = 1,d = 1)  
            if (my_stock.a > 0 && my_stock.d > 0 && !unique_organ_built && builtSporer.first == false) {
                
                // Znajdź najlepsze miejsce na zbudowanie roota
                pair<pair<int, int>, pair<int, int>> result = bestRootPlacement(protein_sources, grid, width, height, my_organs, opponent_organs_map);
                new_root_position = result.second;
                sporer_coords = result.first;
                if (new_root_position.first != -1) {
                    builtSporer = {true, i};
                }
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
                            last_command = "GROW " + to_string(selected_organ_id) + " " + to_string(nx) + " " + to_string(ny) + " HARVESTER " + direction;
                            cout << "GROW " << selected_organ_id << " " << nx << " " << ny << " HARVESTER " << harvesterOrientation(closest_protein,{nx, ny}) << endl;
                            grid[ny][nx] = "HARVESTER"; // Mark the position as occupied
                            grid[closest_protein.second][closest_protein.first] = "PROTEIN_USED"; // Mark the protein source as exclusively used
                            unique_organ_built = true;
                            break;
                        }
                    }
                //}
            }

            cerr<< "Should I build tentacle: " << my_stock.b << " " << my_stock.c << " " << !unique_organ_built << " " << !reachable_opponent_organs.empty() << endl;
            // Logika budowania macek
            if (my_stock.b > 0 && my_stock.c > 0 && !unique_organ_built && !reachable_opponent_organs.empty()) {
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
                        if (is_valid(nx, ny, width, height, grid) && grid[ny][nx] == "EMPTY") {
                            for (const auto& [dx2, dy2] : directions) {
                                int nx2 = nx + dx2;
                                int ny2 = ny + dy2;

                                if (nx2 == best_target.i && ny2 == best_target.j) {
                                    // Znaleziono pole, które sąsiaduje z moim organem i wrogim organem
                                    string tentacle_orientation = harvesterOrientation({best_target.i, best_target.j}, {nx, ny});
                                    last_command = "GROW " + to_string(organ_id) + " " + to_string(nx) + " " + to_string(ny) + " TENTACLE " + tentacle_orientation;
                                    cout << "GROW " << organ_id << " " << nx << " " << ny << " TENTACLE " << tentacle_orientation << endl;
                                    grid[ny][nx] = "TENTACLE"; // Oznacz pole jako zajęte
                                    unique_organ_built = true;
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

            if (!unique_organ_built) {
                if (selected_organ_id != -1 && closest_protein.first != -1 && closest_protein.second != -1 
                && used_protein_sources.find({closest_protein}) == used_protein_sources.end()) {// Prioretise claming proteins
                    last_command = "GROW " + to_string(selected_organ_id) + " " + to_string(closest_protein.first) + " " + to_string(closest_protein.second) + " BASIC";
                    cout << "GROW " << selected_organ_id << " " << bst_first_mov.first << " " << bst_first_mov.second << " BASIC" << endl;
                } else if(my_stock.a > 0){ // If we can't reach any protein take as much space as we can
                    for(int i = 0; i < my_organs.size(); i++){
                        Organ candidate = my_organs[i];
                        string dir = next_to_protein(candidate.i, candidate.j, grid, "EMPTY");
                        pair <int, int> coords = getDirection(dir);
                        if(dir != "" && used_protein_sources.find({candidate.i + coords.first, candidate.j + coords.second}) == used_protein_sources.end()){
                            last_command = "GROW " + to_string(candidate.organ_id) + " " + to_string(candidate.i + coords.first) + " " + to_string(candidate.j + coords.second) + " BASIC";
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
