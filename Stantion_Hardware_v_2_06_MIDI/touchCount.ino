int count = 0;        //variable to store the number of beeps that have happened
int countMax = 1;     //variable to store the number of beeps that should happen
int toneLength = 75;  //variable to store length of tone
bool wasTouched = false;
unsigned long repeatInterval = toneLength;

elapsedMillis repeatTimer;

//function that will create feedback based on how many times a stanchion is touched
void touchCount(int style) {
  style = buttonToggle;
  //initialize after selecting mode
  if (modeChanged) {
    countMax = 1;     //reset counter when mode changes
    toneLength = 75;  //reset tone length
    wasTouched = false;
  }
  modeChanged = false;

  // flashLength = toneLength;  //set the length of the LED flash to the length of the tone

  if (style == 0) {  //beeps
                     //turn off LED if flashLength is exceeded
    // if (flash >= flashLength) {
    //   ledValue = 0;
    //   analogWrite(LED_PIN, ledValue);  //make the light super bright
    // }

    repeatInterval = toneLength * 2;

    if (touched == true && wasTouched == false) {
      wasTouched = true;  //not doing anything
      Serial.println("TOUCHED");
      playNumber(countMax);
      while (count < countMax) {
        outLight(1);
        checkRFCommands();                         //check radio for changes
        // touchVal = fastTouchRead(TOUCH_PIN);       //get touch sensor
        // touched = (touchVal >= touchAverage + 5);  //store value
        // Encoder reading
        if ((millis() - lastButtonTime) > encoderLockoutTime) {
          newPosition = myEnc.read();
          if (newPosition != oldPosition) {
            if (newPosition % 4 == 0) {
              direction = (newPosition - oldPosition > 0) ? 1 : -1;
              mode += direction;
              if (mode >= numModes) mode = 0;
              else if (mode < 0) mode = numModes - 1;
              Serial.print("Mode changed to: ");
              Serial.println(mode);
            }
            oldPosition = newPosition;
          }
        }

        // Call mode selector

        if (mode != 3) {
          modeSelector(mode);  //check knob for changes
          Serial.println("BREAK");
          countMax = 0;     //reset counter
          toneLength = 75;  //reset tone length
          break;
        }
        //check button for changs
        //break out if mode changes

        //turn off LED if flashLength is exceeded

        // Serial.print("flash timer: ");
        // Serial.println(flash);
        // if (flash >= flashLength) {
        //   ledValue = 0;
        //   analogWrite(LED_PIN, ledValue);  //make the light super bright
        // }

        // //error sound attemp
        // if (touched == true && count > 0) {
        //   playNumber(random(60));
        // }

        if (repeatTimer >= repeatInterval) {
          //play sound
          Serial.print("play count: ");
          Serial.println(count);

          switch (VOICE_NUMBER) {
            case 0:
              drum1.pitchMod(0.5);
              drum1.frequency(1800);
              break;

            case 1:
              drum1.pitchMod(0.5);
              drum1.frequency(1900);
              break;

            case 2:
              drum1.pitchMod(0.5);
              drum1.frequency(2000);
              break;

            case 3:
              drum1.pitchMod(0.5);
              drum1.frequency(2100);
              break;

            case 4:
              drum1.pitchMod(0.5);
              drum1.frequency(2200);
              break;

            case 5:
              drum1.pitchMod(0.5);
              drum1.frequency(2300);
              break;

            case 6:
              drum1.pitchMod(0.5);
              drum1.frequency(2400);
              break;

            case 7:
              drum1.pitchMod(0.5);
              drum1.frequency(2500);
              break;
          }

          drum1.length(toneLength);
          drum1.noteOn();
          repeatTimer = 0;  //reset the note timer
          //light the LED
          // ledValue = ledMaxBrightness;
          // analogWrite(LED_PIN, ledValue);  //make the light super bright
          flash = 0;  //reset the flash timer
          count++;
          toneLength = toneLength + (count * 10);
         // delay(toneLength);
        }
      }

      countMax++;
      count = 0;
    }
    if (!touched) {
      wasTouched = false;
    }
  }

  //*****************************************Talking Only

  else if (style == 1) {  //speaking only
    //reset count if mode changes
    if (modeChanged) {
      countMax = 1;
      modeChanged = false;
    }

    if (touched == true && wasTouched == false) {
      playNumber(countMax);
      countMax++;
      wasTouched = true;
      if (countMax > 60) {
        countMax = 1;
      }
    }

    if (!touched) {
      wasTouched = false;
    }



  }

  else if (style == 2) {
    //none
  } else {  //default
  }
}

