// MIDI to RF Bridge for Stanchion Control
// Teensy 4.1 with nRF24L01+ radio module
// Receives MIDI over USB and transmits RF commands to stanchions
// Hardware knobs/buttons for quick testing

#include <RF24.h>
#include <Bounce2.h>

// RF MODULE CONFIGURATION (matches stanchions & transmitter)
#define RF_CE_PIN 22
#define RF_CSN_PIN 15
#define RF_CHANNEL 108

// Hardware pins
#define KNOB1_PIN A1
#define KNOB2_PIN A2
#define KNOB3_PIN A3
#define BUTTON1_PIN 0
#define BUTTON2_PIN 1
#define BUTTON3_PIN 2

RF24 radio(RF_CE_PIN, RF_CSN_PIN);
const byte broadcastAddress[6] = "STAGE";

// RF Command Structure (matches your existing stanchions)
struct CommandPacket {
  byte targetID;      // 0 = broadcast, 1-15 = specific stanchion
  byte commandType;   // Command type (extended for MIDI)
  byte value1;        // First parameter
  byte value2;        // Second parameter
  unsigned long timestamp;
};

// Extended Command Types (keeping your original 0-2, adding new ones)
#define CMD_MODE_CHANGE     0   // Original - mode change
#define CMD_BUTTON          1   // Original - button press/release
#define CMD_KNOB            2   // Original - knob value
#define CMD_NOTE_ON         3   // NEW - MIDI note on
#define CMD_NOTE_OFF        4   // NEW - MIDI note off
#define CMD_CC              5   // NEW - MIDI CC
#define CMD_PROGRAM_CHANGE  6   // NEW - MIDI program change (maps to mode)
#define CMD_PITCH_BEND      7   // NEW - MIDI pitch bend

CommandPacket txPacket;

// Hardware objects
Bounce button1 = Bounce();
Bounce button2 = Bounce();
Bounce button3 = Bounce();

// Knob tracking
int knob1Value = 0;
int knob2Value = 0;
int knob3Value = 0;
int lastKnob1 = -1;
int lastKnob2 = -1;
int lastKnob3 = -1;

// MIDI CC assignments for hardware knobs (you can change these)
const byte KNOB1_CC = 1;   // Mod wheel
const byte KNOB2_CC = 2;   // Breath controller
const byte KNOB3_CC = 7;   // Volume

// MIDI note assignments for hardware buttons (you can change these)
const byte BUTTON1_NOTE = 60;  // C4
const byte BUTTON2_NOTE = 62;  // D4
const byte BUTTON3_NOTE = 64;  // E4

// Timing
elapsedMillis lastKnobSend;
const unsigned long KNOB_SEND_INTERVAL = 50; // Send knob updates max 20Hz

void setup() {
  Serial.begin(9600);
  delay(100);
  
  // Setup buttons
  button1.attach(BUTTON1_PIN, INPUT_PULLUP);
  button1.interval(25);
  button2.attach(BUTTON2_PIN, INPUT_PULLUP);
  button2.interval(25);
  button3.attach(BUTTON3_PIN, INPUT_PULLUP);
  button3.interval(25);
  
  // Setup analog pins for knobs
  pinMode(KNOB1_PIN, INPUT);
  pinMode(KNOB2_PIN, INPUT);
  pinMode(KNOB3_PIN, INPUT);
  
  // Setup RF Radio (match your existing setup exactly)
  if (!radio.begin()) {
    Serial.println("❌ RF module not found!");
    while (1) {
      delay(1000);
    }
  }
  
  // CRITICAL: Match stanchion settings exactly
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(RF_CHANNEL);
  radio.setRetries(15, 15);
  radio.enableDynamicPayloads();
  radio.setAutoAck(false);  // NO ACK - matches your stanchions
  
  radio.openWritingPipe(broadcastAddress);
  radio.stopListening();  // TX mode
  
  Serial.println("===============================");
  Serial.println("MIDI-to-RF Bridge Ready");
  Serial.println("===============================");
  Serial.println("Channel 108, ACKs disabled");
  Serial.println("Triple-send enabled");
  Serial.println();
  Serial.println("MIDI Channel Routing:");
  Serial.println("  Ch 1  -> Broadcast (all)");
  Serial.println("  Ch 2  -> Stanchion #1");
  Serial.println("  Ch 3  -> Stanchion #2");
  Serial.println("  ...   -> ...");
  Serial.println("  Ch 16 -> Stanchion #15");
  Serial.println();
  Serial.println("Hardware Controls:");
  Serial.println("  Knob 1 (A1) -> CC1");
  Serial.println("  Knob 2 (A2) -> CC2");
  Serial.println("  Knob 3 (A3) -> CC7");
  Serial.println("  Button 1 (D0) -> Note 60");
  Serial.println("  Button 2 (D1) -> Note 62");
  Serial.println("  Button 3 (D2) -> Note 64");
  Serial.println("===============================");
  Serial.println();
  
  // Initialize knob values
  lastKnob1 = analogRead(KNOB1_PIN) / 8;
  lastKnob2 = analogRead(KNOB2_PIN) / 8;
  lastKnob3 = analogRead(KNOB3_PIN) / 8;
}

