#include <MIDI.h>
#include <LiquidCrystal_I2C.h>
#include <LED.h>
#include "Button.h"

MIDI_CREATE_DEFAULT_INSTANCE();

// Zmienne do kontroli oszczędzania energii
unsigned long lastActivityTime = 0;
const unsigned long IDLE_TIMEOUT = 300000; // 5 minut (w milisekundach)
bool displayActive = true;

void debug(const char *str) {
  Serial.println(str);
}

// Definicja pojedynczego zdarzenia MIDI
struct MidiEvent {
  byte type;        // rodzaj komunikatu (0: CC, 1: Note On, 2: Program Change)
  byte number;      // numer kontrolera/nuty/programu
  byte value;       // wartość
  unsigned int delay; // opóźnienie przed następnym zdarzeniem (ms)
};

// Maksymalna długość sekwencji
#define MAX_SEQUENCE_LENGTH 10

// Struktura sekwencji dla przycisku
struct ButtonSequence {
  MidiEvent events[MAX_SEQUENCE_LENGTH];
  byte length; // aktualna długość sekwencji
};

enum Mode {
  Mode1,
  Mode2,
  Mode3,
  ModeCount
};

enum SwitchFnButton {
  SFB1,
  SFB2,
  SFB3,
  SFB4,
  SFB5,
  SwitchFnButtonCount
};

enum SwitchModeButton {
  SMBUp,
  SwitchModeButtonCount
};

int commandChannel = 9;
int PROGRAM_CHANGE_SHIFT = 1;

Mode currentMode;

int lastPC = -1;

// Tablica sekwencji dla wszystkich przycisków i trybów
ButtonSequence sequences[ModeCount][SwitchFnButtonCount];

Button commandBtns [SwitchFnButtonCount] =
{
  Button(8),
  Button(9),
  Button(10),
  Button(11),
  Button(12)
};

Button modeBtns [SwitchModeButtonCount] = {
  Button(13)
};

LED modeLEDs [ModeCount] = {
  LED(A0),
  LED(A1),
  LED(A2)
};

LiquidCrystal_I2C lcd(0x3F, 16, 2);

// Funkcja do włączania wyświetlacza i diod
void wakeUpDisplay() {
  if (!displayActive) {
    lcd.backlight();
    for (int i = 0; i < ModeCount; i++) {
      if (i == currentMode) {
        modeLEDs[i].on();
      }
    }
    displayActive = true;
    debug("Display activated");
  }
  
  // Aktualizacja czasu ostatniej aktywności
  lastActivityTime = millis();
}

// Funkcja do wygaszania wyświetlacza i diod
void sleepDisplay() {
  if (displayActive) {
    lcd.noBacklight();
    for (int i = 0; i < ModeCount; i++) {
      modeLEDs[i].off();
    }
    displayActive = false;
    debug("Display sleeping");
  }
}

// Funkcja do sprawdzania czasu bezczynności
void checkIdle() {
  unsigned long currentTime = millis();
  unsigned long idleTime = currentTime - lastActivityTime;
  
  if (displayActive && idleTime > IDLE_TIMEOUT) {
    sleepDisplay();
  }
}

