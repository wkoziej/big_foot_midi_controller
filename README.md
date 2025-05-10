# SooperLooperMidiControler

Kontroler MIDI dla aplikacji SooperLooper i Hydrogen, działający na Arduino Uno.

## Opis projektu

Projekt umożliwia sterowanie aplikacjami SooperLooper i Hydrogen poprzez interfejs MIDI. Za pomocą przycisków można wysyłać komendy MIDI do aplikacji oraz zmieniać tryby pracy kontrolera.

## Wymagane biblioteki

Projekt wykorzystuje następujące biblioteki Arduino:
- MIDI Library
- Button
- LED
- LiquidCrystal_I2C

## Instalacja bibliotek

### Metoda 1: Przez Arduino IDE

1. Otwórz Arduino IDE
2. Przejdź do menu "Szkic" > "Dołącz bibliotekę" > "Dodaj bibliotekę .ZIP"
3. Znajdź i wybierz pliki ZIP zawarte w repozytorium:
   - MIDI_Library.zip
   - Button-master.zip
   - LED.zip
   - LiquidCrystal_I2C-master.zip
4. Arduino IDE automatycznie rozpakuje i zainstaluje te biblioteki

### Metoda 2: Ręczna instalacja

1. Utwórz katalog bibliotek Arduino, jeśli jeszcze nie istnieje:
   ```
   mkdir -p ~/Arduino/libraries/
   ```
   
2. Rozpakuj pliki ZIP do katalogu bibliotek:
   ```
   unzip Button-master.zip -d ~/Arduino/libraries/
   unzip LED.zip -d ~/Arduino/libraries/
   unzip LiquidCrystal_I2C-master.zip -d ~/Arduino/libraries/
   unzip MIDI_Library.zip -d ~/Arduino/libraries/
   ```

## Wgrywanie programu

1. Otwórz plik SooperLooperMidiControler.ino w Arduino IDE
2. Wybierz płytkę "Arduino Uno" z menu Narzędzia > Płytka
3. Wybierz port USB, do którego podłączone jest Arduino
4. Kliknij przycisk "Wgraj"

## Rozwiązywanie problemów

Jeśli masz problemy z wgrywaniem programu:
- Upewnij się, że wybrano właściwą płytkę i port w Arduino IDE
- Sprawdź czy biblioteki zostały poprawnie zainstalowane
- Upewnij się, że kabel USB jest podłączony prawidłowo
- Spróbuj zresetować Arduino tuż przed wgrywaniem
- Sprawdź uprawnienia portu:
  ```
  sudo usermod -a -G dialout $USER
  sudo chmod a+rw /dev/ttyACM0
  ```

## Schemat połączeń

- Przyciski funkcyjne: piny 8, 9, 10, 11, 12
- Przycisk zmiany trybu: pin 13
- Diody LED trybów: piny A0, A1, A2
- Wyświetlacz LCD I2C: piny SDA i SCL (A4 i A5 na Arduino Uno) 