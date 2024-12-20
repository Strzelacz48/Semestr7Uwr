
function Tree(val, left, right) {
    this.left = left;
    this.right = right;
    this.val = val;
}

// Definicja generatora do iteracji wszerz (Breadth-First Search)
Tree.prototype[Symbol.iterator] = function*() {
    // Inicjalizujemy kolejkę z korzeniem drzewa
    let queue = [this];

    // Dopóki kolejka nie jest pusta
    while (queue.length > 0) {
        // Pobieramy pierwszy element z kolejki
        const node = queue.splice(0, 1)[0];

        // Zwracamy wartość bieżącego węzła
        yield node.val;

        // Jeśli istnieje lewy syn, dodajemy go do kolejki
        if (node.left) queue.push(node.left);
        // Jeśli istnieje prawy syn, dodajemy go do kolejki
        if (node.right) queue.push(node.right);
    }
};

// Przykład użycia
const root = new Tree(1, new Tree(2, new Tree(3)), new Tree(4));

for (const e of root) {
    console.log(e); // Powinno wyświetlić: 1 2 4 3
}
    