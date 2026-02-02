bool purrPlaying = false;


void touchPurr(int thresh) {
  envelope3.attack(750);
  envelope3.release(1500);
  if (purrPlaying == false) {
    playWav1.play("PURR.RAW");  // filenames are always uppercase 8.3 format
  }

  if (touched) {
    envelope3.noteOn();
  } else {
    envelope3.noteOff();
  }
}