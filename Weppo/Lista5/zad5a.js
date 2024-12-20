const https = require('https'); // Można użyć http zamiast https

const url = 'https://jsonplaceholder.typicode.com/posts/1'; // Przykładowy zasób

https.get(url, (response) => {
    let data = '';

    // Odbieranie danych w kawałkach
    response.on('data', (chunk) => {
        data += chunk;
    });

    // Po zakończeniu pobierania
    response.on('end', () => {
        console.log('Zawartość zasobu:\n', data);
    });

}).on('error', (err) => {
    console.error('Błąd podczas pobierania:', err.message);
});
