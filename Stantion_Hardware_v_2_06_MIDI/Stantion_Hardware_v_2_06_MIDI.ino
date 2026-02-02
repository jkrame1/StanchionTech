//Stanchion Hardware - MIDI Enhanced
#define VERSION 2.10
// Updated for MIDI control via RF

#define VOICE_NUMBER 3  // SET THIS TO 0-15 FOR EACH STANCHION

// Include libraries
#include <Encoder.h>
#include <Bounce2.h>
#include "Adafruit_VCNL4200.h"
#include <FastTouch.h>
#include <MTP_Teensy.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <RF24.h>

// Pin definitions
#define BUTTON_PIN 2
#define TOUCH_PIN 0
#define R_PIN 3
#define G_PIN 4
#define B_PIN 6
#define ROTARY_PIN_DT 16
#define ROTARY_PIN_CLK 17
#define SDCARD_CS_PIN 10

// RF MODULE CONFIGURATION
#define RF_CSN_PIN 15

// --- Device-dependant Pin Definitions ---
#if defined(ARDUINO_TEENSY40) || defined(ARDUINO_TEENSY41)
// Teensy 4.0 / 4.1 Specific Pins
#define LED_PIN 9
#define SDCARD_MOSI_PIN 11
#define SDCARD_SCK_PIN 13
#define RF_CE_PIN 22

#elif defined(__MK20DX256__) || defined(__MK66FX1M0__) || defined(__MK64FX512__)
// Teensy 3.2 / 3.6 / 3.5 Specific Pins
#define LED_PIN 8
#define SDCARD_MOSI_PIN 7
#define SDCARD_SCK_PIN 14
#define RF_CE_PIN 15

#else
#error "Unsupported Teensy version"
#endif

RF24 radio(RF_CE_PIN, RF_CSN_PIN);
#define RF_CHANNEL 108

// RF Addresses
const byte broadcastAddress[6] = "STAGE";
byte stanchionAddress[6];

// RF Command Structure (Extended for MIDI)
struct CommandPacket {
  byte targetID;
  byte commandType;
  byte value1;
  byte value2;
  unsigned long timestamp;
};

// Command Types
#define CMD_MODE_CHANGE     0   // Mode change (original)
#define CMD_BUTTON          1   // Button press/release (original)
#define CMD_KNOB            2   // Knob value (original)
#define CMD_NOTE_ON         3   // MIDI note on
#define CMD_NOTE_OFF        4   // MIDI note off
#define CMD_CC              5   // MIDI CC
#define CMD_PROGRAM_CHANGE  6   // MIDI program change -> mode
#define CMD_PITCH_BEND      7   // MIDI pitch bend

CommandPacket rxPacket;

// MIDI CC Tracking Array (CC numbers 0-127)
byte ccValues[128];
bool ccChanged[128];

// Generic MIDI state (used by ALL modes)
byte currentMidiNote = 0;
byte currentMidiVelocity = 0;
bool midiNoteActive = false;