void loop() {
  // Check for USB MIDI messages
  if (usbMIDI.read()) {
    handleUSBMIDI();
  }
  
  // Check hardware buttons
  checkButtons();
  
  // Check hardware knobs (throttled)
  if (lastKnobSend > KNOB_SEND_INTERVAL) {
    checkKnobs();
    lastKnobSend = 0;
  }
}

void handleUSBMIDI() {
  byte type = usbMIDI.getType();
  byte channel = usbMIDI.getChannel();
  byte data1 = usbMIDI.getData1();
  byte data2 = usbMIDI.getData2();
  
  // Determine target from MIDI channel
  // Channel 1 = broadcast (0)
  // Channels 2-16 = stanchions 1-15
  byte target = (channel == 1) ? 0 : (channel - 1);
  
  switch (type) {
    
    case usbMIDI.NoteOn:
      if (data2 > 0) {  // Velocity > 0 = note on
        Serial.print("🎵 Note ON: ");
        Serial.print(data1);
        Serial.print(" vel:");
        Serial.print(data2);
        Serial.print(" -> ");
        printTarget(target);
        
        sendRFCommand(target, CMD_NOTE_ON, data1, data2);
      } else {  // Velocity 0 = note off
        Serial.print("🎵 Note OFF: ");
        Serial.print(data1);
        Serial.print(" -> ");
        printTarget(target);
        
        sendRFCommand(target, CMD_NOTE_OFF, data1, 0);
      }
      break;
      
    case usbMIDI.NoteOff:
      Serial.print("🎵 Note OFF: ");
      Serial.print(data1);
      Serial.print(" -> ");
      printTarget(target);
      
      sendRFCommand(target, CMD_NOTE_OFF, data1, 0);
      break;
      
    case usbMIDI.ControlChange:
      Serial.print("🎛️  CC");
      Serial.print(data1);
      Serial.print(": ");
      Serial.print(data2);
      Serial.print(" -> ");
      printTarget(target);
      
      sendRFCommand(target, CMD_CC, data1, data2);
      break;
      
    case usbMIDI.ProgramChange:
      Serial.print("📋 Program Change: ");
      Serial.print(data1);
      Serial.print(" -> ");
      printTarget(target);
      
      // Program changes map to mode changes
      sendRFCommand(target, CMD_PROGRAM_CHANGE, data1, 0);
      break;
      
    case usbMIDI.PitchBend:
      {
        // Pitch bend is 14-bit (0-16383, center = 8192)
        int bendValue = data1 | (data2 << 7);
        Serial.print("🎚️  Pitch Bend: ");
        Serial.print(bendValue);
        Serial.print(" -> ");
        printTarget(target);
        
        // Send as two bytes
        sendRFCommand(target, CMD_PITCH_BEND, data1, data2);
      }
      break;
      
    default:
      // Ignore other MIDI message types
      break;
  }
}

