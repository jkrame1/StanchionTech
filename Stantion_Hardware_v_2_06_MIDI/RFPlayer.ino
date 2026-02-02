//NEW MIDI Architacture
// MIDI-Enhanced RFPlayer - Proper Architecture

// RFPlayer state variables
bool voice1Playing = false;
float frequency = 220;
float targetFrequency = 220;
byte lastRFKnob = 0;

// RFPlayer-specific settings
int rfPlayerWaveform = WAVEFORM_SINE;
float rfPlayerVolume = 0.8;

int rPlayerAttack = 10;
int rPlayerDecay = 10;
float rPlayerSustain = 0.4;
int rPlayerRelease = 1000;


// ===============================
// RFPLAYER MODE FUNCTION (Mode 9)
// ===============================

void RFPlayer() {

  // === CHECK FOR CC2 (WAVEFORM) CHANGES ===
  byte waveformCC;
  if (getCCChanged(2, &waveformCC)) {
    // // Map CC value to waveform types
    // if (waveformCC < 21) {
    //   rfPlayerWaveform = WAVEFORM_SINE;
    //   Serial.println("RFPlayer Waveform: SINE");
    // } else if (waveformCC < 42) {
    //   rfPlayerWaveform = WAVEFORM_SAWTOOTH;
    //   Serial.println("RFPlayer Waveform: SAWTOOTH");
    // } else if (waveformCC < 63) {
    //   rfPlayerWaveform = WAVEFORM_SQUARE;
    //   Serial.println("RFPlayer Waveform: SQUARE");
    // } else if (waveformCC < 84) {
    //   rfPlayerWaveform = WAVEFORM_TRIANGLE;
    //   Serial.println("RFPlayer Waveform: TRIANGLE");
    // } else if (waveformCC < 105) {
    //   rfPlayerWaveform = WAVEFORM_PULSE;
    //   Serial.println("RFPlayer Waveform: PULSE");
    // } else {
    //   rfPlayerWaveform = WAVEFORM_SAWTOOTH_REVERSE;
    //   Serial.println("RFPlayer Waveform: SAWTOOTH_REVERSE");
    // }

    rfPlayerWaveform = map(waveformCC, 0, 127, 0, 12);
    waveformMod1.begin(1, frequency, rfPlayerWaveform);
    // Update waveform if currently playing
    // if (voice1Playing) {
    //   waveformMod1.begin(1, frequency, rfPlayerWaveform);
    // }
  }

  // === CHECK FOR ENVELOPE COMMANDS ===
  byte attackCC;   //cc3
  byte decayCC;    //cc4
  byte sustainCC;  //cc5
  byte releaseCC;  //cc6

  if (getCCChanged(3, &attackCC)) {
    rPlayerAttack = map(attackCC, 0, 127, 0, 3000);
    envelope2.attack(rPlayerAttack);
  }

  if (getCCChanged(4, &decayCC)) {
    rPlayerDecay = map(decayCC, 0, 127, 0, 2000);
    envelope2.decay(rPlayerDecay);
  }

  if (getCCChanged(5, &sustainCC)) {
    rPlayerSustain = sustainCC / 127.0;
    envelope2.sustain(rPlayerSustain);
  }

  if (getCCChanged(6, &releaseCC)) {
    rPlayerRelease = map(releaseCC, 0, 127, 0, 3000);
    envelope2.release(rPlayerRelease);
  }

  // === CHECK FOR CC7 (VOLUME) CHANGES ===
  byte volumeCC;
  if (getCCChanged(7, &volumeCC)) {
    rfPlayerVolume = volumeCC / 127.0;
    ///mixer3.gain(0, 0.1);
    waveformMod1.amplitude(rfPlayerVolume);
    Serial.print("RFPlayer Volume: ");
    Serial.println(rfPlayerVolume);
  }


  // === HANDLE MIDI NOTE CONTROL ===
  if (midiNoteActive) {
    // MIDI note is active - calculate target frequency from note number
    targetFrequency = 440.0 * pow(2.0, (currentMidiNote - 69) / 12.0);

    // Smooth interpolation toward target (glide/portamento effect)
    float glideSpeed = 0.05;  // Adjust: 0.01 = slow, 0.5 = fast
    frequency += (targetFrequency - frequency) * glideSpeed;

    // Update frequency if playing
    if (voice1Playing) {
      waveformMod1.frequency(frequency);
      waveformMod1.amplitude(rfPlayerVolume);
      // envelope2.sustain(rfPlayerVolume);
    }

    // Start note if not already playing
    if (!voice1Playing) {
      waveformMod1.begin(1, frequency, rfPlayerWaveform);
      envelope2.attack(rPlayerAttack);
      envelope2.decay(rPlayerDecay);
      envelope2.sustain(rPlayerSustain);
      envelope2.release(rPlayerRelease);
      envelope2.noteOn();
      voice1Playing = true;

      Serial.print("RFPlayer: Playing note ");
      Serial.print(currentMidiNote);
      Serial.print(" @ ");
      Serial.print(targetFrequency);
      Serial.println(" Hz");
    }

  } else {
    // === NO MIDI NOTE - FALL BACK TO RF KNOB CONTROL ===

    // Update target frequency when knob changes
    if (RFKnob != lastRFKnob) {
      targetFrequency = map(RFKnob, 0, 255, 100, 2000);
      lastRFKnob = RFKnob;
    }

    // Smooth interpolation toward target
    float glideSpeed = 0.005;
    frequency += (targetFrequency - frequency) * glideSpeed;

    if (pressed) {
      waveformMod1.frequency(frequency);
    }

    if (!voice1Playing) {
      if (pressed) {
        waveformMod1.begin(1, frequency, rfPlayerWaveform);
        envelope2.attack(rPlayerAttack);
        envelope2.decay(rPlayerDecay);
        envelope2.sustain(rPlayerSustain);
        envelope2.release(rPlayerRelease);
        envelope2.noteOn();
        voice1Playing = true;
      }
    }
  }

  // === RELEASE NOTE IF NOTHING IS ACTIVE ===
  if (voice1Playing) {
    if (!midiNoteActive && !pressed) {
      envelope2.noteOff();
      voice1Playing = false;
    }
  }
}