// GUItool: begin automatically generated code
AudioSynthNoisePink pink1;                 //xy=65,186
AudioInputI2S i2s1;                        //xy=78,303
AudioSynthWaveform waveform1;              //xy=107,437
AudioFilterStateVariable filter1;          //xy=188,192
AudioRecordQueue queue1;                   //xy=206,265
AudioAmplifier amp1;                       //xy=224,360
AudioSynthWaveformModulated waveformMod1;  //xy=261,443
AudioPlaySdRaw playWav1;                   //xy=275,480
AudioMixer4 mixer5;                        //xy=309,198
AudioMixer4 mixer6;                        //xy=354,297
AudioAnalyzeRMS rms1;                      //xy=356,382
AudioAnalyzePeak peak1;                    //xy=359,347
AudioEffectEnvelope envelope3;             //xy=410,479
AudioEffectEnvelope envelope2;             //xy=414,443
AudioSynthSimpleDrum drum1;                //xy=428,117
AudioPlaySdRaw playRaw1;                   //xy=436,157
AudioEffectEnvelope envelope1;             //xy=441,199
AudioEffectDelay delay1;                   //xy=468,319
AudioMixer4 mixer1;                        //xy=581,180
AudioMixer4 mixer3;                        //xy=585,466
AudioMixer4 mixer2;                        //xy=717,467
AudioAmplifier amp2;                       //xy=765,328
AudioOutputI2S i2s2;                       //xy=863,465
AudioAnalyzeRMS outRMS;                    //xy=879,354
AudioAnalyzePeak outPeak;                  //xy=882,313
AudioConnection patchCord1(pink1, 0, filter1, 0);
AudioConnection patchCord2(i2s1, 0, queue1, 0);
AudioConnection patchCord3(i2s1, 0, mixer6, 0);
AudioConnection patchCord4(i2s1, 1, amp1, 0);
AudioConnection patchCord5(waveform1, 0, waveformMod1, 0);
AudioConnection patchCord6(filter1, 0, mixer5, 0);
AudioConnection patchCord7(filter1, 1, mixer5, 1);
AudioConnection patchCord8(filter1, 2, mixer5, 2);
AudioConnection patchCord9(amp1, peak1);
AudioConnection patchCord10(amp1, rms1);
AudioConnection patchCord11(waveformMod1, envelope2);
AudioConnection patchCord12(playWav1, envelope3);
AudioConnection patchCord13(mixer5, envelope1);
AudioConnection patchCord14(mixer6, delay1);
AudioConnection patchCord15(envelope3, 0, mixer3, 1);
AudioConnection patchCord16(envelope2, 0, mixer3, 0);
AudioConnection patchCord17(drum1, 0, mixer1, 0);
AudioConnection patchCord18(playRaw1, 0, mixer1, 1);
AudioConnection patchCord19(envelope1, 0, mixer1, 2);
AudioConnection patchCord20(delay1, 0, mixer1, 3);
AudioConnection patchCord21(delay1, 1, mixer6, 1);
AudioConnection patchCord22(mixer1, 0, mixer2, 0);
AudioConnection patchCord24(mixer3, 0, mixer2, 1);
AudioConnection patchCord25(mixer2, 0, i2s2, 0);
AudioConnection patchCord26(mixer2, 0, i2s2, 1);
AudioConnection patchCord27(mixer2, amp2);
AudioConnection patchCord28(amp2, outPeak);
AudioConnection patchCord29(amp2, outRMS);
AudioControlSGTL5000 sgtl5000_1;  //xy=114,595
// GUItool: end automatically generated code

const int myInput = AUDIO_INPUT_MIC;

// Hardware objects
Adafruit_VCNL4200 vcnl4200;
Encoder myEnc(ROTARY_PIN_DT, ROTARY_PIN_CLK);
Bounce bouncer = Bounce();

// Timing variables
elapsedMillis drumTime;
elapsedMillis wave;
elapsedMillis flashTime;
elapsedMillis blinkTime;

bool blinkOn = false;

int drumBeatCount = 0;
int patternLength = 8;
unsigned long noteLength = 100;
unsigned long drumBounceTime = 1000;
unsigned long blinkInterval;

int touchAverage = 0;
int touchVal = 0;
int mappedTouch = 0;
bool touched = false;
bool pressed = false;
bool pressed2 = false;
bool buttonPressed = false;
bool buttonToggle = false;
int buttonClickCount = 0;
unsigned long lastButtonTime = 0;
const unsigned long encoderLockoutTime = 250;

int ledMaxBrightness = 200;
int ledBrightness = 255;
int ledValue = 0;
int direction;
int oldDirection = 0;
long newPosition = -999;
long oldPosition = -999;

int mode = 1;
const int numModes = 10;
int oldMode = 1;
bool modeChanged = false;

int debounceDistance = 8;
int detectionDistance = 10;

float peakThreshold = 0.0025;
float peakVal;
int repeatCount = 6;
int playCount = 0;
unsigned long repeatDuration = 750;
unsigned long recordDuration = 1500;
elapsedMillis repeatTime;
elapsedMillis recordTime;

int rMode = 0;
File frec;

bool rfConnected = false;
byte RFKnob = 0;

