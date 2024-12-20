const fs = require('fs');

const filePath = './example.txt';

// Odczyt pliku za pomocą klasycznego interfejsu callback
fs.readFile(filePath, 'utf8', (err, data) => {
    if (err) {
        console.error('Błąd podczas odczytu pliku:', err.message);
    } else {
        console.log('Zawartość pliku (callback):\n', data);
    }
});

//Hand Made Promise===================================================================
function readFilePromise(filePath, encoding) {
    return new Promise((resolve, reject) => {
        fs.readFile(filePath, encoding, (err, data) => {
            if (err) {
                reject(err);
            } else {
                resolve(data);
            }
        });
    });
}

// Użycie z Promise::then
readFilePromise('./example.txt', 'utf8')
    .then((data) => {
        console.log('Zawartość pliku (Promise, ręcznie):\n', data);
    })
    .catch((err) => {
        console.error('Błąd podczas odczytu pliku (Promise, ręcznie):', err.message);
    });

// Użycie z async/await
(async () => {
    try {
        const data = await readFilePromise('./example.txt', 'utf8');
        console.log('Zawartość pliku (Promise, ręcznie, async/await):\n', data);
    } catch (err) {
        console.error('Błąd (Promise, ręcznie, async/await):', err.message);
    }
})();

//Promisify===========================================================================
const util = require('util');

// Promisify funkcji readFile
const readFileAsync = util.promisify(fs.readFile);

// Użycie z Promise::then
readFileAsync('./example.txt', 'utf8')
    .then((data) => {
        console.log('Zawartość pliku (promisify):\n', data);
    })
    .catch((err) => {
        console.error('Błąd podczas odczytu pliku (promisify):', err.message);
    });

// Użycie z async/await
(async () => {
    try {
        const data = await readFileAsync('./example.txt', 'utf8');
        console.log('Zawartość pliku (promisify, async/await):\n', data);
    } catch (err) {
        console.error('Błąd (promisify, async/await):', err.message);
    }
})();

//Promises==============================================================================
const fsPromises = require('fs/promises');

// Użycie z Promise::then
fsPromises.readFile('./example.txt', 'utf8')
    .then((data) => {
        console.log('Zawartość pliku (fs.promises):\n', data);
    })
    .catch((err) => {
        console.error('Błąd podczas odczytu pliku (fs.promises):', err.message);
    });

// Użycie z async/await
(async () => {
    try {
        const data = await fsPromises.readFile('./example.txt', 'utf8');
        console.log('Zawartość pliku (fs.promises, async/await):\n', data);
    } catch (err) {
        console.error('Błąd (fs.promises, async/await):', err.message);
    }
})();
