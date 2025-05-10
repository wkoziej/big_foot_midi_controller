//#include <LiquidCrystal.h>

#include <MIDI.h>
#include <LiquidCrystal_I2C.h>
#include <LED.h>
#include "Button.h"

MIDI_CREATE_DEFAULT_INSTANCE();

void debug(const char *str) {
  //USE_SERIAL_PORT.println(str);
}

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

// Handle button which depending on its id, loop number/functions set sends connected midi message
void handleSwitchFnButtonPressed (Button &button, int buttonId) {

  int program = currentMode * 5 +
     buttonId + PROGRAM_CHANGE_SHIFT;
  MIDI.sendControlChange(program, 127,commandChannel);
                             
  lastPC = program;
  
  refreshLCD();
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

  MIDI.begin(9);
  modeLEDs[Mode1].on();

  currentMode = Mode1;

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