void playNumber(int number) {
  switch (number) {
    case 1:
      playFile("n1.RAW");
      break;

    case 2:
      playFile("n2.RAW");
      break;

    case 3:
      playFile("n3.RAW");
      break;

    case 4:
      playFile("n4.RAW");
      break;

    case 5:
      playFile("n5.RAW");
      break;

    case 6:
      playFile("n6.RAW");
      break;

    case 7:
      playFile("n7.RAW");
      break;

    case 8:
      playFile("n8.RAW");
      break;

    case 9:
      playFile("n9.RAW");
      break;

    case 10:
      playFile("n10.RAW");
      break;

    case 11:
      playFile("n11.RAW");
      break;

    case 12:
      playFile("n12.RAW");
      break;

    case 13:
      playFile("n13.RAW");
      break;

    case 14:
      playFile("n14.RAW");
      break;

    case 15:
      playFile("n15.RAW");
      break;

    case 16:
      playFile("n16.RAW");
      break;

    case 17:
      playFile("n17.RAW");
      break;

    case 18:
      playFile("n18.RAW");
      break;

    case 19:
      playFile("n19.RAW");
      break;

    case 20:
      playFile("n20.RAW");
      break;

    case 21:
      playFile("n21.RAW");
      break;

    case 22:
      playFile("n22.RAW");
      break;

    case 23:
      playFile("n23.RAW");
      break;

    case 24:
      playFile("n24.RAW");
      break;

    case 25:
      playFile("n25.RAW");
      break;

    case 26:
      playFile("n26.RAW");
      break;

    case 27:
      playFile("n27.RAW");
      break;

    case 28:
      playFile("n28.RAW");
      break;

    case 29:
      playFile("n29.RAW");
      break;

    case 30:
      playFile("n30.RAW");
      break;

    case 31:
      playFile("n31.RAW");
      break;

    case 32:
      playFile("n32.RAW");
      break;

    case 33:
      playFile("n33.RAW");
      break;

    case 34:
      playFile("n34.RAW");
      break;

    case 35:
      playFile("n35.RAW");
      break;

    case 36:
      playFile("n36.RAW");
      break;

    case 37:
      playFile("n37.RAW");
      break;

    case 38:
      playFile("n38.RAW");
      break;

    case 39:
      playFile("n39.RAW");
      break;

    case 40:
      playFile("n40.RAW");
      break;

    case 41:
      playFile("n41.RAW");
      break;

    case 42:
      playFile("n42.RAW");
      break;

    case 43:
      playFile("n43.RAW");
      break;

    case 44:
      playFile("n44.RAW");
      break;

    case 45:
      playFile("n45.RAW");
      break;

    case 46:
      playFile("n46.RAW");
      break;

    case 47:
      playFile("n47.RAW");
      break;

    case 48:
      playFile("n48.RAW");
      break;

    case 49:
      playFile("n49.RAW");
      break;

    case 50:
      playFile("n50.RAW");
      break;

    case 51:
      playFile("n51.RAW");
      break;

    case 52:
      playFile("n52.RAW");
      break;

    case 53:
      playFile("n53.RAW");
      break;

    case 54:
      playFile("n54.RAW");
      break;

    case 55:
      playFile("n55.RAW");
      break;

    case 56:
      playFile("n56.RAW");
      break;

    case 57:
      playFile("n57.RAW");
      break;

    case 58:
      playFile("n58.RAW");
      break;

    case 59:
      playFile("n59.RAW");
      break;

    case 60:
      playFile("n60.RAW");
      break;
  }
}