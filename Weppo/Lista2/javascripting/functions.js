function eat (food) {
    return food + ' tasted really good.'
  }
  console.log(eat('bananas'))

const arr = [10, 20, 30];
arr[2] = 31;
arr["2"] = 32;
console.log(arr.length)
arr[4] = 'another string'
console.log(arr.length)
arr['string'] = 'another string'
const objKey = { key: "value" };
console.log(arr.length)
arr[objKey] = "object as key";
console.log(arr.length)
console.log(arr[objKey]); // "object as key"
console.log(arr); // [10, 20, 30, "[object Object]": "object as key"]

console.log((![]+[])[+[]])
console.log((![]+[])[+!+[]])
console.log(([![]]+[][[]])[+!+[]+[+[]]])
console.log((![]+[])[!+[]+!+[]])
console.log((![]+[])[+[]]+(![]+[])[+!+[]]+([![]]+[][[]])[+!+[]+[+[]]]+(![]+[])[!+[]+!+[]]);

// Definiowanie nowego obiektu
const person = {
    // Pole
    name: "Alice",
    
    // Metoda
    greet() {
        return `Hello, my name is ${this.name}`;
    },
    
    // Właściwość z akcesorami get i set
    get age() {
        return this._age;
    },
    set age(value) {
        if (value >= 0) {
            this._age = value;
        } else {
            console.log("Age must be a positive number.");
        }
    }
};

// Użycie obiektu
person.age = 25; // Ustawienie wartości przez setter
console.log(person.age); // Pobranie wartości przez getter - wynik: 25
console.log(person.greet()); // Wywołanie metody - wynik: "Hello, my name is Alice"