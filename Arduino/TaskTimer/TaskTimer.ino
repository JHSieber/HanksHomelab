//Task Timer V1. Embedded system project to create a small device to keep track of time spent on different tasks (like a chess timer and a time tracker in one). 
// This version was built for an arduino UNO board using a 16x2LCD display for menu navigation and info and a 7 segment display for the timer counting down.
// Controlled with a Joystick (navigation and button press)

#include <Wire.h>               // Include the Wire library for I2C communication
#include <LiquidCrystal_I2C.h>  // Include the LCD library for I2C

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Set up the LCD (address 0x27, 16 columns, 2 rows)


//TODO: ADD flashing play and pause buttons/icons
//TODO: connect to other source for automatic task input
// TODO: add 4 digit 7-segment display for countdown
// TODO: Add more satisfying button

void displayNumber(int digit, int number, bool colon = false);

//Joystick Pins
const int xPin = 35;  //the VRX attach to
const int yPin = 34;  //the VRY attach to
const int swPin = 23;  //the SW attach to


int latchPin = 33;
int dataPin = 25; 
int clockPin = 32; 

// Flipped the order so Digit 0 is the far left
const int digitPins[4] = { 13, 14, 27, 26 }; 

// Final Common Anode Map (Inversion baked in based off 7segment test results)
const byte digitMap[] = {
  0b00000011,  // 0
  0b10011111,  // 1
  0b00100101,  // 2
  0b00001101,  // 3
  0b10011001,  // 4
  0b01001001,  // 5
  0b01000001,  // 6
  0b00011111,  // 7
  0b00000001,  // 8
  0b00001001,   // 9
  0b10010001   // 10th position ("H") for hours
};

const int lastDisplayed[4] = {-1,-1,-1,-1};


const int buzzerPin = 9;  // Assigns the pin 9 to the constant for the buzzer

//Timing variables
const int interval = 1000;  //Counts in intervals of 1 second
unsigned long startMillis;  // Stores the time when the timer starts
unsigned long previousMillis;


int taskSelected;  //What task is active
bool loaded;       //track if a screen reload is necessary


unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 500;   // the debounce time in between button presses
int lastButtonState = 1;
bool buttonpressed;

//Global values for menu screen to display
int menuScreen;
const int WELCOME = 0;
const int SETTASK = 1;
const int SETTIMER = 2;
const int RUNNING = 3;
const int PAUSED = 4;
const int FINISHED = 5;

//GLOBAL values for joystick control
const int CENTER = 0;
const int UP = 1;
const int RIGHT = 2;
const int DOWN = 3;
const int LEFT = 4;

// //Custom text symbols
// uint8_t bell[8] = { 0x4, 0xe, 0xe, 0xe, 0x1f, 0x0, 0x4 };
// uint8_t clock[8] = { 0x0, 0xe, 0x15, 0x17, 0x11, 0xe, 0x0 };
// uint8_t check[8] = { 0x0, 0x1, 0x3, 0x16, 0x1c, 0x8, 0x0 };


//Task object to store name, target time, and time spent
struct Task {
  String taskName;
  long targetTime;
  long timeSpent;
};
Task taskArray[3];  //Create array of tasks (TODO: make this dynamic not hard coded)
int numTasks = (sizeof(taskArray) / sizeof(Task)) - 1;


int remainaing_mins = 0;
int first_digit = 0;
int second_digit = 0;
int first_min_digit = 0;
int second_min_digit = 0;
int remaining_seconds = 0;
int first_sec_digit = 0;
int second_sec_digit =0;

