/*

USES VARIABLES FROM PROXIMITY DRUM
bool notPlaying = true;           //keep track of presence of a body
elapsedMillis flash;              //a timer for the light
unsigned long flashLength = 100;  //a variable to store how long we want the light to be on
int soundSelector = 0;            //variable to store which sound style we are currently using
int maxSounds = 2;                //variable to store how many sound styles we want to use
*/


void securityCheckpoint() {
  vcnl4200.setProxShutdown(false);
  uint16_t proxDrumData = vcnl4200.readProxData();
  Serial.println(proxDrumData);

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
    ledValue = ledMaxBrightness;
    analogWrite(LED_PIN, ledValue);  //make the light super bright
    // delay(30);
    // ledValue = 0;
    // analogWrite(LED_PIN, ledValue);  //make the light super bright
    delay(random(500, 3000));       //random delay

    int whatSound = random(5);

    if ((VOICE_NUMBER % 2) == 1) { //for odd-numbered stanchions (EVEN IF YOU ARE COUNTING FROM 1), have a rejection sound
      Serial.println("ODD");
      if (whatSound == 4) {
        playFile("HURTS.RAW");
        Serial.println("Hurts");
      }

      else if (whatSound <= 2) {
        drum1.pitchMod(0.5);
        drum1.frequency(880);
        drum1.length(100);
        drum1.noteOn();
        delay(150);
        drum1.noteOn();
        Serial.println("left");
      }

      else {
        drum1.pitchMod(0.5);
        drum1.frequency(1760);
        drum1.length(100);
        drum1.noteOn();
        Serial.println("right");
      }
    }

    else if (VOICE_NUMBER % 2 == 0) {
      Serial.println("EVEN");

      if (whatSound == 4) {  //no going back on odd stantions
        drum1.pitchMod(0.5);
        drum1.frequency(880);
        drum1.length(100);
        drum1.noteOn();
        delay(150);
        drum1.noteOn();
        Serial.println("left");
      }

      else if (whatSound <= 2) {
        drum1.pitchMod(0.5);
        drum1.frequency(880);
        drum1.length(100);
        drum1.noteOn();
        delay(150);
        drum1.noteOn();
        Serial.println("left");
      }

      else {
        drum1.pitchMod(0.5);
        drum1.frequency(1760);
        drum1.length(100);
        drum1.noteOn();
        Serial.println("right");
      }
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