#include <Arduino.h>
#include <USBComposite.h>

// DEFINE OPERATING SYSTEM HERE
#define Windows
///////////////////////////////

#ifdef Windows
  #define CTRLKEY 0x80
  #define ALTKEY 0x82
  #define SHIFTKEY 0x81
#endif
#ifdef OSX
  #define CTRLKEY 0x83
  #define ALTKEY 0x82
  #define SHIFTKEY 0x81
#endif

//Define Key Pins
#define KeyL1 PA0
#define KeyL2 PA1
#define KeyL3 PA2
#define KeyL4 PA3
#define KeyC1 PA5
#define KeyC2 PA4
#define KeyC3 PA6
#define KeyC4 PA7
#define KeyR1 PB11
#define KeyR2 PB10
#define KeyR3 PB1
#define KeyR4 PB0

#define Debounce_Delay 50

USBHID HID;
HIDKeyboard Keyboard(HID);
//                          Media     TrimStart keyEdit   Audio-    Edit      SplitClip CurveEdit Audio+    Colour    TrimEnd   AddTrans  #SHIFT MODIFIER                                            
int KeyMap[12] =            {KeyL1,    KeyL2,    KeyL3,    KeyL4,    KeyC1,    KeyC2,    KeyC3,    KeyC4,    KeyR1,    KeyR2,    KeyR3,    KeyR4};
uint8_t KeyCodes[12] =      {50,       91,       99,       45,       52,       92,       99,       43,       54,       93,       116,      32   };
uint8_t KeyMod1[12] =       {SHIFTKEY, SHIFTKEY, CTRLKEY,  CTRLKEY,  SHIFTKEY, CTRLKEY,  SHIFTKEY, CTRLKEY,  SHIFTKEY, SHIFTKEY, CTRLKEY,  0    };
uint8_t KeyMod2[12] =       {0,        0,        SHIFTKEY, ALTKEY,   0,        0,        0,        SHIFTKEY, 0,        0,        0,        0    };

uint8_t AltKeyCodes[12] =   {97,       98,       99,       100,      101,      102,      103,      104,      105,      106,      107,      108  };
uint8_t AltKeyMod1[12] =    {0,        0,        0,        0,        0,        0,        0,        0,        0,        0,        0,        0    };
uint8_t AltKeyMod2[12] =    {0,        0,        0,        0,        0,        0,        0,        0,        0,        0,        0,        0    };

uint32 KeyState[12];
uint32 KeyRollover[12];
uint32 KeyPrevious[12];

void callKeyCode(uint8_t KeyCode, uint8_t Modifier1, uint8_t Modifier2) {
  if(Modifier1 != 0) {
    Keyboard.press(Modifier1);
    if(Modifier2 != 0) {
      Keyboard.press(Modifier2);
    }
  }
  Keyboard.press(KeyCode);
  Keyboard.releaseAll();
}


void setup() {
  pinMode(PA0, INPUT_PULLUP);
  
  for(int i=0;i<12;i++) {
    pinMode(KeyMap[i], INPUT_PULLUP);
    KeyState[i] = millis();
    KeyRollover[i] = millis();
    KeyPrevious[i] = HIGH;
  }
  HID.begin(HID_KEYBOARD);
  Keyboard.begin();
  delay(1000);
}

void loop() {
  for(int i=0;i<11;i++) {
    uint32 State = digitalRead(KeyMap[i]);
    if(KeyPrevious[i] != State) {
      KeyRollover[i] = millis();
      if(State == LOW) {
        if(millis() - KeyState[i] >= Debounce_Delay) {
          if(digitalRead(KeyMap[11]) == HIGH) {
            callKeyCode(KeyCodes[i], KeyMod1[i], KeyMod2[i]);
          }
          else {
            callKeyCode(AltKeyCodes[i], AltKeyMod1[i], AltKeyMod2[i]);
          }
          KeyState[i] = millis();
        }
      }
    }
    if(State == LOW) {
      if(millis() - KeyRollover[i] >= 1000) {
        if(KeyMod1[i] == 0) {
          if(digitalRead(KeyMap[11]) == HIGH) {
            callKeyCode(KeyCodes[i], KeyMod1[i], KeyMod2[i]);
          }
          else {
            callKeyCode(AltKeyCodes[i], AltKeyMod1[i], AltKeyMod2[i]);
          }
        }
      }
    }
    else {
      KeyRollover[i] = millis();
    }
    KeyPrevious[i] = digitalRead(KeyMap[i]);
  }
}