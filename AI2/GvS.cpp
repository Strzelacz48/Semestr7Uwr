#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <limits.h>

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
    int vy; // szybkość spadania
    bool destroyed = false; // Flaga wskazująca, czy prezent został zniszczony
};

// Funkcja licząca odległość między punktami o współrzędnych (startx, starty) i (targetx, targety)
float distance(int startx, int starty, int targetx, int targety) {
    return sqrt(pow((startx - targetx), 2) + pow((starty - targety), 2));
}

// Funkcja obliczająca współrzędne obiektu po jednej turze
void calculate_next_position(Present &present) {
    present.y = max(0, present.y + present.vy); // Zakładamy, że `vy` jest ujemne
}

// Funkcja obliczająca czas dotarcia gargulca do celu
int calculate_travel_time(int gargoyle_x, int gargoyle_y, int target_x, int target_y) {
    float dist = distance(gargoyle_x, gargoyle_y, target_x, target_y);
    return ceil((dist - 25 )/ 150); // Zaokrąglenie w górę, bo każda tura to 150 jednostek
}

// Funkcja wybierająca najlepszy cel dla gargulca
Present* select_best_target(const Gargoyle &gargoyle, vector<Present> &presents, const vector<Gargoyle> &foe_gargoyles) {
    Present* best_target = nullptr;
    int min_time = INT_MAX;

    for (auto &present : presents){
        calculate_next_position(present);
    }

    for (auto &present : presents) {
        if (present.destroyed || present.y > 750) continue;

        int travel_time = calculate_travel_time(gargoyle.x, gargoyle.y, present.x, present.y);
        int foe_time = INT_MAX;

        // Sprawdź czas dotarcia najbliższego gargulca przeciwnika
        for (const auto &foe : foe_gargoyles) {
            int t = calculate_travel_time(foe.x, foe.y, present.x, present.y);
            foe_time = min(foe_time, t);
        }

        // Oblicz czas, jaki prezent pozostanie w grze
        int remaining_turns = ceil(float(present.y) / abs(present.vy));

        // Wybierz prezent, do którego gargulec zdąży dotrzeć pierwszy
        if (travel_time < min_time && travel_time <= remaining_turns && travel_time <= foe_time) {
            best_target = &present;
            min_time = travel_time;
        }
    }

    return best_target;
}

int main() {
    int gargoyles_per_player; // liczba gargulców w drużynie
    cin >> gargoyles_per_player;
    cin.ignore();

    // Deklaracja wektorów na gargulce
    vector<Gargoyle> my_gargoyles(gargoyles_per_player);
    vector<Gargoyle> foe_gargoyles(gargoyles_per_player);

    // game loop
    while (1) {
        int missed_presents_to_end; // ile prezentów musi jeszcze spaść na ziemię aby gra się zakończyła
        cin >> missed_presents_to_end;
        cin.ignore();
        int my_score; // mój wynik
        cin >> my_score;
        cin.ignore();

        // Odczyt danych o gargulcach gracza
        for (int i = 0; i < gargoyles_per_player; i++) {
            int x, y, cooldown;
            cin >> x >> y >> cooldown;
            cin.ignore();
            my_gargoyles[i] = {x, y, cooldown};
        }

        int foe_score; // wynik oponenta
        cin >> foe_score;
        cin.ignore();

        // Odczyt danych o gargulcach przeciwnika
        for (int i = 0; i < gargoyles_per_player; i++) {
            int x, y, cooldown;
            cin >> x >> y >> cooldown;
            cin.ignore();
            foe_gargoyles[i] = {x, y, cooldown};
        }

        int entity_count; // liczba spadających obiektów
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

        // Aktualizacja pozycji każdego prezentu
        for (auto &present : presents) {
            calculate_next_position(present);
        }

        // Decyzje dla każdego gargulca
        for (int i = 0; i < gargoyles_per_player; i++) {
            Present* target = select_best_target(my_gargoyles[i], presents, foe_gargoyles);
            if (target) {
                cout << "FLY " << target->x << " " << target->y << endl;
                target->destroyed = true; // Zakładamy, że gargulec wybiera ten prezent
            } else {
                cout << "FLY 800 500" << endl; // Brak odpowiedniego celu, gargulec idzie w domyślny punkt
            }
        }
    }
}
