elapsedMillis beat;
bool sound = false;
bool sounding = false;  //store if the note is on
bool lightOff = false;
int i = 0;
int maxCount = 7;

void blinkingMetro(int BPM) {
  //for BPMs that are each a little differenct:
  //BPM = BPM + (VOICE_NUMBER * 4);

  //for specific BPMs
  switch (VOICE_NUMBER) {

    case 0:
      BPM = 35;
      break;

    case 1:
      BPM = 135;
      break;

    case 2:
      BPM = 77;
      break;
    case 3:
      BPM = 35;
      break;
    case 4:
      BPM = 135;
      break;
    case 5:
      BPM = 77;
      break;
    case 6:
      BPM = 35;
      break;
    case 7:
      BPM = 135;
      break;
  }

  unsigned long beatTime = BPMtoMs(BPM);  //find the duration of a quater note

  //read encoder pushbutton to toggle soiund on/off

    if (bouncer.fell()) {
      sound = !sound;
    }


  if (beat < beatTime / 2) {
    if (lightOff == false) {
      ledValue = 0;
      analogWrite(LED_PIN, ledValue);
      lightOff = true;
      Serial.println("Light Off");
    }
    sounding = false;

  }

  else if (beat >= beatTime) {
    beat = 0;  //reset beat counter if beat is bigger than a quarter note
  }

  else {
    if (lightOff == true) {
      ledValue = ledMaxBrightness;
      analogWrite(LED_PIN, ledValue);  //if beat is >= beatTime/2, then turn light on
      lightOff = false;
      Serial.println("Light on");
    }
    if (sound) {                //if the sound is turned on then trigger the drum
      if (sounding == false) {  //if the drum is not currently triggered, then trigger it
        if (i != 0) {
          drum1.pitchMod(0.5 - (VOICE_NUMBER / 10));
          drum1.frequency(392 + (VOICE_NUMBER * 10));
          drum1.length(20 + (80 - VOICE_NUMBER * 10));
          drum1.noteOn();
        } else {
          drum1.pitchMod(0.5 - (VOICE_NUMBER / 10));
          drum1.frequency(600 + (VOICE_NUMBER * 10));
          drum1.length(20 + (80 - VOICE_NUMBER * 10));
          drum1.noteOn();
          Serial.println("accent");
        }
        sounding = true;
        i++;
        if (i > maxCount) {
          i = 0;
        }
      }
    }
  }
}
