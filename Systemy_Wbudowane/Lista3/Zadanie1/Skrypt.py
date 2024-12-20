# Mapa liter do wartości nut
note_map = {
    'c': 'C', 'C': 'C_high', 'd': 'D', 'D': 'Dis', 'e': 'E',
    'f': 'F', 'F': 'Fis', 'g': 'G', 'G': 'Gis', 'a': 'A',
    'A': 'Ais', 'b': 'H', '-': '-'
}

# Dane wejściowe
data = """
4|c-d-D-c-d-D-f-d-A-g-G-f-g-|
4|D-f-c-c-d-D-c-d-D-f-d-A-g-|
4|G-f-g-D-f-c-c-d-D-c-d-D-f-|
4|d-A-g-G-f-g-D-f-c-c-d-D-c-|
4|d-D-f-d-A-g-G-f-g-D-g-c-c-|
4|d-D-c-d-D-f-d-A-g-G-f-g-D-|
4|f-c-c-d-D-c-d-D-f-d-A-g-G-|
4|f-g-D-f-c-c-d-D-c-d-D-f-d-|
4|A-g-G-f-g-D-f-c-c-d-D-c-d-|
5|---------------c------DDDD|
4|D-f-d-AgD---DgA-AgDcDg----|
3|---------AfA--------------|
5|ccDDDDccDDccDDDDccDDDDccDD|
5|ccggggDDggggDDggDDggggDDgg|
5|ggDDggDDffffccffffccffccff|
5|ffccffffccffcc---c--cd--c-|
4|--------------gaA-aA--A---|
5|f---DDDDccDDDDccDDccDDDDcc|
5|DDDDccDDccggggDDggggDDggDD|
5|ggggDDggggDDggDDffffccffff|
5|ccffccffffccffffccffcc--cd|
4|----------------------ab--|
5|edc---cga-----------------|
4|---bab----------a-a-g---g-|
4|f-g---a-------g-f-g---a-g-|
5|------------------------b-|
4|--f---d-------------------|
"""

# Przetwarzanie danych
notes = []
delays = []

# Przetwarzanie wierszy
for line in data.strip().splitlines():
    # Ignoruj numer oktawy i usuwaj pionowe linie
    line_notes = line.split('|')[1].replace('-', ' - ').strip()

    # Wskaźnik, aby śledzić długości pauz
    previous_delay = 1
    i = 0

    while i < len(line_notes):
        char = line_notes[i]

        if char == '-':
            # Jeśli to pauza, zwiększ ostatni delay
            if delays:
                delays[-1] += 1
            else:
                delays.append(2)
            i += 1
            continue

        # Sprawdź, czy jest to nuta złożona (np. "cc", "DD")
        if i + 1 < len(line_notes) and line_notes[i + 1] in note_map:
            # To może być nuta złożona
            note = char + line_notes[i + 1]
            i += 2
        else:
            note = char
            i += 1

        # Przekształć nutę na odpowiadający dźwięk
        if note in note_map:
            translated_note = note_map[note]
            notes.append(translated_note)
            delays.append(1)
        else:
            print(f"Nieznana nuta: {note}")

# Długość muzyki
MUSIC_LENGTH = len(notes)

# Tworzenie kodu C dla nut i opóźnień
notes_c_code = "static const uint32_t notes[MUSIC_LENGTH] PROGMEM = {\n   "
delays_c_code = "static const uint32_t delays[MUSIC_LENGTH] PROGMEM = {\n   "

notes_c_code += ', '.join(notes) + '\n};\n'
delays_c_code += ', '.join(map(str, delays)) + '\n};\n'

# Wyświetlanie wyników
print(f"#define MUSIC_LENGTH {MUSIC_LENGTH}\n")
print(notes_c_code)
print(delays_c_code)