void checkButtons() {
  button1.update();
  button2.update();
  button3.update();
  
  // Button 1
  if (button1.fell()) {
    Serial.print("🔘 Button 1 PRESSED -> Note ");
    Serial.println(BUTTON1_NOTE);
    sendRFCommand(0, CMD_NOTE_ON, BUTTON1_NOTE, 127);  // Broadcast
  }
  if (button1.rose()) {
    Serial.print("🔘 Button 1 RELEASED -> Note ");
    Serial.println(BUTTON1_NOTE);
    sendRFCommand(0, CMD_NOTE_OFF, BUTTON1_NOTE, 0);
  }
  
  // Button 2
  if (button2.fell()) {
    Serial.print("🔘 Button 2 PRESSED -> Note ");
    Serial.println(BUTTON2_NOTE);
    sendRFCommand(0, CMD_NOTE_ON, BUTTON2_NOTE, 127);
  }
  if (button2.rose()) {
    Serial.print("🔘 Button 2 RELEASED -> Note ");
    Serial.println(BUTTON2_NOTE);
    sendRFCommand(0, CMD_NOTE_OFF, BUTTON2_NOTE, 0);
  }
  
  // Button 3
  if (button3.fell()) {
    Serial.print("🔘 Button 3 PRESSED -> Note ");
    Serial.println(BUTTON3_NOTE);
    sendRFCommand(0, CMD_NOTE_ON, BUTTON3_NOTE, 127);
  }
  if (button3.rose()) {
    Serial.print("🔘 Button 3 RELEASED -> Note ");
    Serial.println(BUTTON3_NOTE);
    sendRFCommand(0, CMD_NOTE_OFF, BUTTON3_NOTE, 0);
  }
}

void checkKnobs() {
  // Read knobs (10-bit ADC to 7-bit MIDI: 0-1023 -> 0-127)
  knob1Value = analogRead(KNOB1_PIN) / 8;
  knob2Value = analogRead(KNOB2_PIN) / 8;
  knob3Value = analogRead(KNOB3_PIN) / 8;
  
  // Only send if changed (with small deadband to prevent jitter)
  if (abs(knob1Value - lastKnob1) > 1) {
    Serial.print("🎛️  Knob 1 -> CC");
    Serial.print(KNOB1_CC);
    Serial.print(": ");
    Serial.println(knob1Value);
    
    sendRFCommand(0, CMD_CC, KNOB1_CC, knob1Value);  // Broadcast
    lastKnob1 = knob1Value;
  }
  
  if (abs(knob2Value - lastKnob2) > 1) {
    Serial.print("🎛️  Knob 2 -> CC");
    Serial.print(KNOB2_CC);
    Serial.print(": ");
    Serial.println(knob2Value);
    
    sendRFCommand(0, CMD_CC, KNOB2_CC, knob2Value);
    lastKnob2 = knob2Value;
  }
  
  if (abs(knob3Value - lastKnob3) > 1) {
    Serial.print("🎛️  Knob 3 -> CC");
    Serial.print(KNOB3_CC);
    Serial.print(": ");
    Serial.println(knob3Value);
    
    sendRFCommand(0, CMD_CC, KNOB3_CC, knob3Value);
    lastKnob3 = knob3Value;
  }
}

void sendRFCommand(byte target, byte cmdType, byte val1, byte val2) {
  txPacket.targetID = target;
  txPacket.commandType = cmdType;
  txPacket.value1 = val1;
  txPacket.value2 = val2;
  txPacket.timestamp = millis();
  
  // Triple-send for reliability (no ACK mode)
  for (int i = 0; i < 3; i++) {
    radio.write(&txPacket, sizeof(CommandPacket));
    delayMicroseconds(500);
  }
}

void printTarget(byte target) {
  if (target == 0) {
    Serial.println("ALL");
  } else {
    Serial.print("Stanchion #");
    Serial.println(target);
  }
}
