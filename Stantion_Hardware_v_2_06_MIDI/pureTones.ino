int freqNum = 0;
//float freq[5] = { 110.0, 220.0, 880.0, 1760.0, 3520.0 };
float freq[8][5] = { { 440.0, 250, 329.63, 349.23, 1760.0 },
                     { 110.0, 220.0, 880.0, 1760.0, 3520.0 },
                     { 7040.0, 220.0, 880.0, 1760.0, 3520.0 },
                     { 210.0, 220.0, 880.0, 460, 500 },
                     { 220.0, 230, 240, 250, 260 },
                     { 210.0, 220.0, 880.0, 460, 500 },
                     { 220, 230, 240, 250, 480 },
                     { 110.0, 220.0, 880.0, 1760.0, 3520.0 } };
elapsedMillis toneTime;  //keep track of interval for tones

void pureTones(int BPM) {
  BPM = BPM + (VOICE_NUMBER * 4);       //make it so each box has a slightly differnt BPM
  unsigned long ms = BPMtoMs(BPM) * 4;  //get the value of a whole note with * 4

  freqNum = random(0, 4);  //randomly select a frequency from the array

 // if (touchRead(TOUCH_PIN) > (touchAverage + touchThreshold)) {  //if touched, hold the tone
    if (touched) {  //if touched, hold the tone
    Serial.println("touched");
    toneTime = ms - 30;
  }

  if (toneTime > ms) {
    waveformMod1.begin(0.25, freq[VOICE_NUMBER][freqNum], WAVEFORM_SINE);
    envelope2.attack(ms / 3);
    envelope2.decay(ms / 52);
    envelope2.sustain(0.8);
    envelope2.sustain(random(ms / 50, ms));
    envelope2.noteOn();
    toneTime = 0;
  }
}