unsigned long waveTime = 1000;  //create a variable to store a changing wave time
elapsedMillis noiseTime;        //create a timer for useful stuff
bool waveOff = true;            //flag to keep track of wether or not a noise wave is playing



void noiseMachine() {

  if (VOICE_NUMBER % 2 == 0) {  //if even, make LPF
    mixer5.gain(0, 0.8);          //LPF
    mixer5.gain(1, 0);          //BPF
    mixer5.gain(2, 0);          //HPF
    mixer5.gain(3, 0);          //No Connection
  }

  else {                //if odd, Make BPF
    mixer5.gain(0, 0);  //LPF
    mixer5.gain(1, 0.8);  //BPF
    mixer5.gain(2, 0);  //HPF
    mixer5.gain(3, 0);  //No Connection
  }

  if (noiseTime >= waveTime) {
    if (waveOff) {
      waveTime = random(1000, 15000);
      filter1.frequency(random(80, 6000));
      envelope1.attack(waveTime / 2);
      envelope1.sustain(1);
      envelope1.release(waveTime / 2);
      mixer1.gain(3, 0);
      mixer1.gain(0, 1);
      mixer1.gain(2, 1);
      pink1.amplitude(0.4);
      envelope1.noteOn();
      waveOff = false;
      noiseTime = 0;
    } else {
      envelope1.noteOff();
      waveOff = true;
      noiseTime = 0;
    }
  }
}