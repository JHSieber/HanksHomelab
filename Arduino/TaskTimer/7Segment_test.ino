//7Segment test. 
// My pin connections did not match the docs. Short program to cycle through different bit mappings until there is a coherent display 

// Pin Definitions (Matches your ESP32 wiring)
int latchPin = 33;
int dataPin = 25; 
int clockPin = 32; 
const int digitPins[4] = { 13, 14, 27, 26 };

// Common Anode Map
const byte digitMap[] = {
  0b00000011, 0b10011111, 0b00100101, 0b00001101, 0b10011001,
  0b01001001, 0b01000001, 0b00011111, 0b00000001, 0b00001001
};

void setup() {
  Serial.begin(115200);
  
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);

  for (int i = 0; i < 4; i++) {
    pinMode(digitPins[i], OUTPUT);
    digitalWrite(digitPins[i], LOW); // Start with all digits OFF (no power)
  }
  
  Serial.println("Starting 7-Segment Logic Test...");
  Serial.println("Logic: Digits = HIGH (Power), Segments = LOW (Sink)");
}

void loop() {
  // cycle through each digit and show a different number
  for (int i = 0; i < 4; i++) { 
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, ~data); 
    digitalWrite(latchPin, HIGH);

    // 2. Turn the Digit ON (Provide 3.3V power)
    digitalWrite(digitPins[i], HIGH); 
    
    // 3. Keep it on long enough to see (Slowed down for testing)
    delay(2); 
    
    // 4. Turn the Digit OFF (Cut power)
    digitalWrite(digitPins[i], LOW);
  }
}