const fs = require('fs'); // Import modułu fs

const filePath = './example.txt'; // Ścieżka do pliku

fs.readFile(filePath, 'utf8', (err, data) => {
    if (err) {
        console.error(`Błąd podczas czytania pliku: ${err.message}`);
        return;
    }
    console.log(`Zawartość pliku:\n${data}`);
});
