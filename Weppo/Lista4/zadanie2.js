function isOwnProperty(obj, property) {
    return obj.hasOwnProperty(property);
}

// Tworzymy obiekty p i q, oraz ustawiamy q jako prototyp dla p
var p = {
    name: 'jan'
};
var q = {
    surname: 'kowalski'
};
Object.setPrototypeOf(p, q);

console.log("Czy 'name' jest bezpośrednio w obiekcie p?", isOwnProperty(p, 'name')); // true
console.log("Czy 'surname' jest bezpośrednio w obiekcie p?", isOwnProperty(p, 'surname')); // false


function listOwnProperties(obj) {
    return Object.keys(obj); // lub Object.getOwnPropertyNames(obj)
}

console.log("Własne składowe obiektu p:", listOwnProperties(p)); // ['name']

function listAllProperties(obj) {
    let allProperties = [];
    for (let prop in obj) {
        allProperties.push(prop);
    }
    return allProperties;
}

console.log("Wszystkie składowe obiektu p i jego prototypów:", listAllProperties(p)); // ['name', 'surname']