// // ===============================
// // EXAMPLE: ANOTHER MODE USING MIDI DIFFERENTLY
// // ===============================

// // Example Mode 5 - uses MIDI notes to trigger samples instead
// void GE() {

//   // Check for note on events in THIS mode
//   static byte lastProcessedNote = 255;

//   if (midiNoteActive && currentMidiNote != lastProcessedNote) {
//     // Different behavior: trigger different samples based on note
//     if (currentMidiNote >= 60 && currentMidiNote < 64) {
//       playWav1.play("SAMPLE1.RAW");
//       Serial.println("GE: Triggered SAMPLE1");
//     } else if (currentMidiNote >= 64 && currentMidiNote < 68) {
//       playWav1.play("SAMPLE2.RAW");
//       Serial.println("GE: Triggered SAMPLE2");
//     } else if (currentMidiNote >= 68 && currentMidiNote < 72) {
//       playWav1.play("SAMPLE3.RAW");
//       Serial.println("GE: Triggered SAMPLE3");
//     }

//     lastProcessedNote = currentMidiNote;
//   }

//   if (!midiNoteActive) {
//     lastProcessedNote = 255;  // Reset
//   }

//   // Check for CC changes specific to GE mode
//   byte ccVal;
//   if (getCCChanged(1, &ccVal)) {
//     // Maybe CC1 controls filter in this mode
//     float filterFreq = map(ccVal, 0, 127, 100, 5000);
//     filter1.frequency(filterFreq);
//     Serial.print("GE: Filter -> ");
//     Serial.println(filterFreq);
//   }

//   // Your existing GE code here...
//   outLight(55);
// }


// // ===============================
// // EXAMPLE: DRUM MODE USING MIDI
// // ===============================

// void ProximityDrum() {

//   // In this mode, MIDI notes trigger different drum sounds
//   static byte lastDrumNote = 255;
//   static bool drumTriggered = false;

//   if (midiNoteActive && !drumTriggered) {
//     // Map different notes to drum sounds
//     if (currentMidiNote == 36) {  // Kick
//       drum1.frequency(60);
//       drum1.length(150);
//       drum1.secondMix(0.0);
//       drum1.noteOn();
//       Serial.println("ProximityDrum: KICK");
//     } else if (currentMidiNote == 38) {  // Snare
//       drum1.frequency(200);
//       drum1.length(100);
//       drum1.secondMix(0.5);
//       drum1.noteOn();
//       Serial.println("ProximityDrum: SNARE");
//     } else if (currentMidiNote == 42) {  // Hi-hat
//       drum1.frequency(400);
//       drum1.length(50);
//       drum1.secondMix(0.8);
//       drum1.noteOn();
//       Serial.println("ProximityDrum: HI-HAT");
//     }

//     drumTriggered = true;
//     lastDrumNote = currentMidiNote;
//   }

//   if (!midiNoteActive) {
//     drumTriggered = false;
//   }

//   // Use CC for drum parameters
//   byte pitchCC;
//   if (getCCChanged(1, &ccVal)) {
//     float drumPitch = map(ccVal, 0, 127, 50, 500);
//     drum1.frequency(drumPitch);
//   }

//   // Your existing proximity drum code here...
// }
