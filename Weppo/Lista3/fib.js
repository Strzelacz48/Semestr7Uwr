// Rekurencyjna implementacja z memoizacją
function fibMemoized(n, memo = {}) {
    if (n <= 1) return n;
    if (memo[n]) return memo[n];
    memo[n] = fibMemoized(n - 1, memo) + fibMemoized(n - 2, memo);
    return memo[n];
}
// Rekurencyjna implementacja bez memoizacji
function fibRecursive(n) {
    if (n <= 1) return n;
    return fibRecursive(n - 1) + fibRecursive(n - 2);
}
const n = 40;  // Możemy dostosować wartość n dla różnych testów

console.time("Rekurencyjna bez memoizacji");
console.log("fibRecursive(", n, ") =", fibRecursive(n));
console.timeEnd("Rekurencyjna bez memoizacji");

console.time("Rekurencyjna z memoizacją");
console.log("fibMemoized(", n, ") =", fibMemoized(n));
console.timeEnd("Rekurencyjna z memoizacją");
