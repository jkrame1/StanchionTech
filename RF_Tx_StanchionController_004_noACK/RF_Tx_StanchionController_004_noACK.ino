/*
 * RF TRANSMITTER CONTROLLER with Keypad & LCD
 * NO ACK, TRIPLE-SEND VERSION for 8+ receivers
 * For Arduino Uno
 * 
 * Controls all stanchions wirelessly via nRF24L01+
 * - Keypad selects mode (keys 0-7) and sends button commands
 * - LCD displays current mode and status
 * - Key 'A' sends button press, 'B' sends button release
 * - Knob sends analog values
 * 
 * WIRING:
 *   nRF24L01+ → Arduino Uno
 *   VCC  → 3.3V (IMPORTANT: nRF24 needs 3.3V!)
 *   GND  → GND
 *   CSN  → Pin 10
 *   CE   → Pin A0
 *   MOSI → Pin 11
 *   MISO → Pin 12
 *   SCK  → Pin 13
 *   
 *   4x4 Keypad → Arduino Uno
 *   Row pins → 9, 8, 7, 6
 *   Col pins → 5, 4, 3, 2
 *   
 *   LCD I2C → Arduino Uno
 *   SDA → A4
 *   SCL → A5
 *   VCC → 5V
 *   GND → GND
 *   
 *   Buttons:
 *   Black button → A1 (pin 15)
 *   Red button   → A2 (pin 16)
 *   
 *   Knob (pot) → A3
 */

#include <SPI.h>
#include <RF24.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Bounce2.h>
#include <elapsedMillis.h>
#include <ResponsiveAnalogRead.h>

// RF Module pins
#define RF_CE_PIN A0
#define RF_CSN_PIN 10

// Button Connections
#define BUTTON_PIN_BLACK 15  // A1
#define BUTTON_PIN_RED 16    // A2

Bounce buttonBlack = Bounce();
Bounce buttonRed = Bounce();

#define ANALOG_PIN A3
#define RF_CHANNEL 108  // Theater-optimized channel

ResponsiveAnalogRead knob1(ANALOG_PIN, true);

// Keypad setup
const byte ROWS = 4;
const byte COLS = 4;
char hexaKeys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte rowPins[ROWS] = { 9, 8, 7, 6 };
byte colPins[COLS] = { 5, 4, 3, 2 };
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

// Create objects
LiquidCrystal_I2C lcd(0x27, 16, 2);
RF24 radio(RF_CE_PIN, RF_CSN_PIN);

// RF Address
const byte broadcastAddress[6] = "STAGE";

// Command packet structure
struct CommandPacket {
  byte targetID;
  byte commandType;
  byte value1;
  byte value2;
  unsigned long timestamp;
};

CommandPacket txPacket;

// State tracking
int currentMode = 0;
bool buttonBlackState = false;
bool buttonRedState = false;
byte knobVal;

// Timers
elapsedMillis screenTimer;
unsigned int screenInterval = 500;
bool screenActive = true;

