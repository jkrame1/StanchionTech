bool notPlaying = true;           //keep track of presence of a body
elapsedMillis flash;              //a timer for the light
unsigned long flashLength = 100;  //a variable to store how long we want the light to be on
int soundSelector = 0;            //variable to store which sound style we are currently using
int maxSounds = 2;                //variable to store how many sound styles we want to use

void ProximityDrum() {
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

    if (soundSelector == 0) {

      switch (VOICE_NUMBER) {
        case 0:
          drum1.pitchMod(0.5);
          drum1.frequency(440.00*2);
          drum1.length(3000);
          drum1.noteOn();
          break;

        case 1:
          drum1.pitchMod(0.5);
          drum1.frequency(493.88*2);
          drum1.length(3000);
          drum1.noteOn();
          break;

        case 2:
          drum1.pitchMod(0.5);
          drum1.frequency(554.37*2);
          drum1.length(3000);
          drum1.noteOn();
          break;

        case 3:
          drum1.pitchMod(0.5);
          drum1.frequency(622.25*2);
          drum1.length(3000);
          drum1.noteOn();
          break;

        case 4:
          drum1.pitchMod(0.5);
          drum1.frequency(698.46*2);
          drum1.length(3000);
          drum1.noteOn();
          break;

          break;

        case 5:
          drum1.pitchMod(0.5);
          drum1.frequency(783.99*2);
          drum1.length(3000);
          drum1.noteOn();
          break;

        case 6:
          drum1.pitchMod(0.5);
          drum1.frequency(880.00*2);
          drum1.length(3000);
          drum1.noteOn();
          break;

        case 7:
          drum1.pitchMod(0.5);
          drum1.frequency(493.88*4);
          drum1.length(3000);
          drum1.noteOn();
          break;
      }
    }

    else if (soundSelector == 1) {
      playFile("HURTS.RAW");
      
    }

    flash = 0;
    ledValue = ledMaxBrightness;
    analogWrite(LED_PIN, ledValue);  //make the light super bright
    delay(20);                       //stabalize readings
  }

  else if (proxDrumData < debounceDistance) {
    notPlaying = true;  //reset flag after body leaves
  }
}




//////////OLD SWITCH HERE
/*
    switch (VOICE_NUMBER) {

      case 0:
        drum1.pitchMod(0.5);
        drum1.frequency(392);
        drum1.length(100);
        drum1.noteOn();
       // playFile("ZACH2.RAW");  // filenames are always uppercase 8.3 format
        break;

      case 1:
        drum1.pitchMod(0.5);
        drum1.frequency(2000);
        drum1.length(50);
        drum1.noteOn();
        break;

      case 2:
        drum1.pitchMod(0.5);
        drum1.frequency(3000);
        drum1.length(100);
        drum1.noteOn();
        break;

      case 3:
        drum1.pitchMod(0.9);
        drum1.frequency(5000);
        drum1.length(50);
        drum1.noteOn();
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
        playFile("TEXT.RAW");

        break;

      case 5:
        playFile("DEFAULT.RAW");
        break;

      case 6:
        playFile("CHARGE.RAW");  // filenames are always uppercase 8.3 format
        break;

      case 7:
        playFile("POPUP.RAW");
        break;
    }
    
    */