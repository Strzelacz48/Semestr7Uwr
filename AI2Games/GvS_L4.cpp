#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <limits.h>

#pragma GCC optimize("Ofast,inline,tracer")
#pragma GCC optimize("unroll-loops,vpt,split-loops,unswitch-loops")

#define TURN_LIMIT 200
#define GIFT_LIMIT 30
#define MAX_X 1920
#define MAX_Y 980
#define HIGHT_LIMIT 750
#define TIME_LIMIT 50 // in ms first turn : 1000
#define REWARD 5

using namespace std;

int turn_counter;

// Struktura do przechowywania danych o gargulcach
struct Gargoyle {
    int x, y;
    int cooldown;
};

// Struktura do przechowywania danych o spadających prezentach
struct Present {
    int id;
    int value;
    int x, y;
    int vy;
};

// Funkcja licząca odległość między punktami o współrzędnych (startx, starty) i (targetx, targety)
float distance(int startx, int starty, int targetx, int targety) {
    return sqrt(pow((startx - targetx), 2) + pow((starty - targety), 2));
}

// Funkcja obliczająca współrzędne obiektu po jednej turze
Present calculate_next_position(const Present &present, int turns) {
    Present updated_present = present;
    updated_present.y = max(0, present.y - turns * present.vy); // Zakładamy, że vy jest dodatnie
    return updated_present;
}

// Funkcja obliczająca czas dotarcia gargulca do celu
int calculate_travel_time(int gargoyle_x, int gargoyle_y, int target_x, int target_y) {
    float dist = distance(gargoyle_x, gargoyle_y, target_x, target_y) - 30;//- 10; // 25 wentyl na zasięg zbierania
    return ceil(dist / 150); // Zaokrąglenie w górę, bo każda tura to 150 jednostek
}

bool can_reach_before_ground(int travel_time, const Present &target) {
    return target.y / target.vy > travel_time;
}
//================================================================
float heuristic(float adjusted_value, int my_TT, int enemy_TT){
    int whofirst;
    if (my_TT > enemy_TT){
        if( my_TT > enemy_TT + 2)
            whofirst = 2;
        else
            whofirst = 1;
    }
    else if (my_TT == enemy_TT)
        whofirst = 0;
    else
        whofirst = -1;

    return 2.5 * adjusted_value - enemy_TT * whofirst - my_TT;
}
//==================================================================
// Funkcja mówiąca czy zdążymy do prezentu przed wrogiem
// 0 - nie, 1 - będzie remis, 2 - będziemy pierwsi
int enemy_will_be_first(const Gargoyle &us, const Gargoyle &them, const Present &target) {
    int my_TT = calculate_travel_time(us.x, us.y, target.x, target.y);
    int enemy_TT = calculate_travel_time(them.x, them.y, target.x, target.y);
    if (my_TT < enemy_TT)
        return 2;
    else if (my_TT == enemy_TT)
        return 1;
    return 0;
}

// Funkcja obliczająca procentową wartość prezentu w zależności od jego pozycji na osi X
float calculate_value_by_position(int x) {
    float center_x = MAX_X / 2.0; // Środek planszy na osi X
    float distance_from_center = abs(x - center_x);
    float max_distance = center_x; // Maksymalna odległość od środka
    float percentage = 0.55 + 0.45 * (1 - (distance_from_center / max_distance));
    return percentage; // Zwraca wartość w zakresie od 0.3 do 1.0
}


// Funkcja wybierająca najlepszy cel dla gargulca
Present select_best_target(const Gargoyle &gargoyle, vector<Present> &presents, const vector<Gargoyle> &foe_gargoyles) {
    Present best_target = {-1, -1, -1, -1, -1};
    int max_score = -INT_MAX;
    //bool empty = true;
    for (auto &present : presents) {
        Present simulated_present = present;
        if(turn_counter == 1 && abs(present.x - gargoyle.x / 2.0) > 150){// w pierwszej turze nie rozpatrujemy oddalonych prezentów
            //cerr<<"Prezent id : "<<present.id<<" za daleko na pierwszą turę\n";
            continue;
        }
        //empty == false;
        for (int turns = 1; turns <= TURN_LIMIT; ++turns) {
            simulated_present.y -= simulated_present.vy;
            if (simulated_present.y <= 0) break;

            int travel_time = calculate_travel_time(gargoyle.x, gargoyle.y, simulated_present.x, simulated_present.y);
            if (travel_time != turns) continue; // nie czekamy nigdy na prezenty

            // Obliczenie wartości punktowej z uwzględnieniem pozycji
            float position_factor = calculate_value_by_position(simulated_present.x);
            float adjusted_value = static_cast<int>(present.value * position_factor);

            /*
            // Sprawdzenie, czy przeciwnik nie zdobędzie prezentu pierwszy
            bool enemy_first = false;
            for (const auto &foe : foe_gargoyles) {
                if (enemy_will_be_first(gargoyle, foe, simulated_present) == 0) {
                    enemy_first = true;
                    break;
                }
            }
            if (enemy_first) break;
            */

            // Wyliczenie potencjalnego wyniku
            float score = heuristic(adjusted_value, travel_time, calculate_travel_time(foe_gargoyles[0].x, foe_gargoyles[0].y, simulated_present.x, simulated_present.y));
            if (score > max_score) {
                max_score = score;
                best_target = simulated_present;
            }
            break;
        }
    }

    return best_target;
}

