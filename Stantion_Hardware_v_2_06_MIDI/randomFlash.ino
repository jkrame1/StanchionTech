unsigned long interval = random(3000, 9000);  //random interval of flash/sound
//unsigned long interval = random(3000, 12000);  //random interval of flash/sound


elapsedMillis flasher;  //a timer for the light
elapsedMillis flashtimer;
//unsigned long flashLength = 100;  //a variable to store how long we want the light to be on


void randomFlash(int min, int max, int style) {
  //vcnl4200.setProxShutdown(false);
  //uint16_t proxDrumData = vcnl4200.readProxData();
  // Serial.println(proxDrumData);

  if (style == 1) {
    if (flasher >= flashLength) {
      ledValue = 0;
      analogWrite(LED_PIN, ledValue);  //make the light super bright
    }

    if (flashtimer > interval) {
      notPlaying = false;
      flashtimer = 0;
      interval = random(min, max);
      switch (VOICE_NUMBER) {

        case 0:
          // drum1.pitchMod(0.5);
          // drum1.frequency(392);
          // drum1.length(100);
          // drum1.noteOn();
          playFile("MATCH.RAW");  // filenames are always uppercase 8.3 format
          break;

        case 1:
          // drum1.pitchMod(0.5);
          // drum1.frequency(2000);
          // drum1.length(50);
          // drum1.noteOn();
          playFile("COMPLETE.RAW");

          break;

        case 2:
          // drum1.pitchMod(0.5);
          // drum1.frequency(3000);
          // drum1.length(100);
          // drum1.noteOn();
          playFile("RATE.RAW");
          break;

        case 3:
          // drum1.pitchMod(0.9);
          // drum1.frequency(5000);
          // drum1.length(50);
          // drum1.noteOn();
          playFile("EXTRACT.RAW");
          break;

        case 4:
          // playFile("CHARGE.RAW");  // filenames are always uppercase 8.3 format
          // delay(500);
          // playFile("DEFAULT.RAW");
          // delay(500);
          // playFile("PAY.RAW");
          // delay(500);
          // playFile("POPUP.RAW");
          // delay(500);
          // playFile("SENT.RAW");
          // delay(500);
          playFile("SELECT.RAW");

          break;

        case 5:
          playFile("SOLVE.RAW");
          break;

        case 6:
          playFile("TALLY.RAW");  // filenames are always uppercase 8.3 format
          break;

        case 7:
          playFile("VERIFY.RAW");
          break;
      }

      flasher = 0;
      ledValue = ledMaxBrightness;
      analogWrite(LED_PIN, ledValue);  //make the light super bright
      delay(20);                       //stabalize readings
    }
  }


  if (style == 2) {
    if (flasher >= flashLength) {
      ledValue = 0;
      analogWrite(LED_PIN, ledValue);  //make the light super bright
    }

    if (flashtimer > interval) {
      notPlaying = false;
      flashtimer = 0;
      interval = random(min, max);
      switch (VOICE_NUMBER) {

        case 0:
          drum1.pitchMod(0.5);
          drum1.frequency(392);
          drum1.length(100);
          drum1.noteOn();
          //playFile("MATCH.RAW");  // filenames are always uppercase 8.3 format
          break;

        case 1:
          drum1.pitchMod(0.5);
          drum1.frequency(2000);
          drum1.length(50);
          drum1.noteOn();
          //playFile("COMPLETE.RAW");

          break;

        case 2:
          drum1.pitchMod(0.5);
          drum1.frequency(3000);
          drum1.length(100);
          drum1.noteOn();
          // playFile("RATE.RAW");
          break;

        case 3:
          drum1.pitchMod(0.9);
          drum1.frequency(5000);
          drum1.length(50);
          drum1.noteOn();
          // playFile("REFRESH.RAW");
          break;

        case 4:
          // playFile("CHARGE.RAW");  // filenames are always uppercase 8.3 format
          // delay(500);
          // playFile("DEFAULT.RAW");
          // delay(500);
          // playFile("PAY.RAW");
          // delay(500);
          // playFile("POPUP.RAW");
          // delay(500);
          playFile("FLYBY.RAW");
          // delay(500);
          //playFile("SELECT.RAW");

          break;

        case 5:
          playFile("CHARGE.RAW");
          break;

        case 6:
          playFile("DEFAULT.RAW");  // filenames are always uppercase 8.3 format
          break;

        case 7:
          playFile("UPSWIPE1.RAW");
          break;
      }

      flasher = 0;
      ledValue = ledMaxBrightness;
      analogWrite(LED_PIN, ledValue);  //make the light super bright
      delay(20);                       //stabalize readings
    }
  }
}

void victory() {
  vcnl4200.setProxShutdown(false);
  uint16_t proxDrumData = vcnl4200.readProxData();
  //Serial.println(proxDrumData);

  //press button to select sound style
  if (bouncer.fell()) {
    soundSelector++;
    if (soundSelector > (maxSounds - 1)) {
      soundSelector = 0;
    }
  }


  if (flash >= flashLength) {
    ledValue = 0;
    analogWrite(LED_PIN, ledValue);  //make the light super bright
  }

  if (proxDrumData > detectionDistance && notPlaying) {
    notPlaying = false;
    playFile("NOTROBOT.RAW");

  }

  else if (proxDrumData < debounceDistance) {
    notPlaying = true;  //reset flag after body leaves
  }
}