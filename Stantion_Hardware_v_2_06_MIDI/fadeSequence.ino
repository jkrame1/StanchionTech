
bool playing = false;
int delayVal = 0;
bool done = false;


void fadeSequence() {

  // if (touched) {  //if touched, hold the tone
  //   Serial.println("touched");
  //   if (playing == false) {
  //     playing = true;
  //     sinceStart = 0;
  //   }
  // }

  // //read encoder pushbutton to toggle sound on/off
  // if (bouncer.update()) {
  //   if (bouncer.read() == HIGH) {
  //     if (playing == false) {
  //       playing = true;
  //       sinceStart = 0;
  //       analogWrite(LED_PIN, 0);
  //     }
  //   }
  // }
  if (modeChanged) {
    done = false;
  }
  //check if pushbutton is presesed once in the mode
  if (pressed == true) {
    if (playing == false) {
      playing = true;
      pressed = false;
      sinceStart = 0;
      analogWrite(LED_PIN, 0);
    }
  }

  if (playing == true) {
    Serial.println("Fade Sequence started");
    if (done == false) {
      switch (VOICE_NUMBER) {
        case 0:
          delayVal = random(4000 / ledMaxBrightness, 10000 / ledMaxBrightness);  //select a delay time that takes between 4 and 10 seconds

          for (int i = 0; i < ledMaxBrightness; i++) {
            ledBrightness = i;
            analogWrite(LED_PIN, ledBrightness);
            delay(delayVal);
          }

          delay(random(25000, 40000));

          // delayVal = random((5000 / ledMaxBrightness), (10000 / ledMaxBrightness));

          for (int i = ledMaxBrightness; i > 0; i--) {
            ledBrightness = i;
            analogWrite(LED_PIN, ledBrightness);
            delay(delayVal);
          }
          playing = false;
          done = true;

          analogWrite(LED_PIN, 0);
          break;

        case 1:
          delay(60000 * 1);
          delayVal = random((4000 / ledMaxBrightness), (10000 / ledMaxBrightness));  //select a delay time that takes between 4 and 10 seconds
          for (int i = 0; i < ledMaxBrightness; i++) {
            ledBrightness = i;
            analogWrite(LED_PIN, ledBrightness);
            delay(delayVal);
          }

          delay(random(25000, 40000));  //stay lit

          delayVal = random((4000 / ledMaxBrightness), (10000 / ledMaxBrightness));

          for (int i = ledMaxBrightness; i > 0; i--) {
            ledBrightness = i;
            analogWrite(LED_PIN, ledBrightness);
            delay(delayVal);
          }
          playing = false;
          done = true;

          analogWrite(LED_PIN, 0);
          break;

        case 2:
          delay(60000 * 2);
          delayVal = random(4000 / ledMaxBrightness, 10000 / ledMaxBrightness);  //select a delay time that takes between 4 and 10 seconds

          for (int i = 0; i < ledMaxBrightness; i++) {
            ledBrightness = i;
            analogWrite(LED_PIN, ledBrightness);
            delay(delayVal);
          }

          delay(random(25000, 40000));

          delayVal = random((4000 / ledMaxBrightness), (10000 / ledMaxBrightness));

          for (int i = ledMaxBrightness; i > 0; i--) {
            ledBrightness = i;
            analogWrite(LED_PIN, ledBrightness);
            delay(delayVal);
          }
          playing = false;
          analogWrite(LED_PIN, 0);
          done = true;

          break;

        case 3:
          delay(60000 * 3);
          delayVal = random(4000 / ledMaxBrightness, 10000 / ledMaxBrightness);  //select a delay time that takes between 4 and 10 seconds

          for (int i = 0; i < ledMaxBrightness; i++) {
            ledBrightness = i;
            analogWrite(LED_PIN, ledBrightness);
            delay(delayVal);
          }

          delay(random(25000, 40000));

          delayVal = random((4000 / ledMaxBrightness), (10000 / ledMaxBrightness));
          for (int i = ledMaxBrightness; i > 0; i--) {
            ledBrightness = i;
            analogWrite(LED_PIN, ledBrightness);
            delay(delayVal);
          }
          playing = false;
          done = true;

          analogWrite(LED_PIN, 0);
          break;

        case 4:
          delay(60000 * 4);
          delayVal = random(4000 / ledMaxBrightness, 10000 / ledMaxBrightness);  //select a delay time that takes between 4 and 10 seconds

          for (int i = 0; i < ledMaxBrightness; i++) {
            ledBrightness = i;
            analogWrite(LED_PIN, ledBrightness);
            delay(delayVal);
          }

          delay(random(25000, 40000));

          delayVal = random((4000 / ledMaxBrightness), (10000 / ledMaxBrightness));
          for (int i = ledMaxBrightness; i > 0; i--) {
            ledBrightness = i;
            analogWrite(LED_PIN, ledBrightness);
            delay(delayVal);
          }
          playing = false;
          done = true;

          analogWrite(LED_PIN, 0);
          break;

        case 5:
          delay(60000 * 5);
          delayVal = random(4000 / ledMaxBrightness, 10000 / ledMaxBrightness);  //select a delay time that takes between 4 and 10 seconds

          for (int i = 0; i < ledMaxBrightness; i++) {
            ledBrightness = i;
            analogWrite(LED_PIN, ledBrightness);
            delay(delayVal);
          }

          delay(random(25000, 40000));

          delayVal = random((4000 / ledMaxBrightness), (10000 / ledMaxBrightness));
          for (int i = ledMaxBrightness; i > 0; i--) {
            ledBrightness = i;
            analogWrite(LED_PIN, ledBrightness);
            delay(delayVal);
          }
          playing = false;
          done = true;

          analogWrite(LED_PIN, 0);
          break;

        case 6:
          delay(60000 * 6);
          delayVal = random(4000 / ledMaxBrightness, 10000 / ledMaxBrightness);  //select a delay time that takes between 4 and 10 seconds

          for (int i = 0; i < ledMaxBrightness; i++) {
            ledBrightness = i;
            analogWrite(LED_PIN, ledBrightness);
            delay(delayVal);
          }

          delay(random(25000, 40000));

          delayVal = random((4000 / ledMaxBrightness), (10000 / ledMaxBrightness));
          for (int i = ledMaxBrightness; i > 0; i--) {
            ledBrightness = i;
            analogWrite(LED_PIN, ledBrightness);
            delay(delayVal);
          }
          playing = false;
          done = true;

          analogWrite(LED_PIN, 0);
          break;

        case 7:
          delay(60000 * 7);
          delayVal = random(4000 / ledMaxBrightness, 10000 / ledMaxBrightness);  //select a delay time that takes between 4 and 10 seconds

          for (int i = 0; i < ledMaxBrightness; i++) {
            ledBrightness = i;
            analogWrite(LED_PIN, ledBrightness);
            delay(delayVal);
          }

          delay(random(25000, 40000));

          delayVal = random((4000 / ledMaxBrightness), (10000 / ledMaxBrightness));
          for (int i = ledMaxBrightness; i > 0; i--) {
            ledBrightness = i;
            analogWrite(LED_PIN, ledBrightness);
            delay(delayVal);
          }
          playing = false;
          done = true;

          analogWrite(LED_PIN, 0);
          break;
      }
    }
  }
}