// Funkcja do odtwarzania sekwencji MIDI
void playMidiSequence(int modeIndex, int buttonIndex) {
  // Aktywacja wyświetlacza przy każdym użyciu
  wakeUpDisplay();
  
  ButtonSequence &seq = sequences[modeIndex][buttonIndex];
  
  // Informacje diagnostyczne przez Serial
  Serial.print("Button pressed: ");
  Serial.print(buttonIndex);
  Serial.print(", Mode: ");
  Serial.print(modeIndex);
  Serial.print(", Sequence length: ");
  Serial.println(seq.length);
  
  // Wyświetl informacje diagnostyczne na LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Btn:");
  lcd.print(buttonIndex);
  lcd.print(" M:");
  lcd.print(modeIndex);
  lcd.print(" #:");
  lcd.print(seq.length);
  
  if(seq.length == 0) {
    // Jeśli sekwencja jest pusta, użyj starego zachowania
    int program = modeIndex * 5 + buttonIndex + PROGRAM_CHANGE_SHIFT;
    
    Serial.print("Sending default CC: ");
    Serial.print(program);
    Serial.print(", value: 127, channel: ");
    Serial.println(commandChannel);
    
    lcd.setCursor(0, 1);
    lcd.print("CC ");
    lcd.print(program);
    lcd.print(" val:127");
    
    MIDI.sendControlChange(program, 127, commandChannel);
    lastPC = program;
  } else {
    // Odtwórz sekwencję zdarzeń
    for(int i = 0; i < seq.length; i++) {
      MidiEvent &event = seq.events[i];
      
      Serial.print("Event ");
      Serial.print(i+1);
      Serial.print("/");
      Serial.print(seq.length);
      Serial.print(": Type=");
      
      // Wyświetl info o aktualnym zdarzeniu
      lcd.setCursor(0, 1);
      switch(event.type) {
        case 0:
          lcd.print("CC  ");
          Serial.print("CC");
          break;
        case 1:
          lcd.print("Note");
          Serial.print("Note");
          break;
        case 2:
          lcd.print("PC  ");
          Serial.print("PC");
          break;
      }
      
      Serial.print(", Number=");
      Serial.print(event.number);
      Serial.print(", Value=");
      Serial.print(event.value);
      Serial.print(", Delay=");
      Serial.println(event.delay);
      
      lcd.print(event.number);
      lcd.print(" val:");
      lcd.print(event.value);
      lcd.print(" ");
      lcd.print(i+1);
      lcd.print("/");
      lcd.print(seq.length);
      
      // Wysyłanie zdarzenia MIDI zależnie od typu
      switch(event.type) {
        case 0: // Control Change
          MIDI.sendControlChange(event.number, event.value, commandChannel);
          break;
        case 1: // Note On
          MIDI.sendNoteOn(event.number, event.value, commandChannel);
          break;
        case 2: // Program Change
          MIDI.sendProgramChange(event.number, commandChannel);
          break;
      }
      
      // Zapisz ostatni numer PC dla wyświetlacza
      if(i == 0) {
        lastPC = event.number;
      }
      
      // Czekaj przez określony czas jeśli to nie ostatnie zdarzenie
      if (i < seq.length - 1 && event.delay > 0) {
        delay(event.delay);
      }
    }
  }
  
  debug("playMidiSequence complete");
}

// Inicjalizacja przykładowych sekwencji MIDI
void setupMidiSequences() {
  // Tryb 0, Przycisk 0 - sekwencja 3 zdarzenia (Program Change + nuta E4 + wyłączenie nuty)
  sequences[0][0].length = 1;
  // Zdarzenie 1: Program Change
  sequences[0][0].events[0].type = 2; // Program Change
  sequences[0][0].events[0].number = 76; // Program 77 (liczymy od )
  sequences[0][0].events[0].value = 0; // nie używane dla PC
  sequences[0][0].events[0].delay = 0;
  
  // Tryb 0, Przycisk 1 - sekwencja 3 zdarzenia (Program Change + nuta E4 + wyłączenie nuty)
  sequences[0][1].length = 1;
  // Zdarzenie 1: Program Change
  sequences[0][1].events[0].type = 2; // Program Change
  sequences[0][1].events[0].number = 77; // Program 78 (o jeden wyżej)
  sequences[0][1].events[0].value = 0; // nie używane dla PC
  sequences[0][1].events[0].delay = 0;
  

  // Tryb 0, Przycisk 1 - sekwencja 3 zdarzenia (Program Change + nuta E4 + wyłączenie nuty)
  sequences[0][2].length = 1;
  // Zdarzenie 1: Program Change
  sequences[0][2].events[0].type = 2; // Program Change
  sequences[0][2].events[0].number = 78; // Program 79 (o jeden wyżej)
  sequences[0][2].events[0].value = 0; // nie używane dla PC
  sequences[0][2].events[0].delay = 0;

    // Tryb 0, Przycisk 1 - sekwencja 3 zdarzenia (Program Change + nuta E4 + wyłączenie nuty)
  sequences[0][3].length = 1;
  // Zdarzenie 1: Program Change
  sequences[0][3].events[0].type = 2; // Program Change
  sequences[0][3].events[0].number = 79; // Program 80 (o jeden wyżej)
  sequences[0][3].events[0].value = 0; // nie używane dla PC
  sequences[0][3].events[0].delay = 0;


  sequences[0][4].length = 1;
  // Zdarzenie 1: Program Change
  sequences[0][4].events[0].type = 2; // Program Change
  sequences[0][4].events[0].number = 126; // Program 127 (o jeden wyżej)
  sequences[0][4].events[0].value = 0; // nie używane dla PC
  sequences[0][4].events[0].delay = 0;

  // Pozostałe sekwencje można zostawić puste (length = 0)
  // Wtedy zostanie użyte domyślne zachowanie
}