void setup() {
  Serial.begin(115200);            
  Serial.print("setup running");
  pinMode(buzzerPin, OUTPUT);    // Initialize buzzer pin as an output
  pinMode(swPin, INPUT_PULLUP);  // Set sw Pin as input with an internal pull-up resistor
  startMillis = millis();        // Record the start time
  lcd.init();                   
  lcd.backlight();              

  //Setup for 7 segment display
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  for (int i = 0; i < 4; i++) {
    pinMode(digitPins[i], OUTPUT);
    digitalWrite(digitPins[i], HIGH);  // For Common Anode, HIGH is OFF
  }

  taskArray[0].taskName = "Task Timer";
  taskArray[1].taskName = "Edit Photos";
  taskArray[2].taskName = "Website";
  taskArray[0].targetTime = 3600000;  //Default target time 1 Hr, 60Min, 3600 sec, 3600000 milliseconds
  taskArray[1].targetTime = 3600000;
  taskArray[2].targetTime = 3600000;
  taskArray[0].timeSpent = 0;
  taskArray[1].timeSpent = 0;
  taskArray[2].timeSpent = 0;
  // for (int i = 0; i < sizeof(taskArray); i++) {
  //   taskArray[i].targetTime = 3600;
  //   taskArray[i].timeSpent = 0;
  // }

  menuScreen = WELCOME;
  loaded = false;
  taskSelected = 0;
}


void loop() {
  unsigned long currentMillis = millis();  // Update the current time
  // Read the joystick values
  int xValue = analogRead(xPin);
  int yValue = analogRead(yPin);
  int swValue = digitalRead(swPin);
  int joystickDirection = CENTER;

  Seconds_to7Seg((int)((taskArray[taskSelected].targetTime - taskArray[taskSelected].timeSpent)/1000));

  buttonpressed = debounce(swValue);
  if (buttonpressed) {
    Serial.println("buttonpressed");
    Serial.println(buttonpressed == true);
  }



  if (yValue < 412) {
    // Joystick up
    joystickDirection = UP;
  } else if (yValue > 612) {
    // Joystick down
    joystickDirection = DOWN;
  } else if (xValue < 412) {
    // Joystick left
    joystickDirection = LEFT;
  } else if (xValue > 612) {
    // Joystick right
    joystickDirection = RIGHT;
  } else {
    // Joystick in the middle, turn off all LEDs
    joystickDirection = CENTER;
  }


  switch (menuScreen) {
    case WELCOME:
      if (loaded == false) {
        Serial.println("loading welcome");
        lcd.clear();                    // Clear the LCD
        lcd.print("    Welcome!");      // Welcome message
        lcd.setCursor(0, 1);            // Move cursor to the second line
        lcd.print("  Select a task!");  // Instruction message
        loaded = true;
      }
      if (joystickDirection != CENTER) {
        menuScreen = SETTASK;
        loaded = false;
      }
      break;
    case SETTASK:
      if (loaded == false) {
        Serial.println("loading Set task");
        loaded = Render_screen(menuScreen, joystickDirection, taskSelected);
      }
      if (buttonpressed) {
        menuScreen = SETTIMER;
        loaded = false;
      }
      if (joystickDirection == RIGHT) {
        taskSelected += 1;
        if (taskSelected > numTasks) {
          taskSelected = 0;
        }
        loaded = false;
        delay(100);

      } else if (joystickDirection == LEFT) {
        taskSelected -= 1;
        if (taskSelected < 0) {
          taskSelected = numTasks;
        }
        loaded = false;
        delay(100);
      }

      break;
    case SETTIMER:
      if (loaded == false) {
        Serial.println("loading Settimer");
        loaded = Render_screen(menuScreen, joystickDirection, taskSelected);
      }

      if (buttonpressed) {
        menuScreen = RUNNING;
        startMillis = millis();  //Initialize the tick counters
        previousMillis = startMillis;
      }
      if (joystickDirection == UP) {
        taskArray[taskSelected].targetTime += 1 * 60000;  //add to target time in increments of 1 minute
        loaded = false;
        delay(100);
      } else if (joystickDirection == DOWN) {
        taskArray[taskSelected].targetTime -= 1 * 60000;   //subtract target time in increments of 1 minute
        if (taskArray[taskSelected].targetTime < 60000) {  //Prevent target time going below 1 minute
          taskArray[taskSelected].targetTime = 60000;
        }
        loaded = false;
        delay(100);
      }

      break;
    case RUNNING:
      if (loaded == false) {
        Serial.println("loading Running");
        loaded = Render_screen(menuScreen, joystickDirection, taskSelected);
      }

      if (buttonpressed == true) {
        loaded = false;
        menuScreen = PAUSED;
        break;
      }

      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        taskArray[taskSelected].timeSpent += 1000;  //Add 1 second to timer
        //loaded = false;                             //trigger reload
      }

      if (taskArray[taskSelected].timeSpent >= taskArray[taskSelected].targetTime) {
        Buzz_Buzz();
        menuScreen = FINISHED;
      }


      break;
    case PAUSED:
      if (loaded == false) {
        Serial.println("loading Paused");
        loaded = Render_screen(menuScreen, joystickDirection, taskSelected);
      }
      if (buttonpressed == true) {
        loaded = false;
        menuScreen = RUNNING;
        break;
      }
      if (joystickDirection == RIGHT) {
        taskSelected += 1;
        if (taskSelected > numTasks) {
          taskSelected = 0;
        }
        loaded = false;

      } else if (joystickDirection == LEFT) {
        taskSelected -= 1;
        if (taskSelected < 0) {
          taskSelected = numTasks;
        }
        loaded = false;
      }
      break;
    case FINISHED:
      break;
  }
}