void setup() {
  // Set up PWM frequencies
  analogWriteFrequency(3, 585937.5);
  analogWriteFrequency(4, 585937.5);
  analogWriteFrequency(6, 585937.5);

  // Set RGB LED off
  analogWrite(R_PIN, 0);
  analogWrite(G_PIN, 0);
  analogWrite(B_PIN, 0);

  randomSeed(analogRead(A0));

  // Initialize CC tracking
  for (int i = 0; i < 128; i++) {
    ccValues[i] = 0;
    ccChanged[i] = false;
  }

  // Setup USB File Transfer
  MTP.begin();
  Serial.begin(9600);
  delay(100);
  MTP.addFilesystem(SD, "SD Card");

  // Setup digital I/O
  bouncer.attach(BUTTON_PIN, INPUT_PULLUP);
  bouncer.interval(25);
  pinMode(TOUCH_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  touchCalibration(10);

#if defined(ARDUINO_TEENSY40) || defined(ARDUINO_TEENSY41)
  AudioMemory(1800);
#elif defined(__MK20DX256__) || defined(__MK66FX1M0__) || defined(__MK64FX512__)
  AudioMemory(110);
#else
#error "Unsupported Teensy version"
#endif

  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(1);
  sgtl5000_1.micGain(10);
  sgtl5000_1.volume(0.8);
  sgtl5000_1.adcHighPassFilterEnable();

  // Mixer gains
  mixer1.gain(0, 1);
  mixer1.gain(1, 3.5);
  mixer1.gain(2, 0);
  mixer1.gain(3, 0.25);
  mixer6.gain(0, 0.21);
  mixer6.gain(1, 0.75);

  delay1.delay(0, 240 - (VOICE_NUMBER * 20));
  delay1.delay(1, 240 - (VOICE_NUMBER * 2));

  waveformMod1.begin(1, 220, WAVEFORM_SINE);
  envelope2.attack(10);
  envelope2.decay(10);
  envelope2.sustain(0.8);
  envelope2.release(1000);

  // Initialize SD card
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);

  if (!(SD.begin(SDCARD_CS_PIN))) {
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }

  // SETUP RF RADIO
  setupRF();

  Serial.println("===============================");
  Serial.print("Stanchion #");
  Serial.print(VOICE_NUMBER);
  Serial.print("  Code Version: ");
  Serial.println(VERSION);
  Serial.println("MIDI Enhanced");
  Serial.println("===============================");

  if (!vcnl4200.begin()) {
    Serial.println("Could not find VCNL4200 sensor!");
  } else {
    Serial.println("VCNL4200 found!");
  }

  // Setup proximity sensor
  vcnl4200.setProxShutdown(true);
  vcnl4200.setProxHD(false);
  vcnl4200.setProxLEDCurrent(VCNL4200_LED_I_200MA);
  vcnl4200.setProxIntegrationTime(VCNL4200_PS_IT_8T);
  vcnl4200.setALSshutdown(false);
  vcnl4200.setALSIntegrationTime(VCNL4200_ALS_IT_100MS);
  vcnl4200.setALSPersistence(VCNL4200_ALS_PERS_2);
  vcnl4200.setALSthresholdLow(100);
  vcnl4200.setALSthresholdHigh(20000);
  vcnl4200.setInterrupt(true, false);

  Serial.println("Setup complete - RF listening...");

  // Flash LED to show startup
  for (int i = 0; i < VOICE_NUMBER + 1; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
}

void loop() {
  MTP.loop();

  // Check for RF commands (non-blocking)
  checkRFCommands();

  // Read local button
  bouncer.update();
  bool buttonState = !bouncer.read();

  if (buttonState && !buttonPressed) {
    buttonPressed = true;
    lastButtonTime = millis();
    handleButtonPress();
    oldPosition = myEnc.read();
  } else if (!buttonState && buttonPressed) {
    buttonPressed = false;
    pressed = false;
    oldPosition = myEnc.read();
  }

  // Read touch
  touchVal = fastTouchRead(TOUCH_PIN);
  if (touchVal >= touchAverage + 5) {
    touched = true;
  } else if (touchVal <= touchAverage - 2) {
    touched = false;
  }

  // Encoder reading
  if ((millis() - lastButtonTime) > encoderLockoutTime) {
    newPosition = myEnc.read();
    if (newPosition != oldPosition) {
      if (newPosition % 4 == 0) {
        direction = (newPosition - oldPosition > 0) ? 1 : -1;
        mode += direction;
        if (mode >= numModes) mode = 0;
        else if (mode < 0) mode = numModes - 1;
        Serial.print("Mode changed to: ");
        Serial.println(mode);
        modeChanged = true;
      }
      oldPosition = newPosition;
    }
  }

  // Call mode selector
  modeSelector(mode);
}

// ===============================
// RF COMMUNICATION FUNCTIONS
// ===============================

void setupRF() {
  sprintf((char*)stanchionAddress, "STN%02d", VOICE_NUMBER);

  if (!radio.begin()) {
    Serial.println("⚠️ RF module not found!");
    rfConnected = false;
    return;
  }

  rfConnected = true;

  // CRITICAL: Match transmitter settings
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(RF_CHANNEL);
  radio.setRetries(15, 15);
  radio.enableDynamicPayloads();
  radio.setAutoAck(false);  // DISABLE ACK

  radio.openWritingPipe(broadcastAddress);
  radio.openReadingPipe(1, broadcastAddress);
  radio.openReadingPipe(2, stanchionAddress);

  radio.startListening();

  Serial.println("✓ RF Module initialized");
  Serial.print("  Stanchion #");
  Serial.println(VOICE_NUMBER);
  Serial.println("  MIDI enhanced mode");
}

void checkRFCommands() {
  if (!rfConnected) return;

  uint8_t pipe;
  if (radio.available(&pipe)) {
    radio.read(&rxPacket, sizeof(CommandPacket));

    // Check if message is for us
    if (rxPacket.targetID == 0 || rxPacket.targetID == VOICE_NUMBER) {

      Serial.print("📻 RF: ");

      switch (rxPacket.commandType) {

        case CMD_MODE_CHANGE:  // 0 - Original mode change
          mode = rxPacket.value1;
          if (mode >= numModes) mode = 0;
          Serial.print("Mode → ");
          Serial.println(mode);
          break;

        case CMD_BUTTON:  // 1 - Original button
          if (rxPacket.value1 == 1) {
            Serial.println("Button PRESSED");
            handleButtonPress();
          } else {
            Serial.println("Button RELEASED");
            pressed = false;
          }
          break;

        case CMD_KNOB:  // 2 - Original knob
          RFKnob = rxPacket.value1;
          Serial.print("RF Knob Value: ");
          Serial.println(RFKnob);
          break;

        case CMD_NOTE_ON:  // 3 - MIDI Note On
          Serial.print("Note ON: ");
          Serial.print(rxPacket.value1);
          Serial.print(" vel:");
          Serial.println(rxPacket.value2);
          handleNoteOn(rxPacket.value1, rxPacket.value2);
          break;

        case CMD_NOTE_OFF:  // 4 - MIDI Note Off
          Serial.print("Note OFF: ");
          Serial.println(rxPacket.value1);
          handleNoteOff(rxPacket.value1);
          break;

        case CMD_CC:  // 5 - MIDI CC
          Serial.print("CC");
          Serial.print(rxPacket.value1);
          Serial.print(": ");
          Serial.println(rxPacket.value2);
          handleCC(rxPacket.value1, rxPacket.value2);
          break;

        case CMD_PROGRAM_CHANGE:  // 6 - MIDI Program Change -> Mode
          Serial.print("Program Change → Mode ");
          Serial.println(rxPacket.value1);
          mode = rxPacket.value1 % numModes;  // Wrap to available modes
          break;

        case CMD_PITCH_BEND:  // 7 - MIDI Pitch Bend
          {
            int bendValue = rxPacket.value1 | (rxPacket.value2 << 7);
            Serial.print("Pitch Bend: ");
            Serial.println(bendValue);
            handlePitchBend(bendValue);
          }
          break;

        default:
          Serial.print("Unknown command type: ");
          Serial.println(rxPacket.commandType);
          break;
      }
    }
  }
}

// ===============================
// MIDI HANDLER FUNCTIONS
// ===============================

void handleNoteOn(byte note, byte velocity) {
  // ONLY store - no mode-specific logic
  currentMidiNote = note;
  currentMidiVelocity = velocity;
  midiNoteActive = true;
}

void handleNoteOff(byte note) {
  // ONLY clear flag
  if (note == currentMidiNote) {
    midiNoteActive = false;
  }
}

void handleCC(byte ccNumber, byte ccValue) {
  // ONLY store in array
  ccValues[ccNumber] = ccValue;
  ccChanged[ccNumber] = true;
}

void handlePitchBend(int bendValue) {
  // Handle pitch bend (0-16383, center = 8192)
  
  // Your custom pitch bend handling here
}

// ===============================
// CC HELPER FUNCTIONS
// ===============================

// Check if a specific CC has changed and get its value
bool getCCChanged(byte ccNumber, byte* value) {
  if (ccChanged[ccNumber]) {
    *value = ccValues[ccNumber];
    ccChanged[ccNumber] = false;  // Clear flag
    return true;
  }
  return false;
}

// Get current value of a CC without clearing the changed flag
byte getCCValue(byte ccNumber) {
  return ccValues[ccNumber];
}

// Clear all CC changed flags
void clearAllCCFlags() {
  for (int i = 0; i < 128; i++) {
    ccChanged[i] = false;
  }
}

// ===============================
// ORIGINAL FUNCTIONS (unchanged)
// ===============================

void handleButtonPress() {
  Serial.println("Button action triggered");
  buttonToggle = !buttonToggle;
  Serial.println(buttonToggle);

  buttonClickCount++;
  Serial.println(buttonClickCount);

  if (mode == 0) {
    if (ledValue == 0) {
      ledValue = ledMaxBrightness;
    } else {
      ledValue = 0;
    }
    analogWrite(LED_PIN, ledValue);
    Serial.print("LED → ");
    Serial.println(ledValue);
  }

  pressed = true;
}

unsigned long BPMtoMs(int BPM) {
  return 60000 / BPM;
}

void startRecording() {
  Serial.println("startRecording");
  if (SD.exists("RECORD.RAW")) {
    SD.remove("RECORD.RAW");
  }
  frec = SD.open("RECORD.RAW", FILE_WRITE);
  if (frec) {
    queue1.begin();
    rMode = 1;
  }
}

void continueRecording() {
  if (queue1.available() >= 2) {
    byte buffer[512];
    memcpy(buffer, queue1.readBuffer(), 256);
    queue1.freeBuffer();
    memcpy(buffer + 256, queue1.readBuffer(), 256);
    queue1.freeBuffer();
    frec.write(buffer, 512);
  }
}

void stopRecording() {
  Serial.println("stopRecording");
  queue1.end();
  if (rMode == 1) {
    while (queue1.available() > 0) {
      frec.write((byte *)queue1.readBuffer(), 256);
      queue1.freeBuffer();
    }
    frec.close();
  }
  rMode = 0;
}

void startPlaying() {
  Serial.println("startPlaying");
  playRaw1.play("RECORD.RAW");
  rMode = 2;
}

void continuePlaying() {
  if (!playRaw1.isPlaying()) {
    playRaw1.stop();
    Serial.println("STOPSTOPSTOPSTOPSTOP");
    delay(200);
    if (peak1.available()) {
      Serial.print("oldPeakVal = ");
      Serial.println(peak1.read());
    }
    rMode = 0;
  }
}

void stopPlaying() {
  Serial.println("stopPlaying");
  if (rMode == 2) playRaw1.stop();
  rMode = 0;
}

void playFile(const char *filename) {
  Serial.print("Playing file: ");
  Serial.println(filename);
  playRaw1.play(filename);
}

void touchCalibration(int numReadings) {
  analogWrite(R_PIN, 0);
  analogWrite(G_PIN, 255);
  analogWrite(B_PIN, 0);

  touchAverage = 0;
  for (int i = 0; i < numReadings; i++) {
    touchAverage = touchAverage + fastTouchRead(TOUCH_PIN);
    delay(100);

    Serial.print("touchAverage = ");
    Serial.println(touchAverage);
  }
  touchAverage = touchAverage / numReadings;

  Serial.print("Final touchAverage = ");
  Serial.println(touchAverage);

  analogWrite(R_PIN, 0);
  analogWrite(G_PIN, 0);
  analogWrite(B_PIN, 0);
  delay(500);
}

void statusBlink(int r, int g, int b, unsigned long blinkOnInterval, unsigned long blinkOffInterval) {

  if (blinkOn) {
    blinkInterval = blinkOnInterval;
  } else
    blinkInterval = blinkOffInterval;

  if (blinkTime > blinkInterval) {
    blinkTime = 0;
    if (blinkOn) {
      analogWrite(R_PIN, r / 2);
      analogWrite(G_PIN, g / 2);
      analogWrite(B_PIN, b / 2);
      blinkOn = false;
    } else {
      analogWrite(R_PIN, r);
      analogWrite(G_PIN, g);
      analogWrite(B_PIN, b);

      blinkOn = true;
    }
  }
}

void modeSelector(int modeNum) {

  switch (modeNum) {

    case 0:  //RED: Pushbotton Light + Audio Surveillance
      vcnl4200.setProxShutdown(true);
      envelope1.release(250);
      envelope1.noteOff();
      envelope2.noteOff();
      envelope3.noteOff();
      mixer1.gain(0, 1);
      mixer1.gain(3, 0);
      mixer1.gain(1, 50);

      analogWrite(R_PIN, 255);
      analogWrite(G_PIN, 0);
      analogWrite(B_PIN, 0);

      inLight(15);
      surveillance();

      break;

    case 2:  //dimBlue: proximity drum
      vcnl4200.setProxShutdown(false);
      envelope1.release(250);
      envelope1.noteOff();
      envelope2.noteOff();
      envelope3.noteOff();
      mixer1.gain(3, 0);
      mixer1.gain(0, 2);
      mixer1.gain(1, 1);

      analogWrite(R_PIN, 10);
      analogWrite(G_PIN, 17);
      analogWrite(B_PIN, 15);

      ProximityDrum();
      break;

    case 3:  //GREEN
      vcnl4200.setProxShutdown(true);
      envelope1.noteOff();
      mixer1.gain(0, 1);
      mixer1.gain(3, 0.05);

      analogWrite(R_PIN, 0);
      analogWrite(G_PIN, 255);
      analogWrite(B_PIN, 0);

      touchCount(0);
      outLight(3.5);
      break;

    case 4:  //BLUE
      vcnl4200.setProxShutdown(false);
      envelope1.release(250);
      envelope1.noteOff();
      envelope2.noteOff();
      envelope3.noteOff();
      mixer1.gain(0, 1);

      analogWrite(R_PIN, 0);
      analogWrite(G_PIN, 0);
      analogWrite(B_PIN, 255);

      melodyCells();
      outLight(1);
      break;

    case 5:  //PINK
      mixer1.gain(3, 0.15);
      mixer1.gain(0, 1);
      mixer3.gain(1, 0.20);
      envelope1.release(400);
      envelope1.noteOff();

      analogWrite(R_PIN, 200);
      analogWrite(G_PIN, 0);
      analogWrite(B_PIN, 150);

      GE();
      outLight(55);
      break;

    case 6:  //WHITE
      vcnl4200.setProxShutdown(true);
      envelope1.release(250);
      envelope1.noteOff();
      envelope2.noteOff();
      envelope3.noteOff();
      mixer1.gain(3, 0);
      mixer1.gain(0, 1);
      mixer1.gain(1, 1.5);

      analogWrite(R_PIN, 255);
      analogWrite(G_PIN, 255);
      analogWrite(B_PIN, 255);

      randomFlash(5000, 10000, 1);
      victory();
      outLight(1);
      break;

    case 7:  //FLASHING dim PINK
      envelope2.noteOff();
      mixer1.gain(3, 0.1);
      mixer1.gain(0, 2);
      mixer1.gain(1, 1);

      statusBlink(10, 5, 5, 80, 50);

      fadeSequence();
      break;

    case 8:  //Fast FLASHING Periwinkle
      OneRandomSoundEach();
      {
        static unsigned long lastLEDUpdate = 0;
        static byte lastRFKnob = 255;

        statusBlink(20, 20, 20, 40, 60);

        if (RFKnob != lastRFKnob && millis() - lastLEDUpdate > 10) {
          ledValue = RFKnob;
          analogWrite(LED_PIN, ledValue);
          lastRFKnob = RFKnob;
          lastLEDUpdate = millis();
        }
      }
      break;

    case 9:  //Dim ORANGE
      mixer3.gain(0, 0.8);
      statusBlink(10, 10, 1, 200, 5);
      RFPlayer();
      outLight(0.25);
      break;

    case 1:  //Yellowish - Flocking
      vcnl4200.setProxShutdown(true);
      mixer1.gain(0, 1);
      mixer1.gain(1, 2);
      envelope1.release(250);
      envelope1.noteOff();
      envelope3.noteOff();

      analogWrite(R_PIN, 255);
      analogWrite(G_PIN, 110);
      analogWrite(B_PIN, 10);

      randomFlash(10000, 26000, 2);
      outLight(1);
      break;
  }
}
