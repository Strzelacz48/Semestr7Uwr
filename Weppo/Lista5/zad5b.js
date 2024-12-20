const https = require('https'); // Można użyć http zamiast https

function fetchUrl(url) {
    return new Promise((resolve, reject) => {
        https.get(url, (response) => {
            let data = '';

            // Odbieranie danych w kawałkach
            response.on('data', (chunk) => {
                data += chunk;
            });

            // Po zakończeniu pobierania
            response.on('end', () => {
                resolve(data); // Zwracamy pełną zawartość przez resolve
            });

            // Obsługa błędów podczas odpowiedzi
            response.on('error', (err) => {
                reject(err);
            });
        // Obsługa błędów podczas nawiązywania połączenia
        }).on('error', (err) => {            
            reject(err);
        });
    });
}

// Wykorzystanie obietnicy
const url = 'https://jsonplaceholder.typicode.com/posts/1';

fetchUrl(url)
    .then((data) => {
        console.log('Zawartość zasobu:\n', data);
    })
    .catch((err) => {
        console.error('Błąd podczas pobierania:', err.message);
    });
