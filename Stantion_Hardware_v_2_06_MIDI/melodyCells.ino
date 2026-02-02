int brightThreshold = 13000;  //set a brigthness threshold that resets and holds the pattern
float melFreq[] = { 440.0, 493.88, 523.25, 587.33, 659.26, 698.46, 783.99, 880 };
int freqCounter = 0;
bool stoppedPlaying = true;           //keep track of presence of a body


void melodyCells() {

  //////////proximity stuff///////////////////////

  uint16_t proxData = vcnl4200.readProxData();
  //Serial.print("Prox Data: ");
  //Serial.println(proxData);

  //delay(100);
  if (proxData > detectionDistance && stoppedPlaying) {
    stoppedPlaying = false;
    freqCounter++;
    if (freqCounter >= 8) {
      freqCounter = 0;
    }
  }

  if (proxData < 9) {
    stoppedPlaying = true;  //reset flag after body leaves
  }
  ///////////////////////////////////////////////


  // Read the ambient light sensor (ALS) data/////
  uint16_t alsData = vcnl4200.readALSdata();
  Serial.print("ALS Data: ");
  Serial.print(alsData);
  uint16_t whiteData = vcnl4200.readWhiteData();
  Serial.print(", White Data: ");
  Serial.println(whiteData);


  // if (alsData > brightThreshold) {
  //   drumTime = 495;  //hold drum pattern at start if light is bright
  //   drumBeatCount = 0;
  // }


  // //fade out led if no note
  // if (flashTime > noteLength / 20) {
  //   ledBrightness = ledBrightness - 2;
  //   if(ledBrightness <= 0){
  //     ledBrightness = 0;
  //   }
  // }
  // analogWrite(LED_PIN, ledBrightness);

  if (drumTime > BPMtoMs(80)) { //was 500 in v 1.8


    drum1.pitchMod(0.5);
    drum1.frequency(melFreq[freqCounter]);
    noteLength = random(450, 2000);
    drum1.length(noteLength);
    drum1.secondMix(0.1);
    drum1.noteOn();
    // ledBrightness = ledMaxBrightness;
    // analogWrite(LED_PIN, ledBrightness);
    // flashTime = 0;


    drumTime = 0;
  }
}






/* old code
int brightThreshold = 13000; //set a brigthness threshold that resets and holds the pattern

void melodyCells() {

  //////////proximity stuff///////////////////////

  uint16_t proxData = vcnl4200.readProxData();
  //Serial.print("Prox Data: ");
  //Serial.println(proxData);

  //delay(100);

  if (proxData > 20 && debounce > drumBounceTime) {
    drum1.frequency(392);
    drum1.length(1000);
    drum1.noteOn();
    debounce = 0;
  }

  ///////////////////////////////////////////////


  // Read the ambient light sensor (ALS) data/////
  uint16_t alsData = vcnl4200.readALSdata();
  Serial.print("ALS Data: ");
  Serial.print(alsData);
  uint16_t whiteData = vcnl4200.readWhiteData();
  Serial.print(", White Data: ");
  Serial.println(whiteData);


  if (alsData > brightThreshold) {
    drumTime = 495;  //hold drum pattern at start if light is bright
    drumBeatCount = 0;
  }


  //fade out led if no note
  if (flashTime > noteLength / 20) {
    ledBrightness = ledBrightness - 3;
  }
  analogWrite(LED_PIN, ledBrightness);

  if (drumTime > 500) {

    switch (drumBeatCount) {
      case 0:
        drum1.pitchMod(0.5);
        drum1.frequency(440);
        noteLength = random(450, 2000);
        drum1.length(noteLength);
        drum1.secondMix(0.1);
        drum1.noteOn();
        ledBrightness = 255;
        analogWrite(LED_PIN, ledBrightness);
        flashTime = 0;
        break;

      case 1:
        drum1.pitchMod(0.95);
        drum1.frequency(random(250, 252));
        noteLength = random(15, 45);
        drum1.length(noteLength);
        drum1.secondMix(0.1);
        drum1.noteOn();
        ledBrightness = 255;
        analogWrite(LED_PIN, ledBrightness);
        flashTime = 0;
        break;

      case 2:
        drum1.pitchMod(0.5);
        drum1.frequency(329.63);
        noteLength = random(20, 2500);
        drum1.length(noteLength);
        drum1.secondMix(0.1);
        drum1.noteOn();
        ledBrightness = 255;
        analogWrite(LED_PIN, ledBrightness);
        flashTime = 0;
        break;

      case 3:
        drum1.pitchMod(0.5);
        drum1.frequency(random(1000, 1000));
        noteLength = random(450, 1080);
        drum1.length(noteLength);
        drum1.secondMix(0.1);
        //drum1.noteOn();
        break;

      case 4:
        drum1.pitchMod(0.5);
        drum1.frequency(440);
        noteLength = random(450, 2000);
        drum1.length(noteLength);
        drum1.secondMix(0.1);
        drum1.noteOn();
        ledBrightness = 255;
        analogWrite(LED_PIN, ledBrightness);
        flashTime = 0;
        break;

      case 5:
        drum1.pitchMod(0.5);
        drum1.frequency(349.23);
        noteLength = 2000;
        drum1.length(noteLength);
        drum1.secondMix(0.1);
        drum1.noteOn();
        ledBrightness = 255;
        analogWrite(LED_PIN, ledBrightness);
        flashTime = 0;
        break;

      case 6:
        drum1.pitchMod(0.5);
        drum1.frequency(329.63);
        noteLength = random(20, 2500);
        drum1.length(noteLength);
        drum1.secondMix(0.1);
        drum1.noteOn();
        ledBrightness = 255;
        analogWrite(LED_PIN, ledBrightness);
        flashTime = 0;
        break;

      case 7:
        drum1.pitchMod(0.5);
        drum1.frequency(329.63);
        noteLength = random(20, 2500);
        drum1.length(noteLength);
        drum1.secondMix(0.1);
        drum1.noteOn();
        ledBrightness = 255;
        analogWrite(LED_PIN, ledBrightness);
        flashTime = 0;
        break;
    }

    drumBeatCount++;

    if (drumBeatCount > patternLength) {
      drumBeatCount = 0;
    }

    drumTime = 0;
  }
}*/