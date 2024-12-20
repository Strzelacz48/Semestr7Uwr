const fs = require('fs');
const readline = require('readline');

// Ścieżka do pliku logów
const logFilePath = './server.log';

// Tworzenie interfejsu do czytania pliku linia po linii
const rl = readline.createInterface({
    input: fs.createReadStream(logFilePath),
    crlfDelay: Infinity, // Rozpoznaje linie zakończone \r\n lub \n
});

// Mapa przechowująca liczbę żądań dla każdego adresu IP
const ipRequestCount = new Map();

rl.on('line', (line) => {
    const parts = line.split(' '); // Podziel linię na części
    const ip = parts[1]; // Druga kolumna to adres IP

    if (ip) {
        // Zwiększ licznik dla adresu IP
        ipRequestCount.set(ip, (ipRequestCount.get(ip) || 0) + 1);
    }
});

rl.on('close', () => {
    // Sortowanie adresów IP według liczby żądań malejąco
    const topIPs = Array.from(ipRequestCount.entries())
        .sort((a, b) => b[1] - a[1]) // Sortowanie po liczbie żądań
        .slice(0, 3); // Pobierz trzy najlepsze wyniki

    // Wygenerowanie raportu
    console.log('Raport: Najwięcej żądań do serwera wysłały te adresy IP:');
    topIPs.forEach(([ip, count]) => {
        console.log(`${ip} ${count}`);
    });
});
