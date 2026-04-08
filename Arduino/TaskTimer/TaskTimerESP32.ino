// Task Timer V2 for ESP32. 
// After arduino Uno prototype, this script converts the script to run on an ESP32 board for a smaller form factor for the prototype device.
// Also includes a lot of logic and timing optimization to account for the less powerful ESP32 to prevent flicker and delays in operation.

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pin Definitions for ESP32
const int xPin = 35;
const int yPin = 34;
const int swPin = 23;
int latchPin = 33;
int dataPin = 25;
int clockPin = 32;
const int digitPins[4] = { 13, 14, 27, 26 };

// Common Anode bit Map for displaying the digits
const byte digitMap[] = {
  0b00000011, 0b10011111, 0b00100101, 0b00001101, 0b10011001,
  0b01001001, 0b01000001, 0b00011111, 0b00000001, 0b00001001,
  0b10010001 // Index 10: "H"
};

// Timing and State
unsigned long previousMillis = 0;
unsigned long lastLogicUpdate = 0;
const int interval = 1000;
int menuScreen = 0;
bool loaded = false;
int taskSelected = 0;

// Global Digit Storage for 7-Seg
int d[4] = {0, 0, 0, 0};
bool globalColon = false;

struct Task {
  String taskName;
  long targetTime;
  long timeSpent;
};
Task taskArray[3];
int numTasks = 2;

// Debounce variables
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 200; 

void setup() {
  Serial.begin(115200); // ESP32 standard is 115200
  Wire.setClock(400000); // Speed up I2C for the LCD
  
  lcd.init();
  lcd.backlight();

  pinMode(swPin, INPUT_PULLUP);
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  for (int i = 0; i < 4; i++) {
    pinMode(digitPins[i], OUTPUT);
    digitalWrite(digitPins[i], HIGH);
  }

  taskArray[0] = {"Task Timer", 3600000, 0};
  taskArray[1] = {"Edit Photos", 3600000, 0};
  taskArray[2] = {"Website", 3600000, 0};
}

void loop() {
  unsigned long currentMillis = millis();

  // 1. REFRESH 7-SEGMENT (One digit per loop for maximum speed)
  long remaining = (taskArray[taskSelected].targetTime - taskArray[taskSelected].timeSpent) / 1000;
  if (remaining < 0) remaining = 0;
  updateDigitBuffer((int)remaining);
  refreshSevenSegment();

  // 2. RUN LOGIC (Every 50ms to keep I2C bus clear)
  if (currentMillis - lastLogicUpdate > 50) {
    lastLogicUpdate = currentMillis;
    handleInputsAndMenu();
  }

  yield(); // Required for ESP32 background stability
}

// Pre-calculates the digits so the refresh function is lightning fast
void updateDigitBuffer(int seconds) {
  if (seconds > 3600) {
    d[0] = seconds / 3600;
    d[1] = 10; // "H"
    int mins = (seconds % 3600) / 60;
    d[2] = mins / 10;
    d[3] = mins % 10;
    globalColon = false;
  } else {
    int mins = seconds / 60;
    int secs = seconds % 60;
    d[0] = mins / 10;
    d[1] = mins % 10;
    d[2] = secs / 10;
    d[3] = secs % 10;
    globalColon = true;
  }
}

// Cycles through one digit each time it is called
void refreshSevenSegment() {
  static int currentDigit = 0;
  
  byte data = digitMap[d[currentDigit]];
  // Apply colon to the second digit if required
  if (currentDigit == 1 && globalColon) data &= 0b11111110;

  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, ~data);
  digitalWrite(latchPin, HIGH);

  digitalWrite(digitPins[currentDigit], LOW); // ON
  delayMicroseconds(600);                     // Brightness control TODO: tie value to potentiometer knob
  digitalWrite(digitPins[currentDigit], HIGH); // OFF

  currentDigit = (currentDigit + 1) % 4;
}

void handleInputsAndMenu() {
  // ESP32 ADC is 12-bit (0-4095)
  int xValue = analogRead(xPin);
  int yValue = analogRead(yPin);
  
  // The SW pin on joystick is usually active LOW (0 when pressed)
  bool swState = digitalRead(swPin); 
  bool buttonPressed = debounce(swState);

  // Determine Joystick Direction with 4095 scaling
  // Center is roughly 2048
  int joy = 0; // CENTER
  if (yValue < 1000) joy = 1;      // UP
  else if (yValue > 3000) joy = 3; // DOWN
  else if (xValue < 1000) joy = 4; // LEFT
  else if (xValue > 3000) joy = 2; // RIGHT

  // --- MENU NAVIGATION ---
  // We use a "lastJoy" check to ensure we only move ONCE per tilt
  static int lastJoy = 0;
  if (joy != lastJoy && joy != 0) {
    if (menuScreen == 1) { // SETTASK
       if (joy == 2) { // RIGHT
         taskSelected = (taskSelected + 1) % 3;
         loaded = false;
       } else if (joy == 4) { // LEFT
         taskSelected = (taskSelected - 1 + 3) % 3;
         loaded = false;
       }
    } 
    else if (menuScreen == 2) { // SETTIMER
       if (joy == 1) { // UP
         taskArray[taskSelected].targetTime += 60000;
         loaded = false;
       } else if (joy == 3) { // DOWN
         if (taskArray[taskSelected].targetTime > 60000) {
            taskArray[taskSelected].targetTime -= 60000;
            loaded = false;
         }
       }
    }
    else if (menuScreen == 0) { // WELCOME
       menuScreen = 1;
       loaded = false;
    }
  }
  lastJoy = joy;

  // --- BUTTON NAVIGATION ---
  if (buttonPressed) {
    if (menuScreen == 1) menuScreen = 2;      // From Task to Timer
    else if (menuScreen == 2) menuScreen = 3; // From Timer to Running
    else if (menuScreen == 3) menuScreen = 4; // From Running to Paused
    else if (menuScreen == 4) menuScreen = 3; // From Paused to Running
    loaded = false;
  }

  // --- TIMER TICKING ---
  if (menuScreen == 3) { // RUNNING
    if (millis() - previousMillis >= interval) {
      previousMillis = millis();
      taskArray[taskSelected].timeSpent += 1000;
      
      int currentSecs = (taskArray[taskSelected].timeSpent / 1000) % 60;

      // ONLY update LCD when seconds hit zero (every minute)
      // or if we just started (loaded == false)
      if (currentSecs == 0) {
        updateLCDOnlyNumbers(); 
      }
    }
  }

  // Reload screen if menu changed
  if (!loaded) {
    renderFullLCD();
  }
}

void renderFullLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(taskArray[taskSelected].taskName);
  updateLCDOnlyNumbers();
  loaded = true;
}

void updateLCDOnlyNumbers() {
  lcd.setCursor(0, 1);
  lcd.print(taskArray[taskSelected].timeSpent / 60000);
  lcd.print("min / ");
  lcd.print(taskArray[taskSelected].targetTime / 60000);
  lcd.print("min   "); // Spaces clear old digits
}

bool debounce(int reading) {
  if (reading == LOW && (millis() - lastDebounceTime > debounceDelay)) {
    lastDebounceTime = millis();
    return true;
  }
  return false;
}