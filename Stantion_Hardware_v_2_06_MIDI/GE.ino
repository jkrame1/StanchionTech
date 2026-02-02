bool armed = false;  //keep track of presence of a body
int armDarkness = 8;
int brightThreshold2 = 12;

void GE() {
  envelope3.attack(1);
  envelope3.release(1400);


  // Read the ambient light sensor (ALS) data/////
  uint16_t alsData = vcnl4200.readALSdata();
  Serial.print("ALS Data: ");
  Serial.print(alsData);
  uint16_t whiteData = vcnl4200.readWhiteData();
  Serial.print(", White Data: ");
  Serial.println(whiteData);

  if (whiteData < armDarkness) {
    armed = true;
  }

  if (whiteData > brightThreshold2 && armed) {
    armed = false;
    envelope3.noteOn();
    switch (VOICE_NUMBER) {

      case 0:
        // playWav1.play("GEL.RAW");    // filenames are always uppercase 8.3 format
        playWav1.play("ZACH3.RAW");  // filenames are always uppercase 8.3 format

        break;

      case 1:
        // playWav1.play("GER.RAW");  // filenames are always uppercase 8.3 format
        playWav1.play("ZACH3.RAW");  // filenames are always uppercase 8.3 format

        break;

      case 2:
        //playWav1.play("GENR.RAW");  // filenames are always uppercase 8.3 format
        playWav1.play("ZACH3.RAW");  // filenames are always uppercase 8.3 format

        break;

      case 3:
        //playWav1.play("GEm12.RAW");  // filenames are always uppercase 8.3 format
        playWav1.play("ZACH3.RAW");  // filenames are always uppercase 8.3 format

        break;

      case 4:
        //playWav1.play("GEp12.RAW");  // filenames are always uppercase 8.3 format
        playWav1.play("ZACH3.RAW");  // filenames are always uppercase 8.3 format

        break;

      case 5:
        //playWav1.play("GEm24.RAW");  // filenames are always uppercase 8.3 format
        playWav1.play("ZACH3.RAW");  // filenames are always uppercase 8.3 format

        break;

      case 6:
        //playWav1.play("GEp7.RAW");  // filenames are always uppercase 8.3 format
        playWav1.play("ZACH3.RAW");  // filenames are always uppercase 8.3 format

        break;

      case 7:
        //playWav1.play("GEm24.RAW");  // filenames are always uppercase 8.3 format
        playWav1.play("ZACH3.RAW");  // filenames are always uppercase 8.3 format

        break;
    }
  }
}
