bool playing2 = false;
unsigned long stanchionTimeDelayMax = 10000;
int delayVal2;

elapsedMillis sinceStart;

void OneRandomSoundEach() {

  // if (touched) {  //if touched, hold the tone
  //   Serial.println("touched");
  //   if (playing2 == false) {
  //     playing2 = true;
  //     sinceStart = 0;
  //   }
  // }

  //read encoder pushbutton to toggle sound on/off
  // if (bouncer.update()) {
  //   if (bouncer.read() == HIGH) {
  //     if (playing2 == false) {
  //       playing2 = true;
  //       sinceStart = 0;
  //       analogWrite(LED_PIN, 0);
  //     }
  //   }
  // }

//check if pushbutton is presesed once in the mode
  if (pressed) {
      if (playing2 == false) {
        playing2 = true;
        sinceStart = 0;
        analogWrite(LED_PIN, 0);
    }
  }

  if (sinceStart >= stanchionTimeDelayMax * 8) { //wait until all 8 have had a chance to go, then start the sequence again
    playing2 = true;
    sinceStart = 0;
  }

  if (playing2) {

    switch (VOICE_NUMBER) {
      case 0:
        delayVal2 = random((stanchionTimeDelayMax / 2), stanchionTimeDelayMax);  //select a delay time that takes between 4 and 10 seconds
        delay(delayVal2);
        analogWrite(LED_PIN, ledMaxBrightness);

        drum1.pitchMod(0.5);
        drum1.frequency(392);
        drum1.length(100);
        drum1.noteOn();
        delay(100);

        playing2 = false;
        analogWrite(LED_PIN, 0);
        break;

      case 1:
        delay(stanchionTimeDelayMax * 1);
        delayVal2 = random((stanchionTimeDelayMax / 2), stanchionTimeDelayMax);  //select a delay time that takes between 4 and 10 seconds
        delay(delayVal2);
        analogWrite(LED_PIN, ledMaxBrightness);

        drum1.pitchMod(0.5);
        drum1.frequency(392);
        drum1.length(100);
        drum1.noteOn();
        delay(100);

        playing2 = false;
        analogWrite(LED_PIN, 0);
        break;

      case 2:
        delay(stanchionTimeDelayMax * 2);
        delayVal2 = random((stanchionTimeDelayMax / 2), stanchionTimeDelayMax);  //select a delay time that takes between 4 and 10 seconds
        delay(delayVal2);
        analogWrite(LED_PIN, ledMaxBrightness);

        drum1.pitchMod(0.5);
        drum1.frequency(392);
        drum1.length(100);
        drum1.noteOn();
        delay(100);

        playing2 = false;
        analogWrite(LED_PIN, 0);
        break;

      case 3:
        delay(stanchionTimeDelayMax * 3);
        delayVal2 = random((stanchionTimeDelayMax / 2), stanchionTimeDelayMax);  //select a delay time that takes between 4 and 10 seconds
        delay(delayVal2);
        analogWrite(LED_PIN, ledMaxBrightness);

        drum1.pitchMod(0.5);
        drum1.frequency(392);
        drum1.length(100);
        drum1.noteOn();
        delay(100);

        playing2 = false;
        analogWrite(LED_PIN, 0);
        break;

      case 4:
        delay(stanchionTimeDelayMax * 4);
        delayVal2 = random((stanchionTimeDelayMax / 2), stanchionTimeDelayMax);  //select a delay time that takes between 4 and 10 seconds
        delay(delayVal2);
        analogWrite(LED_PIN, ledMaxBrightness);

        drum1.pitchMod(0.5);
        drum1.frequency(392);
        drum1.length(100);
        drum1.noteOn();
        delay(100);

        playing2 = false;
        analogWrite(LED_PIN, 0);
        break;

      case 5:
        delay(stanchionTimeDelayMax * 5);
        delayVal2 = random((stanchionTimeDelayMax / 2), stanchionTimeDelayMax);  //select a delay time that takes between 4 and 10 seconds
        delay(delayVal2);
        analogWrite(LED_PIN, ledMaxBrightness);

        drum1.pitchMod(0.5);
        drum1.frequency(392);
        drum1.length(100);
        drum1.noteOn();
        delay(100);

        playing2 = false;
        analogWrite(LED_PIN, 0);
        break;

      case 6:
        delay(stanchionTimeDelayMax * 6);
        delayVal2 = random((stanchionTimeDelayMax / 2), stanchionTimeDelayMax);  //select a delay time that takes between 4 and 10 seconds
        delay(delayVal2);
        analogWrite(LED_PIN, ledMaxBrightness);

        drum1.pitchMod(0.5);
        drum1.frequency(392);
        drum1.length(100);
        drum1.noteOn();
        delay(100);

        playing2 = false;
        analogWrite(LED_PIN, 0);
        break;

      case 7:
        delay(stanchionTimeDelayMax * 7);
        delayVal2 = random((stanchionTimeDelayMax / 2), stanchionTimeDelayMax);  //select a delay time that takes between 4 and 10 seconds
        delay(delayVal2);
        analogWrite(LED_PIN, ledMaxBrightness);

        drum1.pitchMod(0.5);
        drum1.frequency(392);
        drum1.length(100);
        drum1.noteOn();
        delay(100);

        playing2 = false;
        analogWrite(LED_PIN, 0);
        break;
    }
  }
}


// void fadeUpRandom(int minFadeTime, int maxFadeTime) {                                     //fade times in ms
//   delayVal2 = random((minFadeTime / ledMaxBrightness), (maxFadeTime / ledMaxBrightness));  //select a delay time that takes between 4 and 10 seconds
// }

// void fadeUp(int fadeTime) {
//   int brightnessChange = ledMaxBrightness - ledBrightness;
//   long unsigned int changeTimer = fadeTime / brightnessChange;
//   if (ledBrightness < ledMaxBrightness) {
//     if (sinceStart > changeTimer) {
//       analogWrite(LED_PIN, ledBrightness);
//       ledBrightness++;
//       sinceStart = 0;
//     }
//   }
// }
