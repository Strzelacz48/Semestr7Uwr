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
Present calculate_next_position(Present &present) {
    Present updated_present = present;
    updated_present.y = max(0, present.y - present.vy); // Zakładamy, że `vy` jest dodatnie
    return updated_present;
}

// Funkcja obliczająca czas dotarcia gargulca do celu
int calculate_travel_time(int gargoyle_x, int gargoyle_y, int target_x, int target_y) {
    float dist = distance(gargoyle_x, gargoyle_y, target_x, target_y) - 25;// 25 wentyl na zasięg zbierania
    return ceil(dist / 150); // Zaokrąglenie w górę, bo każda tura to 150 jednostek
}

bool can_reach_before_ground(int travel_time, Present target){
    if (target.y / target.vy > travel_time)
        return true;
    return false;
}
//funkcja mówiąca czy zdążymy do prezentu przed wrogiem 0 - nie, - 1 bedzie remis, 2 - bedziemy pierwsi
int enemy_will_be_first(Gargoyle us, Gargoyle them, Present target){
    int my_TT = calculate_travel_time(us.x, us.y, target.x, target.y);
    int enemy_TT = calculate_travel_time(them.x, them.y, target.x, target.y);
    if (my_TT < enemy_TT)
        return 2;
    else if (my_TT == enemy_TT)
        return 1;
    return 0;
}

Present closest_Present(){
    
}
// Funkcja wybierająca najlepszy cel dla gargulca
Present select_best_target(const Gargoyle &gargoyle, vector<Present> &presents, const vector<Gargoyle> &foe_gargoyles) {
    Present best_target = {-1, -1, -1, -1, -1};
    int min_time = INT_MAX, best_score = - MAX_X/150;

    for (auto &present : presents) {
        if (present.y > MAX_Y) continue;

        // Kopia prezentu, aby symulować jego pozycję w przyszłości
        Present simulated_present = present;

        int turns = 0;

        while (true) {// zmienic na fora ktory zwieksza ilosc i sprawdzanie pozycji policzyc  y + i * yv
            turns++;
            // Oblicz przyszłą pozycję prezentu
            simulated_present = calculate_next_position(simulated_present);
            //cerr<< "Turn : "<< turns <<" present id : "<<present.id<<" y: "<<present.y<<endl;
            //cerr<< " simulated present id : "<<simulated_present.id<<" y: "<<simulated_present.y<<endl;
            // Sprawdź, czy prezent jest nadal w grze
            if (simulated_present.y > HIGHT_LIMIT && simulated_present.y <= MAX_Y) {
                //cerr << "present unreachable\n";
                continue;
            }
            // Oblicz czas podróży gargulca do przyszłej pozycji prezentu
            int travel_time = calculate_travel_time(
                gargoyle.x, gargoyle.y, 
                simulated_present.x, simulated_present.y
            );
            
            if (!can_reach_before_ground(turns, present)){
                cerr<< "Present will hit ground before we can get to it\n";
                break;
            }
            //Ten fragment sprawai że kod działa gorzej ?
            bool enemy_first = false;
            for(int i = 0; i < foe_gargoyles.size(); i++){
                if(enemy_will_be_first(gargoyle, foe_gargoyles[i], simulated_present) == 0){
                    enemy_first = true;
                    break;
                }
            }

            if (enemy_first){
                cerr<< "Foe will be first to the target\n";
                break;
            }
            //cerr <<"Travel time do presentu id : "<< simulated_present.id<< " tt: "<<travel_time<<endl;
            
            // Sprawdź, czy gargulec zdąży dotrzeć w określonym czasie
            if (travel_time <= turns) {

                // Sprawdź, czy dystans jest akceptowalny
                if (travel_time < min_time && best_score < simulated_present.value + 1 - turns) {
                    best_score = simulated_present.value + 1 - turns;
                    best_target = simulated_present;
                    min_time = travel_time;
                }
                break; // Znaleziono możliwy cel, wyjście z pętli
            }
        }
    }
    if (best_target.id != -1)
        cerr<<"Best target : "<<best_target.id<< " x : "<<best_target.x<<" y :" <<best_target.y<<endl;
    else
        cerr<<"Best target is null";
    return best_target;
}


int main() {
    int gargoyles_per_player;
    cin >> gargoyles_per_player;
    cin.ignore();

    // Deklaracja wektorów na gargulce
    vector<Gargoyle> my_gargoyles(gargoyles_per_player);
    vector<Gargoyle> foe_gargoyles(gargoyles_per_player);

    // game loop
    while (1) {
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
            //cerr << " id : "<< id << " vy : "<< vy;
            cin.ignore();
            presents[i] = {id, value, x, y, vy};
        }

        // Tablica do śledzenia ruchów gargulców
        vector<pair<int, int>> gargoyle_moves(gargoyles_per_player);

        // Decyzje dla każdego gargulca
        for (int i = 0; i < gargoyles_per_player; i++) {
            Present target = select_best_target(my_gargoyles[i], presents, foe_gargoyles);
            if (target.id != -1) {
                int target_x = target.x;
                int target_y = target.y;
                gargoyle_moves[i] = {target_x, target_y};
                cout << "FLY " << target_x << " " << target_y << endl;
            } else {
                gargoyle_moves[i] = {800, 500};
                cout << "FLY 960 750" << endl; // Brak odpowiedniego celu
            }
        }

        // Sprawdź, czy gargulce zniszczyły prezenty
        /*for (int i = 0; i < gargoyles_per_player; i++) {
            for (auto &present : presents) {
                if (!present.destroyed && distance(gargoyle_moves[i].first, gargoyle_moves[i].second, present.x, present.y) < 30) {
                    present.destroyed = true;
                    my_score += REWARD;
                }
            }
        }*/
    }
}