/*
//===========Alternate Fade Sequence without delays=============

//Try some idea here where at startup, a random number is selected, then another,then another

int randomFadeUp[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };  //array to store random fade up times
int randomFadeDown[8[ = {0,0,0,0,0,0,0,0}; //array to store random fade down times
elapsedMillis programTime; //a timer to keep track of time between events 
bool fadeUp, fadeDown, hold; //state flags to decide if we are fading up, fading down, or holding

void fadeSequence(){
  //Check to see if we have changed into this mode
  //This if statement acts as a Setup function for this function
  if (modeChanged) {
    for (int i = 0, i <= 7, i++) {
      randomFadeUp[i] = random(4000, 10000);
      randomFadeDown[i] = random(4000, 10000);
    }

    ledValue = 0;  //start with LED off

    if (VOICE_NUMBER == 0) {  //set state for the first stanchion to start fading up
      fadeUp = true;
      fadeDown = false;
      hold = false;
    }

    else {  //set all other stanchions to hold
      fadeUp = false;
      fadeDown = false;
      hold = true;
    }
  }
  //End our pseudo setup

  switch (VOICE_NUMBER) {
    case 0:
      //fading works by incrementing brightness over time
      //we find how much time to wait between each increment by taking the total fade time
      //and dividing it by the difference in brightness values that need to be covered.
      //for example, if a random fade needs to take 4000ms and the max brightness is 200, then the fade
      //increments 4000/200, or every 20ms. to cover the same number of level in 10s we would pause 50ms between
      //each increment
      unsigned long fadeUpInterval = randomFadeUp[VOICE_NUMBER] / ledMaxBrightness;
      unsigned long fadeDownInterval = randomFadeDown[VOICE_NUMBER] / ledMaxBrightness;
      unsigned long waitInterval = 0;
      unsigned long holdInterval = 40000;

      if (fadeUp) {
        if (programTime > fadeUpInterval) { //check to see if enought time has passed 
          if (ledValue <= ledMaxBrightness) { //if we are not yet at max brightness
            ledValue++;                       //then increment up
            analogWrite(ledValue);            //and make the light brighter
          } else {                            //if we reached or exceed max brightness
            fadeUp = false;                   //change out of fade up state
            hold = true;                      //and move into hold state
          }
          programTime = 0; //reset timer for next interval
        }
      }
      break;

    case 2:
      if {}

      break;
  }

}
*/
