#include <MIDI.h>
#include <LiquidCrystal_I2C.h>
#include <LED.h>
#include "Button.h"

MIDI_CREATE_DEFAULT_INSTANCE();

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

const char *commandDesc [SwitchFnButtonCount * ModeCount] = {
  "RC",
  "OV",
  "MU",
  "RS",
  "RV",
  
  "A",
  "B",
  "C",
  "D",
  "E",
  
  "F",
  "G",
  "H",
  "I",
  "J" 
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

void saveState() {
}

void refreshLCD() {
  lcd.clear();
    
  if (lastPC >= 0) {
  lcd.setCursor(0, 0);
    lcd.print("P");
    lcd.print(lastPC);
    
    lcd.setCursor(6, 0);
    lcd.print(commandDesc[lastPC - PROGRAM_CHANGE_SHIFT]);
  }

  lcd.setCursor(12, 0);

  lcd.print("M");
  lcd.print(currentMode);

  lcd.print("C");
  lcd.print(commandChannel);


  for (int i = 0; i < SwitchFnButtonCount; i++) {
    lcd.setCursor(i * 3, 1);
    lcd.print(commandDesc[currentMode * SwitchFnButtonCount + i]);
  }
}

// Funkcja do odtwarzania sekwencji MIDI
void playMidiSequence(int modeIndex, int buttonIndex) {
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
  
  // Odczekaj chwilę, żeby informacje były widoczne
  delay(1000);
  
  // Przywróć standardowy widok ekranu
  refreshLCD();
  debug("playMidiSequence complete");
}

// Inicjalizacja przykładowych sekwencji MIDI
void setupMidiSequences() {
  // Tryb 0, Przycisk 0 - sekwencja 2 zdarzeń
  sequences[0][0].length = 2;
  // Wydarzenie 1: Control Change
  sequences[0][0].events[0].type = 0;
  sequences[0][0].events[0].number = 1;
  sequences[0][0].events[0].value = 127;
  sequences[0][0].events[0].delay = 100;
  // Wydarzenie 2: Note On
  sequences[0][0].events[1].type = 1;
  sequences[0][0].events[1].number = 60; // nuta C4
  sequences[0][0].events[1].value = 100; // velocity
  sequences[0][0].events[1].delay = 0;
  
  // Tryb 0, Przycisk 1 - sekwencja 3 zdarzeń
  sequences[0][1].length = 3;
  sequences[0][1].events[0].type = 0;
  sequences[0][1].events[0].number = 2;
  sequences[0][1].events[0].value = 127;
  sequences[0][1].events[0].delay = 50;
  sequences[0][1].events[1].type = 1;
  sequences[0][1].events[1].number = 62; // nuta D4
  sequences[0][1].events[1].value = 100;
  sequences[0][1].events[1].delay = 50;
  sequences[0][1].events[2].type = 1;
  sequences[0][1].events[2].number = 64; // nuta E4
  sequences[0][1].events[2].value = 100;
  sequences[0][1].events[2].delay = 0;
  
  // Tryb 1, Przycisk 0 - sekwencja 1 zdarzenie
  sequences[1][0].length = 1;
  sequences[1][0].events[0].type = 2; // Program Change
  sequences[1][0].events[0].number = 10;
  sequences[1][0].events[0].value = 0; // nie używane dla PC
  sequences[1][0].events[0].delay = 0;
  
  // Pozostałe sekwencje można zostawić puste (length = 0)
  // Wtedy zostanie użyte domyślne zachowanie
}

// Handle button which depending on its id, loop number/functions set sends connected midi message
void handleSwitchFnButtonPressed (Button &button, int buttonId) {
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
void handleHold(Button &, int delta) {
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
  refreshLCD();
  debug("handleHold");
}

void handleHoldUp(Button &button) {
  handleHold(button, +1) ;
}

void handleHoldDown(Button &button) {
  handleHold(button, -1) ;
}

void setup() {
  Serial.begin(115200); // Inicjalizacja komunikacji szeregowej z wysoką prędkością
  Serial.println("Big Foot MIDI Controller starting...");
  
  MIDI.begin(9);
  modeLEDs[Mode1].on();

  currentMode = Mode1;
  
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

  commandBtns [SFB1].pressHandler(handleSwitchFnButton1Pressed);
  commandBtns [SFB2].pressHandler(handleSwitchFnButton2Pressed);
  commandBtns [SFB3].pressHandler(handleSwitchFnButton3Pressed);
  commandBtns [SFB4].pressHandler(handleSwitchFnButton4Pressed);
  commandBtns [SFB5].pressHandler(handleSwitchFnButton5Pressed);

  modeBtns [SMBUp].pressHandler(handleHoldUp);
  
  Serial.println("Setup complete");
}

void loop() {

  int f = 0;
  for (; f < SwitchFnButtonCount; f++) {
    if ( commandBtns[f].uniquePress() ) {
    }
  }
  f = 0;
  for (; f < SwitchModeButtonCount; f++) {
    if ( modeBtns[f].uniquePress() ) {
    }
  }

}
