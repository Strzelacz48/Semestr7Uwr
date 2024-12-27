#include <stdio.h>
#include <ctype.h>

// Funkcja do przekształcania znaku na alfabet Morse'a
void translateToMorse(char character) {
    // Tablica znaków Morse'a odpowiadająca literom i cyfrom
    const char *morseCode[] = {
        ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", "-.-",
        ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-", "..-", "...-",
        ".--", "-..-", "-.--", "--..",  // litery od A do Z
        "-----", ".----", "..---", "...--", "....-", ".....", "-....", "--...", "---..", "----."  // cyfry 0-9
    };

    if (isalpha(character)) { // jeśli znak jest literą
        int index = toupper(character) - 'A'; // znajdź indeks (A=0, B=1, ...)
        printf("%s ", morseCode[index]);
    } else if (isdigit(character)) { // jeśli znak jest cyfrą
        int index = character - '0' + 26; // przesuń indeks na zakres cyfr (0=26, 1=27, ...)
        printf("%s ", morseCode[index]);
    } else if (character == ' ') { // jeśli znak jest spacją
        printf("   "); // 3 spacje oznaczają odstęp między słowami
    } else {
        printf("? "); // znak nieznany
    }
}

int main() {
    char text[100];

    printf("Podaj tekst do przetłumaczenia na Morse'a: ");
    fgets(text, sizeof(text), stdin); // odczytanie ciągu znaków od użytkownika

    printf("Tekst w alfabecie Morse'a: ");
    for (int i = 0; text[i] != '\0'; i++) {
        translateToMorse(text[i]);
    }

    printf("\n");
    return 0;
}
