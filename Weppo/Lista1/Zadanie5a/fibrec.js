function fibonacciRecursive(n) {
    if (n <= 1) return n;
    return fibonacciRecursive(n - 1) + fibonacciRecursive(n - 2);
}

function fibonacciIterative(n) {
    let a = 0, b = 1, temp;
    for (let i = 2; i <= n; i++) {
        temp = a + b;
        a = b;
        b = temp;
    }
    return n === 0 ? 0 : b;
}

function measureFibonacciPerformance(nMax) {
    console.log("n\tIteracyjny czas (ms)\tRekurencyjny czas (ms)");

    for (let n = 10; n <= nMax; n++) {
        // Mierzenie czasu dla wersji iteracyjnej
        console.time(`Iteracyjny czas dla n = ${n}`);
        fibonacciIterative(n);
        console.timeEnd(`Iteracyjny czas dla n = ${n}`);

        // Mierzenie czasu dla wersji rekurencyjnej
        if (n <= 45) { // Dla wartości n > 40 czas wykonania rekurencyjnej wersji staje się bardzo długi
            console.time(`Rekurencyjny czas dla n = ${n}`);
            fibonacciRecursive(n);
            console.timeEnd(`Rekurencyjny czas dla n = ${n}`);
        } else {
            console.log(`${n}\t---\t\t\t\t--- (zbyt długi dla rekurencyjnej)`);
        }
    }
}

measureFibonacciPerformance(45);
