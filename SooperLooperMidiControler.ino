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
  /*FunctionMode,
    LoopMode,
    DrumMode,*/
  Mode1,
  Mode2,
  Mode3,
  ModeCount
};

//
//enum Loops {
//  Selected,
//  Loop1,
//  Loop2,
//  Loop3,
//  /*
//    Loop4,
//    Loop5,
//    Loop6,
//    Loop7,
//    Loop8,
//  */
//  LoopCount
//};

//
//enum LoopFunctionsSet {
//  LFS1,
//  LFS2,
//  LFS3,
//  LFS4,
//  LoopFunctionSetCount
//};
//
//enum DrumCommandSet {
//  DCS1,
//  DrumCommandSetCount
//};

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
  // SMBDown,
  SwitchModeButtonCount
};
//
//enum LooperCommands {
//  recordCmd,
//  overdubCmd,
//  muteCmd,
//  undoCmd,
//
//  insertCmd,
//  replaceCmd,
//  reverseCmd,
//  redoCmd,
//
//  redoAllCmd,
//  multiplyCmd,
//  oneshotCmd,
//  pauseCmd,
//
//  soloCmd,
//  substituteCmd,
//  delayCmd,
//  triggerCmd,
//
//  LooperCommandsCount
//};

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

//
//enum DrumCommands {
//  playDrumCmd,
//  pat0DrumCmd,
//  pat1DrumCmd,
//  pat2DrumCmd,
//  DrumCommandsCount
//};
//
//
//char *drumCommandsDesc [DrumCommandsCount] = {
//  "pla",
//  "p0",
//  "p1",
//  "p2"
//};

//char *PrgChange = "PC";
//char *CtrlChange = "CC";
int commandChannel = 9;
//int drumCommandChannel = 10;
int PROGRAM_CHANGE_SHIFT = 1;

//int PROGRAM_CHANGE_OFFSET_FOR_LOOP_SELECTION = 100;

// What up/down buttons change?
Mode currentMode;

int lastPC = -1;

// On which loop we work
//Loops currentLoop;

// On which function set
//LoopFunctionsSet currentFunctionSet;

// Current drum command set
//DrumCommandSet currentDrumCommandSet;

// Memory of previous loop
//Loops _lastLoop;

// Memory of function set
//LoopFunctionsSet _lastFunctionSet;

// Memory of drum command set
//DrumCommandSet _lastDrumCommandSet;


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
  //  _lastLoop = currentLoop;
  //  _lastFunctionSet = currentFunctionSet;
  //  _lastDrumCommandSet = currentDrumCommandSet;
}

/*

  // Handle Switch Up button (change loop or loop function set or global function set)
  void handleSwitchUp (Button &b) {
  saveState ();
  switch (currentMode) {
    case FunctionMode :
      {
        int s = currentFunctionSet;
        s++;
        if ( s == LoopFunctionSetCount ) {
          s = 0;
        }
        currentFunctionSet = (LoopFunctionsSet) s;
        break;
      }
    case LoopMode:
      {
        int c = currentLoop;
        c++;
        if ( c == LoopCount) {
          c = 0;
        }
        currentLoop = (Loops) c;
        break;
      }
    case  DrumMode:
      {
        int d = currentDrumCommandSet;
        d++;
        if ( d == DrumCommandSetCount) {
          d = 0;
        }
        currentDrumCommandSet = (DrumCommandSet) d;
        break;
      }
  }
  refreshLCD();
  debug("handleSwitchUp");
  }
*/

