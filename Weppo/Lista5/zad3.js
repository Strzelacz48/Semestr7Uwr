// Losowanie liczby z zakresu 0–100
const randomNumber = Math.floor(Math.random() * 101);

console.log("Wylosowałem liczbę od 0 do 100. Spróbuj ją zgadnąć!");
console.log(randomNumber);

process.stdin.setEncoding("utf-8");

// Obsługa zgadywania
process.stdin.on("data", (input) => {
    const userGuess = parseInt(input.trim(), 10); // Pobranie liczby od użytkownika
    
    if (isNaN(userGuess)) {
        console.log("Proszę wprowadzić liczbę!");
        return;
    }

    if (userGuess === randomNumber) {
        console.log("To jest właśnie ta liczba! Gratulacje!");
        process.stdin.pause(); // Koniec gry
        return;
    } else if (userGuess < randomNumber) {
        console.log("Moja liczba jest większa. Spróbuj ponownie:");
    } else {
        console.log("Moja liczba jest mniejsza. Spróbuj ponownie:");
    }
});
