class Foo {
    // Definicja prywatnej metody z prefiksem #
    #Qux() { 
        console.log("Foo::Qux (private function)");
    }

    // Publiczna metoda
    Bar() {
        console.log("Foo::Bar (public method)");
        this.#Qux(); // Wywołanie prywatnej metody wewnątrz publicznej metody
    }
}

const foo = new Foo();
foo.Bar(); // Działa, wywołuje Foo::Bar, a następnie Foo::Qux

// Próba dostępu do prywatnej metody z zewnątrz
console.log(typeof foo.#Qux); // Błąd składni - prywatne pole dostępne tylko wewnątrz klasy