/*
  // Handle Switch Down button (change loop or loop function set or global function set)
  void handleSwitchDown (Button &b) {
  saveState ();
  switch (currentMode) {
    case FunctionMode :
      {
        int s = currentFunctionSet;
        if ( s > 0 ) {
          s--;
        }
        else {
          s = LoopFunctionSetCount - 1;
        }
        currentFunctionSet = (LoopFunctionsSet) s;
        break;
      }
    case LoopMode:
      {
        int c = currentLoop;
        if ( c > 0) {
          c--;
        }
        else {
          c = LoopCount - 1;
        }
        currentLoop = (Loops) c;
        break;
      }
    case  DrumMode:
      {
        int d = currentDrumCommandSet;
        if ( d > 0 ) {
          d--;
        }
        else {
          d = DrumCommandSetCount - 1;
        }
        currentDrumCommandSet = (DrumCommandSet) d;
        break;
      }
  }
  refreshLCD();
  debug("handleSwitchDown");
  }
*/
/*
  void displayCommand(int channel, char *command, int p1, int p2) {
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(command);

  lcd.setCursor(8, 0);
  lcd.print("CH :"); lcd.print(channel);

  lcd.setCursor(0, 1);
  lcd.print("P1 :");  lcd.print(p1);
  lcd.setCursor(8, 1);
  lcd.print("P2 :");  lcd.print(p2);

  }
*/

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


  /*
    switch (currentMode) {
    case LoopMode:
      {
        lcd.print("F");
        lcd.print(currentFunctionSet);
        lcd.print("  Loops("); lcd.print(currentLoop);
        lcd.print(")  D");
        lcd.print(currentDrumCommandSet);

        for (int i = 0; i < SwitchFnButtonCount; i++) {
          lcd.setCursor(i * 4, 1);
          lcd.print("L");
          lcd.print(i);
        }
        break;
      }
    case FunctionMode:
      {
        //lcd.print("Func");
        lcd.print("Funcs(");
        lcd.print(currentFunctionSet);
        lcd.print(")  L"); lcd.print(currentLoop);
        lcd.print("  D");
        lcd.print(currentDrumCommandSet);
        for (int i = 0; i < SwitchFnButtonCount; i++) {
          lcd.setCursor(i * 4, 1);
          lcd.print(commandDesc[currentFunctionSet * SwitchFnButtonCount + i]);
        }
        break;
      }
    case DrumMode:
      {
        //lcd.print("Drum");
        lcd.print("F");
        lcd.print(currentFunctionSet);
        lcd.print("  L"); lcd.print(currentLoop);
        lcd.print("  Drums(");
        lcd.print(currentDrumCommandSet);
        lcd.print(")");
        for (int i = 0; i < SwitchFnButtonCount; i++) {
          lcd.setCursor(i * 4, 1);
          lcd.print(drumCommandsDesc[currentDrumCommandSet * SwitchFnButtonCount + i]);
        }
        break;
      }
    }
  */
}
// Handle button which depending on its id, loop number/functions set sends connected midi message
void handleSwitchFnButtonPressed (Button &button, int buttonId) {

  int program = //currentLoop * LooperCommandsCount +
    //currentFunctionSet * LoopFunctionSetCount +
    currentMode * 5 +
     buttonId + PROGRAM_CHANGE_SHIFT;
  //MIDI.sendProgramChange (program, commandChannel);

  //MIDI.sendNoteOn(program,127,commandChannel);
  MIDI.sendControlChange(program, 127,commandChannel);
                             
  //displayCommand(commandChannel, PrgChange, program, 0);
  lastPC = program;
  
  refreshLCD();
  //        break;

  /*

    switch (currentMode) {
      case FunctionMode :
        {
          int program = currentLoop * LooperCommandsCount +
                        currentFunctionSet * LoopFunctionSetCount +
                        buttonId;
          MIDI.sendProgramChange (program, commandChannel);
          displayCommand(commandChannel, PrgChange, program, 0);
          break;
        }
      case LoopMode:
        {
          currentLoop = (Loops) buttonId;
          int program = PROGRAM_CHANGE_OFFSET_FOR_LOOP_SELECTION + currentLoop;
          MIDI.sendProgramChange (program, commandChannel);
          displayCommand(commandChannel, PrgChange, program, 0);

          break;
        }
      case  DrumMode:
        {
          int ctrlNumber = currentDrumCommandSet * SwitchFnButtonCount + buttonId;
          int ctrlValue = 0;
          MIDI.sendControlChange (ctrlNumber, ctrlValue, drumCommandChannel);
          displayCommand(drumCommandChannel, CtrlChange, ctrlNumber, ctrlValue);

          break;
        }
    }*/
  // refreshLCD();
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
  //  currentLoop = _lastLoop;
  //  currentFunctionSet = _lastFunctionSet;
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

  //  currentDrumCommandSet = ( DrumCommandSet) 0;
  //  currentFunctionSet = (LoopFunctionsSet) 0;
  //  currentLoop = (Loops) 0;
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

  //modeBtns [SMBUp].pressHandler(handleSwitchUp);
  //modeBtns [SMBUp].holdHandler(handleHoldUp, 1000);

  modeBtns [SMBUp].pressHandler(handleHoldUp);
  //modeBtns [SMBDown].pressHandler(handleSwitchDown);
  //modeBtns [SMBDown].holdHandler(handleHoldDown, 1000);
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
