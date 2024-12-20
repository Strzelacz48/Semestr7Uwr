function fib1() {
    let a = 0, b = 1;
    return {
        next: function() {
            let value = a;
            [a, b] = [b, a + b]; // Przejście do kolejnych wartości Fib
            return { value: value, done: false }; // done jest zawsze false, bo jest to nieskończony generator
        }
    };
}

// Przykład użycia nieskończonego iteratora
var _it = fib1();
for (var _result; _result = _it.next(), !_result.done;) {
    console.log(_result.value); // Wypisze liczby Fib
    if (_result.value > 100) break;
}

function* fib2() {
    let a = 0, b = 1;
    while (true) {
        yield a;
        [a, b] = [b, a + b]; // Przejście do kolejnych wartości Fib
    }
}

// Przykład użycia nieskończonego generatora z for-of
for (var i of fib2()) {
    console.log(i);
    if (i > 100) break;
}
