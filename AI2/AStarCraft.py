import random
import copy
import time
import math

# Kierunki i ich zmiany współrzędnych
directions = {
    'U': (0, -1),  # Góra
    'R': (1, 0),   # Prawo
    'D': (0, 1),   # Dół
    'L': (-1, 0)   # Lewo
}

# Zmiana kierunku robota na podstawie napotkanej strzałki
arrow_turn = {
    'U': 'U',
    'R': 'R',
    'D': 'D',
    'L': 'L'
}

# Plansza 19x10
board = []
robot_count = 0
robots = []

def parse_input(board, robot_count, robots):
    # Wczytaj mapę planszy
    board = []
    editable_rows = []
    for y in range(10):  # Dostajemy input wierszami
        line = input().strip()
        line = list(line)
        board.append(line)
        row = []
        for x in range(len(line)):
            if line[x] == '.':
                row.append((x, y))
        editable_rows.append(row)

    # Wczytaj roboty
    robot_count = int(input())
    robots = []
    for _ in range(robot_count):
        x, y, direction = input().split()
        robots.append((int(x), int(y), direction))
    
    return board, robot_count, robots, editable_rows

# Oblicza nową pozycję z uwzględnieniem przejścia na drugą stronę
def wrap_position(x, y):
    return x % 19, y % 10

def valid_arrow(x, y, direction, board):
    dx, dy = directions[direction]
    x, y = wrap_position(x + dx, y + dy)
    if board[y][x] == '#':
        return False
    return True

# Symulacja ruchów robota dla danej konfiguracji planszy
def simulate_robot(x, y, direction, board):
    visited = set()
    moves = 0

    while True:
        # Zmieniamy kierunek robota, jeśli jest strzałka
        if board[y][x] in arrow_turn:
            direction = board[y][x]

        # Nowa pozycja na podstawie kierunku
        dx, dy = directions[direction]
        x, y = wrap_position(x + dx, y + dy)

        # Sprawdzenie, czy robot już odwiedził pozycję lub trafił na pustkę
        if board[y][x] == '#' or (x, y, direction) in visited:
            break

        # Dodanie pozycji do odwiedzonych
        visited.add((x, y, direction))
        moves += 1

    return moves

# Ocena planszy dla danej konfiguracji strzałek
def evaluate_board(board, robots):
    total_moves = 0
    for (x, y, direction) in robots:
        total_moves += simulate_robot(x, y, direction, board)
    return total_moves

# Wygenerowanie losowej konfiguracji strzałek
def randomize_arrows(board, editable_rows):
    new_board = copy.deepcopy(board)
    arrow_positions = {}
    for row in editable_rows:
        for x, y in row:
            direction = random.choice(['U', 'R', 'D', 'L', '.'])
            if direction != '.' and valid_arrow(x, y, direction, board):
                new_board[y][x] = direction
                arrow_positions[(x, y)] = direction
    return new_board, arrow_positions

# Przeprowadzenie jednej perturbacji konfiguracji
def perturb_arrows(board, editable_rows, arrow_positions):
    new_board = copy.deepcopy(board)
    new_arrow_positions = arrow_positions.copy()  # Kopiowanie strzałek

    for row in editable_rows:
        if not row:
            continue
        x, y = random.choice(row)
        direction = random.choice(['U', 'R', 'D', 'L', '.'])

        if direction == '.':
            continue

        if valid_arrow(x, y, direction, board):
            new_board[y][x] = direction
            new_arrow_positions[(x, y)] = direction  # Aktualizacja lub dodanie nowej strzałki

    return new_board, new_arrow_positions

if __name__ == "__main__":
    board, robot_count, robots, editable_rows = parse_input(board, robot_count, robots)

    # Parametry algorytmu simulated annealing
    total_time_limit = 1  # Całkowity czas wykonania (1 sekunda)
    runs = 6  # Liczba prób
    individual_time_limit = total_time_limit / runs  # Czas na jedną próbę

    # Ustawienie najlepszego znalezionego rozwiązania dla wszystkich prób
    best_overall_score = 0
    best_overall_board = None
    best_overall_arrows = {}

    for _ in range(runs):
        start_time = time.time()
        temperature = 100
        cooling_rate = 0.99
        min_temperature = 1e-3

        # Inicjalizacja początkowej konfiguracji
        best_board, best_arrows = randomize_arrows(board, editable_rows)
        best_score = evaluate_board(best_board, robots)
        current_board, current_arrows = best_board, best_arrows
        current_score = best_score

        # Wykonanie simulated annealing
        while time.time() - start_time < individual_time_limit:
            # Stworzenie nowej, perturbowanej konfiguracji
            new_board, new_arrows = perturb_arrows(current_board, editable_rows, current_arrows)
            new_score = evaluate_board(new_board, robots)

            # Obliczenie zmiany energii
            delta_score = new_score - current_score

            # Akceptacja nowego rozwiązania z pewnym prawdopodobieństwem
            if delta_score > 0 or random.uniform(0, 1) < math.exp(delta_score / temperature):
                current_board = new_board
                current_score = new_score
                current_arrows = new_arrows

                # Aktualizacja najlepszego rozwiązania dla bieżącej próby
                if current_score > best_score:
                    best_score = current_score
                    best_board = current_board
                    best_arrows = current_arrows

            # Obniżenie temperatury
            temperature *= cooling_rate

        # Aktualizacja najlepszego wyniku dla wszystkich prób
        if best_score > best_overall_score:
            best_overall_score = best_score
            best_overall_board = best_board
            best_overall_arrows = best_arrows

    # Wypisz najlepszy wynik spośród wszystkich prób w formacie X Y DIR
    output = " ".join(f"{x} {y} {direction}" for (x, y), direction in best_overall_arrows.items())
    print(output)
