
function findPrimeNumbers(limit) {
    let result = [];
    
    for (let i = 1; i <= limit; i++) {
        let prime = true;
        for (let j = 2; j * j <= i; j++) {
            if (i % j == 0){
                prime = false;
                break;
            }
        }
        if (prime){
        result.push(i);
        }
    }
    return result;
}

const primeNumbers = findPrimeNumbers(100000);

console.log(primeNumbers);