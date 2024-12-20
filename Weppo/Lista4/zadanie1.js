// Definicja funkcji getLastProto
function getLastProto(o) {
    var p = o;
    do {
        o = p;
        p = Object.getPrototypeOf(o);
    } while (p);
    return o;
}

// Przykładowe obiekty do testów
const obj1 = {};  // Obiekt literałowy
const obj2 = [];  // Tablica
const obj3 = new Date();  // Obiekt typu Date
const obj4 = new Map();  // Obiekt typu Map

// Sprawdzamy, do jakiego prototypu zbiegają wszystkie te obiekty
const lastProtoObj1 = getLastProto(obj1);
const lastProtoObj2 = getLastProto(obj2);
const lastProtoObj3 = getLastProto(obj3);
const lastProtoObj4 = getLastProto(obj4);


console.log("Last prototype of obj1:", lastProtoObj1);
console.log("Last prototype of obj2:", lastProtoObj2);
console.log("Last prototype of obj3:", lastProtoObj3);
console.log("Last prototype of obj4:", lastProtoObj4);


console.log("Czy lastProtoObj1 === lastProtoObj2?", lastProtoObj1 === lastProtoObj2);
console.log("Czy lastProtoObj1 === lastProtoObj3?", lastProtoObj1 === lastProtoObj3);
console.log("Czy lastProtoObj1 === lastProtoObj4?", lastProtoObj1 === lastProtoObj4);
