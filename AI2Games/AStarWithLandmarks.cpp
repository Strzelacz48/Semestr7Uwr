#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <algorithm>

using namespace std;

struct Point {
    int x, y;
    bool operator==(const Point &other) const {
        return x == other.x && y == other.y;
    }
};

// Mapa i lista przechodnich pól
int width, height;
float efficiency;
vector<string> map;
vector<Point> passable_tiles;

// Przesunięcia dla ruchów kardynalnych i diagonalnych
vector<Point> directions = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

// Funkcja obliczająca odległość między dwoma punktami na podstawie kosztu ruchu
double distance(const Point &a, const Point &b) {
    int dx = abs(a.x - b.x);
    int dy = abs(a.y - b.y);
    if (dx == 0 || dy == 0) return dx + dy; // Kardynalny
    return sqrt(2) * min(dx, dy) + abs(dx - dy); // Diagonalny
}

// Funkcja przetwarzająca mapę na graf i znajdująca przechodnie pola
void parse_map() {
    cin >> width >> height;
    map.resize(height);
    //cerr<<"width, hight : "<<width<<height<<"\n";
    for (int y = 0; y < height; y++) {
        cin >> map[y];
        for (int x = 0; x < width; x++) {
            //cerr<<"passable tiles check\n";
            if (map[y][x] == '.') {
                passable_tiles.push_back({x, y});
            }
        }
    }
}

// Zwraca sąsiadów dla podanego punktu na grafie
vector<Point> get_neighbors(const Point &p) {
    vector<Point> neighbors;
    for (const auto &dir : directions) {
        Point neighbor = {p.x + dir.x, p.y + dir.y};
        if (neighbor.x >= 0 && neighbor.y >= 0 && neighbor.x < width && neighbor.y < height &&
            map[neighbor.y][neighbor.x] == '.') {
            neighbors.push_back(neighbor);
        }
    }
    return neighbors;
}

// Funkcja oceniająca gęstość punktu przez liczbę dostępnych wierzchołków w określonym promieniu
int calculate_density(const Point &p, int radius) {
    queue<pair<Point, int>> q;
    q.push({p, 0});
    vector<vector<bool>> visited(height, vector<bool>(width, false));
    visited[p.y][p.x] = true;

    int count = 1;  // Liczymy także punkt centralny
    while (!q.empty()) {
        auto [current, dist] = q.front();
        q.pop();

        if (dist >= radius) continue;

        for (const auto &neighbor : get_neighbors(current)) {
            if (!visited[neighbor.y][neighbor.x]) {
                visited[neighbor.y][neighbor.x] = true;
                q.push({neighbor, dist + 1});
                count++;
            }
        }
    }
    return count;
}

// Znajdowanie punktów odniesienia o największej gęstości w promieniu `radius`
vector<Point> select_dense_landmarks(int landmarks_num, int radius) {
    vector<pair<int, Point>> densities;
    cerr<<"calculating density\n";
    for (const auto &p : passable_tiles) {
        int density = calculate_density(p, radius);
        densities.push_back({density, p});
    }
    cerr<<"Finished calculating density\n";

    // Sortowanie według gęstości malejąco (po wartości `int`)
    sort(densities.rbegin(), densities.rend(), [](const pair<int, Point>& a, const pair<int, Point>& b) {
        return a.first < b.first;
    });
    cerr<<"Sorting finished\n";
    vector<Point> landmarks;
    for (int i = 0; i < landmarks_num && i < densities.size(); i++) {
        landmarks.push_back(densities[i].second);
    }
    cerr<<"Landmarks chosen\n";
    return landmarks;
}

int main() {
    int landmarks_num, radius = 5; // radius może być dostosowany w zależności od potrzeb
    cin >> landmarks_num>>efficiency;
    cerr<<"efficiency : "<<efficiency;
    parse_map();
    cerr<<landmarks_num<<"\n";
    cerr<<"Passable tiles size : "<<passable_tiles.size()<<"\n";

    // Wybieramy punkty odniesienia o największej gęstości
    vector<Point> landmarks = select_dense_landmarks(landmarks_num, radius);
    cerr<<landmarks.size();
    // Wypisanie współrzędnych punktów odniesienia
    for (const auto &lm : landmarks) {
        cout << lm.x << " " << lm.y << endl;
    }

    return 0;
}
