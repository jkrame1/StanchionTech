// Record sound as raw data to a SD card, and play it back.
//
// Requires the audio shield:
//   http://www.pjrc.com/store/teensy3_audio.html
//
// Three pushbuttons need to be connected:
//   Record Button: pin 0 to GND
//   Stop Button:   pin 1 to GND
//   Play Button:   pin 2 to GND
//
// This example code is in the public domain.

#include <Bounce.h>

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthNoisePink      pink1;          //xy=97,426
AudioInputI2S            i2s1;           //xy=172,128
AudioEffectEnvelope      envelope1;      //xy=239,400
AudioPlaySdRaw           playRaw1;       //xy=255,274
AudioSynthSimpleDrum     drum1;          //xy=277,225
AudioRecordQueue         queue1;         //xy=319,103
AudioAnalyzePeak         peak1;          //xy=319,146
AudioMixer4              mixer1;         //xy=501,272
AudioOutputI2S           i2s2;           //xy=670,249
AudioConnection          patchCord1(pink1, envelope1);
AudioConnection          patchCord2(i2s1, 0, queue1, 0);
AudioConnection          patchCord3(i2s1, 0, peak1, 0);
AudioConnection          patchCord4(envelope1, 0, mixer1, 2);
AudioConnection          patchCord5(playRaw1, 0, mixer1, 1);
AudioConnection          patchCord6(drum1, 0, mixer1, 0);
AudioConnection          patchCord7(mixer1, 0, i2s2, 0);
AudioControlSGTL5000     sgtl5000_1;     //xy=368,512
// GUItool: end automatically generated code


// For a stereo recording version, see this forum thread:
// https://forum.pjrc.com/threads/46150?p=158388&viewfull=1#post158388

// A much more advanced sound recording and data logging project:
// https://github.com/WMXZ-EU/microSoundRecorder
// https://github.com/WMXZ-EU/microSoundRecorder/wiki/Hardware-setup
// https://forum.pjrc.com/threads/52175?p=185386&viewfull=1#post185386

// Bounce objects to easily and reliably read the buttons
Bounce buttonRecord = Bounce(0, 8);
Bounce buttonStop = Bounce(1, 8);  // 8 = 8 ms debounce time
Bounce buttonPlay = Bounce(2, 8);


// which input on the audio shield will be used?
//const int myInput = AUDIO_INPUT_LINEIN;
const int myInput = AUDIO_INPUT_MIC;


float peakThreshold = 0.1;  //a trigger threshold between 0.0 - 1.0
float peakVal;               //variable to store peak reading

int repeatCount = 6;                  // number of times to repeat Recording
int playCount = 0;                    //keep track of play repeats
unsigned long repeatDuration = 750;   //number of ms before retriggering recording
unsigned long recordDuration = 750;  //length of recording
elapsedMillis repeatTime;             //keep track of playback repeat interval
elapsedMillis recordTime;             //keep track of the recording length

// Use these with the Teensy Audio Shield
#define SDCARD_CS_PIN 10
#define SDCARD_MOSI_PIN 7  // Teensy 4 ignores this, uses pin 11
#define SDCARD_SCK_PIN 14  // Teensy 4 ignores this, uses pin 13

// Use these with the Teensy 3.5 & 3.6 & 4.1 SD card
// #define SDCARD_CS_PIN BUILTIN_SDCARD
// #define SDCARD_MOSI_PIN 11  // not actually used
// #define SDCARD_SCK_PIN 13   // not actually used

// Use these for the SD+Wiz820 or other adaptors
//#define SDCARD_CS_PIN    4
//#define SDCARD_MOSI_PIN  11
//#define SDCARD_SCK_PIN   13


// Remember which mode we're doing
int mode = 0;  // 0=stopped, 1=recording, 2=playing

// The file where data is recorded
File frec;

void setup() {
  // Configure the pushbutton pins
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);

  // Audio connections require memory, and the record queue
  // uses this memory to buffer incoming audio.
  AudioMemory(90);

  // Enable the audio shield, select input, and enable output
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(myInput);
  sgtl5000_1.micGain(35);  //The input number is in decibels, from 0 to 63.
  sgtl5000_1.volume(0.95);

  mixer1.gain(0, 0.5);
  mixer1.gain(1, 8.0);
  mixer1.gain(2, 0.5);
  mixer1.gain(3, 1);


  // Initialize the SD card
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    // stop here if no SD card, but print a message
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
}


