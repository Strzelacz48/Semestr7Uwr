//zachłan
#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <algorithm>
#include <limits>
#include <random>

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
    for (int y = 0; y < height; y++) {
        cin >> map[y];
        for (int x = 0; x < width; x++) {
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

// BFS do znajdowania najdalszego punktu od danego punktu
Point find_farthest_point(const Point &start) {
    queue<pair<Point, int>> q;
    q.push({start, 0});
    vector<vector<bool>> visited(height, vector<bool>(width, false));
    visited[start.y][start.x] = true;

    Point farthest = start;
    int max_distance = 0;

    while (!q.empty()) {
        auto [current, dist] = q.front();
        q.pop();

        if (dist > max_distance) {
            max_distance = dist;
            farthest = current;
        }

        for (const auto &neighbor : get_neighbors(current)) {
            if (!visited[neighbor.y][neighbor.x]) {
                visited[neighbor.y][neighbor.x] = true;
                q.push({neighbor, dist + 1});
            }
        }
    }
    return farthest;
}

// Funkcja znajdująca najdalszy punkt od istniejących punktów orientacyjnych
Point find_farthest_from_landmarks(const vector<Point> &landmarks) {
    Point farthest = landmarks[0];
    int max_min_distance = -1;

    for (const auto &p : passable_tiles) {
        int min_distance = numeric_limits<int>::max();
        for (const auto &lm : landmarks) {
            min_distance = min(min_distance, static_cast<int>(distance(p, lm)));
        }
        if (min_distance > max_min_distance) {
            max_min_distance = min_distance;
            farthest = p;
        }
    }
    return farthest;
}

// Wybieranie landmarków według algorytmu "najdalszy punkt"
vector<Point> select_landmarks(int landmarks_num) {
    vector<Point> landmarks;

    // Wybierz losowy punkt startowy jako pierwszy landmark
    mt19937 gen(random_device{}());
    uniform_int_distribution<> dist(0, passable_tiles.size() - 1);
    Point start = passable_tiles[dist(gen)];
    landmarks.push_back(start);

    // Dodajemy kolejne punkty, wybierając ten najdalszy od istniejących landmarków
    for (int i = 1; i < landmarks_num; i++) {
        Point farthest = find_farthest_from_landmarks(landmarks);
        landmarks.push_back(farthest);
    }

    return landmarks;
}

int main() {
    int landmarks_num;
    cin >> landmarks_num>>efficiency;
    parse_map();

    // Wybieranie punktów orientacyjnych za pomocą algorytmu najdalszego punktu
    vector<Point> landmarks = select_landmarks(landmarks_num);

    // Wypisanie współrzędnych punktów orientacyjnych
    for (const auto &lm : landmarks) {
        cout << lm.x << " " << lm.y << endl;
    }

    return 0;
}