// Handle button which depending on its id, loop number/functions set sends connected midi message
void handleSwitchFnButtonPressed (Button &button, int buttonId) {
  wakeUpDisplay(); // Aktywacja wyświetlacza przy naciśnięciu przycisku
  playMidiSequence(currentMode, buttonId);
  debug("handleSwitchFnButtonPressed");
}

void handleSwitchFnButton1Pressed (Button &button) {
  handleSwitchFnButtonPressed(button, SFB1);
}

void handleSwitchFnButton2Pressed (Button &button) {
  handleSwitchFnButtonPressed(button, SFB2);
}

void handleSwitchFnButton3Pressed (Button &button) {
  handleSwitchFnButtonPressed(button, SFB3);
}

void handleSwitchFnButton4Pressed (Button &button) {
  handleSwitchFnButtonPressed(button, SFB4);
}


void handleSwitchFnButton5Pressed (Button &button) {
  handleSwitchFnButtonPressed(button, SFB5);
}

// If Up/Down button was released after 1 second
// - restore previous state
// - change mode (Function -> Loop or Loop -> Function)
void handleBtnUpDown(Button &, int delta) {
  wakeUpDisplay(); // Aktywacja wyświetlacza przy naciśnięciu przycisku
  
  int m = currentMode;
  m += delta;
  if (m == ModeCount) {
    m = 0;
  }
  else if (m < 0) {
    m = ModeCount - 1;
  }
  modeLEDs[currentMode].toggle();
  currentMode = (Mode) m;
  modeLEDs[currentMode].toggle();
  debug("handleHold");
}

void handleBtnUp(Button &button) {
  handleBtnUpDown(button, +1) ;
}

void handleBtnDown(Button &button) {
  handleBtnUpDown(button, -1) ;
}

void setup() {
  Serial.begin(115200); // Inicjalizacja komunikacji szeregowej z wysoką prędkością
  Serial.println("Big Foot MIDI Controller starting...");
  
  MIDI.begin(MIDI_CHANNEL_OMNI);
  modeLEDs[Mode1].on();

  currentMode = Mode1;
  
  // Ustawienie kanału MIDI na 15 (0-based to 14 w bibliotece MIDI)
  commandChannel = 15;
  
  // Inicjalizacja sekwencji MIDI
  setupMidiSequences();

  // set up the LCD's number of columns and rows:
  lcd.init();                      // initialize the lcd
  lcd.backlight();
  // Print a message to the LCD.
  lcd.setCursor(0, 0);
  lcd.print("SooperLooper &");
  lcd.setCursor(0, 1);
  lcd.print("Hydrogen Controller");

  // Podłącz obsługę przycisków
  commandBtns [SFB1].pressHandler(handleSwitchFnButton1Pressed);
  commandBtns [SFB2].pressHandler(handleSwitchFnButton2Pressed);
  commandBtns [SFB3].pressHandler(handleSwitchFnButton3Pressed);
  commandBtns [SFB4].pressHandler(handleSwitchFnButton4Pressed);
  commandBtns [SFB5].pressHandler(handleSwitchFnButton5Pressed);

  modeBtns [SMBUp].pressHandler(handleBtnUp);
  
  // Inicjalizacja czasu ostatniej aktywności
  lastActivityTime = millis();
  
  Serial.println("Setup complete");
}

void loop() {
  // Sprawdź czas bezczynności
  checkIdle();

  // Sprawdź wszystkie przyciski funkcyjne
  for (int f = 0; f < SwitchFnButtonCount; f++) {
    if (commandBtns[f].uniquePress()) {
      wakeUpDisplay(); // Aktywacja wyświetlacza przy naciśnięciu przycisku
    }
  }
  
  // Sprawdź przyciski zmiany trybu
  for (int f = 0; f < SwitchModeButtonCount; f++) {
    if (modeBtns[f].uniquePress()) {
      wakeUpDisplay(); // Aktywacja wyświetlacza przy naciśnięciu przycisku
    }
  }
  
  delay(10); // Krótki delay
}