void loop() {
  // First, read the buttons and knobs
  buttonRecord.update();
  buttonStop.update();
  buttonPlay.update();
  //knob code for testing
  // repeatDuration = map(analogRead(A1), 0, 1023, 50, 6000);  //number of ms before retriggering recording
  // recordDuration = map(analogRead(A2), 0, 1023, 50, 6000);  //number of ms before retriggering recording
  // repeatCount = map(analogRead(A3), 0, 1023, 1, 20);


  // Next, read the audio signal and store the peak value
  if (peak1.available()) {
    peakVal = peak1.read();
    Serial.print("peakVal = ");
    Serial.println(peakVal);
  }


  if (mode == 0) {                   //if system is currently stopped
    if (peakVal >= peakThreshold) {  //check to see if the audio crosses the threshold
      playCount = 0;                 //set repeat counter to 0 when starting the recording
      Serial.println("<<<<<<<<<<<<<<<<<Peak Detected ");
      if (mode == 2) stopPlaying();     //stop any playback
      if (mode == 0) startRecording();  //start recording (this function sets the mode to 1: recording)
      recordTime = 0;                   //start a recording timer
    }
  }

  Serial.print("Repeat Time = ");
  Serial.println(repeatTime);

  if (mode == 1) {                      //check to see if system is recording
    if (recordTime > recordDuration) {  //if it is recording, and the record timer has reached the desired length
    repeatCount = random(4, 12);
      if (playCount < repeatCount) {
        if (mode == 1) stopRecording();
        if (mode == 0) startPlaying();
        repeatDuration = random(40, 1000);
        repeatTime = 0;
        playCount = 1;
        Serial.println(">>>>>>>>>>>>>>>>>>>>timer elapsed play");
        Serial.print("Play Count = ");
        Serial.println(playCount);
      } else {
        Serial.println(">>>>>>>>>>>>>>>>>>>>playStopped by count");
        stopPlaying();
      }
    } else {
      continueRecording();  //keep recording if the recordTime hasn't reached our desired duration
    }
  }


  // If we're playing or recording, carry on...
  // if (mode == 1) {
  //   continueRecording();
  // }

  if (mode == 2) {
    if (repeatTime > repeatDuration && playCount < repeatCount) {
      stopPlaying();
      startPlaying();
      repeatTime = 0;
      playCount++;
      Serial.print("<><><><><>Repeat Number ");
      Serial.println(playCount);

    }

    else {
      continuePlaying();  //if happily playing, then continue
    }
  }


    // Respond to button presses
  if (buttonRecord.fallingEdge()) {
    Serial.println("Record Button Press");
    repeatTime = 0;
    if (mode == 2) stopPlaying();
    if (mode == 0) startRecording();
  }
  if (buttonStop.fallingEdge()) {
    Serial.println("Stop Button Press");
    if (mode == 1) stopRecording();
    if (mode == 2) stopPlaying();
  }
  if (buttonPlay.fallingEdge()) {
    Serial.println("Play Button Press");
    if (mode == 1) stopRecording();
    if (mode == 0) startPlaying();
  }

  // // when using a microphone, continuously adjust gain
  // if (myInput == AUDIO_INPUT_MIC) adjustMicLevel();

  //delay(5);  //stabalize serial
}


void startRecording() {
  Serial.println("startRecording");
  if (SD.exists("RECORD.RAW")) {
    // The SD library writes new data to the end of the
    // file, so to start a new recording, the old file
    // must be deleted before new data is written.
    SD.remove("RECORD.RAW");
  }
  frec = SD.open("RECORD.RAW", FILE_WRITE);
  if (frec) {
    queue1.begin();
    mode = 1;
  }
}

void continueRecording() {
  if (queue1.available() >= 2) {
    byte buffer[512];
    // Fetch 2 blocks from the audio library and copy
    // into a 512 byte buffer.  The Arduino SD library
    // is most efficient when full 512 byte sector size
    // writes are used.
    memcpy(buffer, queue1.readBuffer(), 256);
    queue1.freeBuffer();
    memcpy(buffer + 256, queue1.readBuffer(), 256);
    queue1.freeBuffer();
    // write all 512 bytes to the SD card
    //elapsedMicros usec = 0;
    frec.write(buffer, 512);
    // Uncomment these lines to see how long SD writes
    // are taking.  A pair of audio blocks arrives every
    // 5802 microseconds, so hopefully most of the writes
    // take well under 5802 us.  Some will take more, as
    // the SD library also must write to the FAT tables
    // and the SD card controller manages media erase and
    // wear leveling.  The queue1 object can buffer
    // approximately 301700 us of audio, to allow time
    // for occasional high SD card latency, as long as
    // the average write time is under 5802 us.
    //Serial.print("SD write, us=");
    //Serial.println(usec);
  }
}

void stopRecording() {
  Serial.println("stopRecording");
  queue1.end();
  if (mode == 1) {
    while (queue1.available() > 0) {
      frec.write((byte*)queue1.readBuffer(), 256);
      queue1.freeBuffer();
    }
    frec.close();
  }
  mode = 0;
}


void startPlaying() {
  Serial.println("startPlaying");
  playRaw1.play("RECORD.RAW");
  mode = 2;
}

void continuePlaying() {
  if (!playRaw1.isPlaying()) {
    playRaw1.stop();
    Serial.println("STOPSTOPSTOPSTOPSTOP");
    delay(100);               //a little debounce time to avoid feedback triggering
    if (peak1.available()) {  //get rid of any old readings before going back into the loop
      Serial.print("oldPeakVal = ");
      Serial.println(peak1.read());
    }
    mode = 0;
  }
}

void stopPlaying() {
  Serial.println("stopPlaying");
  if (mode == 2) playRaw1.stop();
  mode = 0;
}

void adjustMicLevel() {
  // TODO: read the peak1 object and adjust sgtl5000_1.micGain()
  // if anyone gets this working, please submit a github pull request :-)
}

void peakDetection() {
  if (!playRaw1.isPlaying()) {  //check to see if a recording is currently playing back
  }
}

void voiceRepeat() {
  for (int i; i < repeatCount; i++) {
  }
}
