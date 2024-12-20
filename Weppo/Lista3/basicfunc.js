function forEach( a, f ) {
    for (let i = 0; i < a.length; i++) {
        f(a[i], i, a); // przekazuje element, jego indeks i całą tablicę
    }
}
function map(a, f) {
    const result = [];
    for (let i = 0; i < a.length; i++) {
        result.push(f(a[i], i, a)); // dodaje przekształcony element do wynikowej tablicy
    }
    return result;
}

function filter( a, f ) {
    const result = [];
    for (let i = 0; i < a.length; i++) {
        if (f(a[i], i, a)) { // dodaje element do wynikowej tablicy, jeśli warunek jest spełniony
            result.push(a[i]);
        }
    }
    return result;
}

const a = [1, 2, 3, 4];

// Przykład użycia forEach
console.log("forEach:");
forEach(a, _ => { console.log(_); });  // Funkcja strzałkowa: wypisuje każdy element tablicy
// [1, 2, 3, 4]

forEach(a, function(x) { console.log(x * 2); });  // Zwykła funkcja: wypisuje każdy element pomnożony przez 2
// [2, 4, 6, 8]

// Przykład użycia filter
console.log("filter:");
console.log(filter(a, _ => _ < 3));  // Funkcja strzałkowa: filtruje elementy mniejsze od 3
// [1, 2]

console.log(filter(a, function(x) { return x % 2 === 0; }));  // Zwykła funkcja: filtruje liczby parzyste
// [2, 4]

// Przykład użycia map
console.log("map:");
console.log(map(a, _ => _ * 2));  // Funkcja strzałkowa: mnoży każdy element przez 2
// [2, 4, 6, 8]

console.log(map(a, function(x) { return x + 1; }));  // Zwykła funkcja: dodaje 1 do każdego elementu
// [2, 3, 4, 5]
