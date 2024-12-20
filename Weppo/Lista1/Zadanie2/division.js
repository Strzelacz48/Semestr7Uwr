function isDivisibleByDigits(num) {
    const digits = Array.from(String(num), Number);
    for (let digit of digits) {
        if (digit === 0 || num % digit !== 0) {
            return false;
        }
    }
    return true;
}

function isDivisibleBySumOfDigits(num) {
    const sumOfDigits = Array.from(String(num), Number).reduce((a, b) => a + b, 0); // Suma cyfr liczby
    return num % sumOfDigits === 0;
}

function findSpecialNumbers(limit) {
    let result = [];
    
    for (let i = 1; i <= limit; i++) {
        if (isDivisibleByDigits(i) && isDivisibleBySumOfDigits(i)) {
            result.push(i); // Dodajemy liczbę do wyników, jeśli spełnia oba warunki
        }
    }
    
    return result;
}

const specialNumbers = findSpecialNumbers(100000);

console.log(specialNumbers);
