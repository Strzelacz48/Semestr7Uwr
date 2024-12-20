function* take(it, top) {
    let count = 0;
    for (let value of it) {
        if (count >= top) break;
        yield value;
        count++;
    }
}

// Testowanie z nieskończonym generatorem Fibonacciego
function* fib() {
    let a = 0, b = 1;
    while (true) {
        yield a;
        [a, b] = [b, a + b];
    }
}

// Wypisanie dokładnie 10 liczb Fibonacciego za pomocą `take`
for (let num of take(fib(), 10)) {
    console.log(num); // 0, 1, 1, 2, 3, 5, 8, 13, 21, 34
}