// Funkcja znajdująca najbardziej wartościowy prezent w zasięgu 300, do którego przeciwnik dotrze w 1 turę
Present find_high_value_present_in_range(const Gargoyle &gargoyle, const vector<Present> &presents, const vector<Gargoyle> &foe_gargoyles) {
    Present best_present = {-1, -1, -1, -1, -1};
    int max_value = -1;

    for (const auto &present : presents) {
        // Sprawdzenie czy prezent jest w zasięgu 300 od aktualnej pozycji gargulca
        if (distance(gargoyle.x, gargoyle.y, present.x, present.y) > 300) {
            continue;
        }

        // Sprawdzenie czy jakikolwiek przeciwnik może dotrzeć do tego prezentu w 1 turę
        for (const auto &foe : foe_gargoyles) {
            int foe_travel_time = calculate_travel_time(foe.x, foe.y, present.x, present.y);
            if (foe_travel_time == 1) {
                // Jeżeli przeciwnik dotrze do tego prezentu w 1 turę, sprawdzamy jego wartość
                if (present.value > max_value) {
                    max_value = present.value;
                    best_present = present;
                }
                break; // Nie trzeba sprawdzać innych przeciwników dla tego prezentu
            }
        }
    }

    return best_present;
}



int main() {
    int gargoyles_per_player;
    cin >> gargoyles_per_player;
    cin.ignore();

    turn_counter = 0;
    // Deklaracja wektorów na gargulce
    vector<Gargoyle> my_gargoyles(gargoyles_per_player);
    vector<Gargoyle> foe_gargoyles(gargoyles_per_player);

    Present last_target, last_last_target;// zmienna do prób załatania kręcenia się w miejscu

    // game loop
    while (1) {
        turn_counter += 1;
        cerr << turn_counter<<endl;
        int missed_presents_to_end;
        cin >> missed_presents_to_end;
        cin.ignore();
        int my_score;
        cin >> my_score;
        cin.ignore();

        // Odczyt danych o gargulcach gracza
        for (int i = 0; i < gargoyles_per_player; i++) {
            int x, y, cooldown;
            cin >> x >> y >> cooldown;
            cin.ignore();
            my_gargoyles[i] = {x, y, cooldown};
        }

        int foe_score;
        cin >> foe_score;
        cin.ignore();

        // Odczyt danych o gargulcach przeciwnika
        for (int i = 0; i < gargoyles_per_player; i++) {
            int x, y, cooldown;
            cin >> x >> y >> cooldown;
            cin.ignore();
            foe_gargoyles[i] = {x, y, cooldown};
        }

        int entity_count;
        cin >> entity_count;
        cin.ignore();

        // Deklaracja wektora na spadające prezenty
        vector<Present> presents(entity_count);
        for (int i = 0; i < entity_count; i++) {
            int id, value, x, y, vy;
            cin >> id >> value >> x >> y >> vy;
            cin.ignore();
            presents[i] = {id, value, x, y, vy};
        }

        // Decyzje dla każdego gargulca
        for (int i = 0; i < gargoyles_per_player; i++) {
            Present target = select_best_target(my_gargoyles[i], presents, foe_gargoyles);
            int travel_time = calculate_travel_time(my_gargoyles[i].x, my_gargoyles[i].y, target.x, target.y);
            //last_target = target;
            cerr << "Travel time to target : "<<travel_time<< endl;
            cerr << "Target stats value : "<<target.value<< " vy : "<< target.vy <<" id : "<< target.id;
            cerr << "x : " << target.x << " y : " << target.y << endl;

            Present best_fireball_target = find_high_value_present_in_range(my_gargoyles[i], presents, foe_gargoyles);
            if (target.id != -1 && target.id == best_fireball_target.id && my_gargoyles[i].cooldown == 0){
                cout << "FIREBALL " << best_fireball_target.id << endl;
            }
            else if (target.id != -1) {
                cout << "FLY " << target.x << " " << target.y << endl; //<< " ID "<<target.id<<" "<< target.x << "," << target.y << endl;
            } else {
                int a = (i - 1) * 150 + 600;
                cout << "FLY 960 "<< a << endl; // Brak odpowiedniego celu
            }

            // Usuwanie celu z wektora presents
            presents.erase(
                remove_if(presents.begin(), presents.end(), [&](const Present &p) { return p.id == target.id; }),
                presents.end()
            );
        }
    }
}