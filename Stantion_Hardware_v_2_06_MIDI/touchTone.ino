bool toneOn = false;   //keep track of whether the envelope was triggered
bool soundOn = false;  //keep track of whether we want the sound to play or not
float freqs[8] =  { 440.0, 250, 329.63, 349.23, 1760.0, 250, 329.63, 349.23};

void touchTone() {
  //read encoder pushbutton to toggle sound on/off
  if (bouncer.update()) {
    if (bouncer.read() == HIGH) {
      soundOn = !soundOn;
    }
  }
  if (soundOn) {


    if (touched) {
      if (!toneOn) {
        //int freqNum = map(touchVal, 20, 64, 1, 8);
        waveformMod1.begin(0.25, freqs[VOICE_NUMBER], WAVEFORM_SINE);
        envelope2.attack(1000);
        envelope2.decay(200);
        envelope2.sustain(0.8);
        envelope2.release(500);
        envelope2.noteOn();
        toneOn = true;
      } else {
        waveformMod1.frequency(freqs[VOICE_NUMBER]);
      }
    } else {
      envelope2.noteOff();
      toneOn = false;
    }
  }
}