bool debounce(int reading) {

  // check if enough time has passed since last button press to prevent double presses

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    lastButtonState = reading;
    // if the button state has changed:
    if (reading == 0) {
      Serial.println("true");
      lastDebounceTime = millis();
      return true;
    }
  }
  return false;


  // save the reading. Next time through the loop, it'll be the lastButtonState:
}


void Buzz_Buzz() {
  digitalWrite(buzzerPin, HIGH);  // Turn buzzer on
  delay(500);                     // Buzzer on for 500 milliseconds
  digitalWrite(buzzerPin, LOW);   // Turn buzzer off
  delay(200);                     // Buzzer off for 200 milliseconds
  digitalWrite(buzzerPin, HIGH);  // Turn buzzer on
  delay(500);                     // Buzzer on for 500 milliseconds
  digitalWrite(buzzerPin, LOW);   // Turn buzzer off
  delay(200);                     // Buzzer off for 200 milliseconds
}


//For rendering task screens (not the welcome screen)
bool Render_screen(int menuScreen, int joystickDirection, int taskSelected) {
  lcd.clear();  // Clear the LCD
  lcd.cursor_off();
  lcd.setCursor(0, 0);                          // Move cursor to the second line
  lcd.print(taskArray[taskSelected].taskName);  // Welcome message
  //lcd.createChar(2, check);
  lcd.setCursor(0, 1);  // Move cursor to the second line
  lcd.print(taskArray[taskSelected].timeSpent / 60000);
  //lcd.createChar(0, bell);
  lcd.print("m");
  lcd.print((taskArray[taskSelected].timeSpent / 1000) % 60);
  lcd.print("s of ");
  lcd.print(taskArray[taskSelected].targetTime / 60000);
  lcd.print("mins");

  loaded = true;
  return loaded;
}

void Seconds_to7Seg(int seconds) {
  // We calculate the digits once
  int d[4];
  bool colon = false;

  if (seconds > 3600) {
    d[0] = seconds / 3600;
    d[1] = 10; // "H"
    int mins = (seconds % 3600) / 60;
    d[2] = mins / 10;
    d[3] = mins % 10;
  } else {
    int mins = seconds / 60;
    int secs = seconds % 60;
    d[0] = mins / 10;
    d[1] = mins % 10;
    d[2] = secs / 10;
    d[3] = secs % 10;
    colon = true;
  }

  // On the ESP32, we pulse ONLY ONE digit per loop iteration
  // to keep the loop speed extremely high and fix the flickering problem.

  static int currentDigit = 0;
  renderDigit(currentDigit, d[currentDigit], (currentDigit == 1 && colon));
  currentDigit = (currentDigit + 1) % 4;
}

void renderDigit(int digit, int number, bool colon) {
  byte data = digitMap[number];
  if (colon) data &= 0b11111110;

  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, ~data);
  digitalWrite(latchPin, HIGH);

  digitalWrite(digitPins[digit], LOW);  
  delayMicroseconds(500); // Shorter pulse for ESP32
  digitalWrite(digitPins[digit], HIGH); 
}
