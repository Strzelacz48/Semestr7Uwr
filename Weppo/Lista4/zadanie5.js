function Foo() {
    // Definiujemy prywatną funkcję Qux wewnątrz konstruktora
    function Qux() {
        console.log("Foo::Qux (private function)");
    }

    // Dodajemy metodę Bar do instancji Foo
    this.Bar = function() {
        console.log("Foo::Bar (public method)");
        // Wywołanie prywatnej funkcji Qux wewnątrz publicznej metody Bar
        Qux();
    };
}

// Tworzymy nową instancję Foo
var foo = new Foo();
foo.Bar(); // Działa poprawnie i wywołuje Qux wewnątrz Bar

// Próba bezpośredniego wywołania Qux
console.log(typeof foo.Qux); // undefined - brak dostępu do prywatnej funkcji