void setup() {
  Serial.begin(9600);
  delay(500);

  // Set up debouncing for buttons
  buttonBlack.attach(BUTTON_PIN_BLACK, INPUT_PULLUP);
  buttonBlack.interval(25);
  buttonRed.attach(BUTTON_PIN_RED, INPUT_PULLUP);
  buttonRed.interval(25);

  // Initialize LCD
  lcd.backlight();
  lcd.init();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("RF Controller");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");

  Serial.println("========================================");
  Serial.println("RF TX - NO ACK, TRIPLE-SEND");
  Serial.println("========================================");

  // Initialize RF module
  if (!radio.begin()) {
    Serial.println("RF module FAILED!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("RF Module");
    lcd.setCursor(0, 1);
    lcd.print("ERROR!");
    while (1) delay(1000);
  }

  Serial.println("RF module OK");

  // *** CRITICAL: NO ACK CONFIGURATION ***
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(RF_CHANNEL);
  radio.setRetries(15, 15);        // Not used without ACK, but set anyway
  radio.enableDynamicPayloads();
  radio.setAutoAck(false);         // *** DISABLE ACK - Prevents collision with 8 receivers ***

  radio.openWritingPipe(broadcastAddress);
  radio.stopListening();

  Serial.println("Configuration:");
  Serial.println("  - ACKs DISABLED");
  Serial.println("  - Triple-send enabled");
  Serial.println("  - Channel 108");
  Serial.println("\nController ready!");
  Serial.println("Keys 0-7: Select mode");
  Serial.println("Key A: Button press");
  Serial.println("Key B: Button release");
  Serial.println("========================================\n");

  // Display ready screen
  delay(500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Mode: 0");
  lcd.setCursor(0, 1);
  lcd.print("Ready (NoACK)");
}

void loop() {
  // Read knob, detect changes, and only send on change
  knob1.update();

  knobVal = (1023 - knob1.getRawValue() / 4);
  
  if (knob1.hasChanged()) {
    handleKnobChange(knobVal);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("KnobVal Sent");
    lcd.setCursor(0, 1);
    lcd.print(knobVal);
    delay(10);
    screenTimer = 0;
    screenActive = true;
  }

  // Read KeyPad
  char customKey = customKeypad.getKey();

  if (customKey) {
    handleKeyPress(customKey);
  }

  // Read buttons
  buttonBlack.update();
  buttonRed.update();

  if (buttonBlack.fell()) {
    sendButtonPress(true);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Button PRESSED");
    lcd.setCursor(0, 1);
    lcd.print("Sent 3x");
    screenTimer = 0;
    screenActive = true;
  }

  if (buttonBlack.rose()) {
    sendButtonPress(false);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Button RELEASED");
    lcd.setCursor(0, 1);
    lcd.print("Sent 3x");
    screenTimer = 0;
    screenActive = true;
  }

  if (buttonRed.fell()) {
    sendButtonPress(true);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Button PRESSED");
    lcd.setCursor(0, 1);
    lcd.print("Sent 3x");
    screenTimer = 0;
    screenActive = true;
  }

  if (buttonRed.rose()) {
    sendButtonPress(false);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Button RELEASED");
    lcd.setCursor(0, 1);
    lcd.print("Sent 3x");
    screenTimer = 0;
    screenActive = true;
  }

  // Clear screen after interval
  if (screenTimer > screenInterval) {
    if (screenActive) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Mode: ");
      lcd.print(currentMode);
      lcd.setCursor(0, 1);
      lcd.print("Ready");
      screenActive = false;
    }
  }
}

void handleKeyPress(char key) {
  Serial.print("Key pressed: ");
  Serial.println(key);

  // Mode selection (0-7)
  if (key >= '0' && key <= '9') {
    int newMode = key - '0';
    currentMode = newMode;
    sendModeChange(newMode);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Mode: ");
    lcd.print(newMode);
    lcd.setCursor(0, 1);
    lcd.print("Sent 3x");

    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Mode: ");
    lcd.print(currentMode);
    lcd.setCursor(0, 1);
    lcd.print("Ready");
  }
  // Button press command
  else if (key == 'A') {
    sendButtonPress(true);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Button PRESSED");
    lcd.setCursor(0, 1);
    lcd.print("Sent 3x");

    delay(500);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Mode: ");
    lcd.print(currentMode);
    lcd.setCursor(0, 1);
    lcd.print("Ready");
  }
  // Button release command
  else if (key == 'B') {
    sendButtonPress(false);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Button RELEASED");
    lcd.setCursor(0, 1);
    lcd.print("Sent 3x");

    delay(500);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Mode: ");
    lcd.print(currentMode);
    lcd.setCursor(0, 1);
    lcd.print("Ready");
  }
  // Other keys
  else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Key: ");
    lcd.print(key);
    lcd.setCursor(0, 1);
    lcd.print("Not assigned");

    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Mode: ");
    lcd.print(currentMode);
    lcd.setCursor(0, 1);
    lcd.print("Ready");
  }
}

void handleKnobChange(int knobVal) {
  sendKnobChange(knobVal);
}

// *** TRIPLE-SEND FUNCTIONS - NO ACK ***

void sendModeChange(int newMode) {
  txPacket.targetID = 0;
  txPacket.commandType = 0;
  txPacket.value1 = newMode;
  txPacket.value2 = 0;
  txPacket.timestamp = millis();

  // Send 3 times for reliability
  for (int i = 0; i < 3; i++) {
    radio.write(&txPacket, sizeof(CommandPacket));
    delay(5);
  }

  Serial.print("Mode ");
  Serial.print(newMode);
  Serial.println(" SENT (3x)");
}

void sendButtonPress(bool pressed) {
  txPacket.targetID = 0;
  txPacket.commandType = 1;
  txPacket.value1 = pressed ? 1 : 0;
  txPacket.value2 = 0;
  txPacket.timestamp = millis();

  // Send 3 times for reliability
  for (int i = 0; i < 3; i++) {
    radio.write(&txPacket, sizeof(CommandPacket));
    delay(5);
  }

  Serial.print("Button ");
  Serial.print(pressed ? "PRESSED" : "RELEASED");
  Serial.println(" SENT (3x)");
}

void sendKnobChange(int knobVal) {
  txPacket.targetID = 0;
  txPacket.commandType = 2;
  txPacket.value1 = knobVal;
  txPacket.value2 = 0;
  txPacket.timestamp = millis();

  // Send 3 times for reliability
  for (int i = 0; i < 3; i++) {
    radio.write(&txPacket, sizeof(CommandPacket));
    delay(5);
  }

  Serial.print("knobVal ");
  Serial.print(knobVal);
  Serial.println(" SENT (3x)");
}

/*
 * CHANGES FROM ORIGINAL:
 * 
 * 1. radio.setAutoAck(false) - CRITICAL for 8 receivers
 * 2. All send functions now loop 3x with 5ms delay
 * 3. Channel changed to 108 (less WiFi interference)
 * 4. LCD feedback shows "Sent 3x" instead of success/fail
 * 5. Removed success checking (no ACKs to check)
 * 
 * USAGE:
 * 
 * Keys 0-7: Select mode and broadcast to all stanchions
 * Key A:    Send button PRESS command
 * Key B:    Send button RELEASE command
 * Keys C, D, *, #: Not assigned (can be customized)
 * Black/Red buttons: Hardware button press/release
 * Knob: Send analog value (0-255)
 * 
 * LCD shows current mode and feedback for each action
 * 
 * TESTING:
 * - Should work reliably with 4-8 receivers
 * - No ACK collision issues
 * - Triple-send ensures delivery even in noisy theater
 */