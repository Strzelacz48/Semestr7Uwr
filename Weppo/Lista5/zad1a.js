// mathUtils.js
const add = (a, b) => a + b;
const subtract = (a, b) => a - b;

module.exports = { add, subtract };

// main.js
const mathUtils = require('./mathUtils');

console.log(mathUtils.add(5, 3));       // 8
console.log(mathUtils.subtract(5, 3)); // 2
