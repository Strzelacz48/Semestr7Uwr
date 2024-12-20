console.log("Podaj swoje imię:");

// Obsługa wejścia z konsoli
process.stdin.setEncoding("utf-8"); // Ustawienie kodowania dla strumienia wejściowego
process.stdin.on("data", (input) => {
    const name = input.trim();
    console.log(`Witaj, ${name}!`);
    process.stdin.pause(); // Zakończenie odczytu danych
});
