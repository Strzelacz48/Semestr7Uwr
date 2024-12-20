function createFs(n) {
  var fs = [];
  for (var i = 0; i < n; i++) {
    //mozna miec oryginał tylko zmienic ten var na let
    fs[i] = (function (currentI) {
      // funkcja tworząca nowe domknięcie
      return function () {
        return currentI;
      };
    })(i); // przekazujemy bieżące `i` do funkcji, tworząc unikalny kontekst
  }
  return fs;
}

var myfs = createFs(10);
console.log(myfs[0]()); // 0
console.log(myfs[2]()); // 2
console.log(myfs[7]()); // 7
