elapsedMillis waitTime;
unsigned long servDel = 30000;      // variable to store how long to wait before repeating a recording 35000
unsigned long serRecDur = 15000;  //recording duration 15000

// The file where data is recorded
File frec2;

// Remember which mode we're doing
int sMode = 0;  // 0=stopped, 1=recording, 2=playing

// Read the audio signal and store the peak value
void surveillance() {
  //delay(25);
  if (peak1.available()) {
    peakVal = peak1.read();
    // Serial.print("peakVal = ");
    // Serial.println(peakVal);
  }


  if (sMode == 0) {                  //if system is currently stopped
    if (peakVal >= peakThreshold) {  //check to see if the audio crosses the threshold
      Serial.println("<<<<<<<<<<<<<<<<<Peak Detected ");
      //Uncomment the Lines Below for Random Timing of the Recording Length and the time between record and playback.
      // serRecDur = random(5000, 10000);  //get a random record time
      // servDel = random(1000, 20000);    //get a random delay time


      if (sMode == 2) stopPlayingS();     //stop any playback
      if (sMode == 0) startRecordingS();  //start recording (this function sets the mode to 1: recording)
      recordTime = 0;                     //start a recording timer
    }
  }

  if (sMode == 1) {                 //check to see if system is recording
    if (recordTime >= serRecDur) {  //if it is recording, and the record timer has reached the desired length
      stopRecordingS();
      sMode = 3;
      waitTime = 0;
      //repeatCount = random(4, 12);

    } else {
      continueRecordingS();  //keep recording if the recordTime hasn't reached our desired duration
    }
  }


  if (sMode == 3) {
    if (waitTime >= servDel) {

      startPlayingS();
    }
  }


  if (sMode == 2) {

    continuePlayingS();  //if happily playing, then continue
  }
}


//////////////////Recorder Functions////////////
void startRecordingS() {
  Serial.println("startRecording");
  if (SD.exists("LISTEN.RAW")) {
    // The SD library writes new data to the end of the
    // file, so to start a new recording, the old file
    // must be deleted before new data is written.
    SD.remove("LISTEN.RAW");
  }
  frec2 = SD.open("LISTEN.RAW", FILE_WRITE);
  if (frec2) {
    queue1.begin();
    sMode = 1;
  }
}


// void startRecordingS() {
//   Serial.println("startRecording");
//   if (SD.exists("LISTEN.RAW")) {
//     SD.remove("LISTEN.RAW");
//   }
//   frec2 = SD.open("LISTEN.RAW", FILE_WRITE);
//   if (frec2) {
//     // Pre-allocate file space to reduce write latency
//     frec2.seek(serRecDur * 44); // Estimate file size (44.1kHz * 2 bytes)
//     frec2.write(0);
//     frec2.seek(0);

//     queue1.begin();
//     sMode = 1;
//   }
// }

//

// void continueRecordingS() {
//   if (queue1.available() >= 2) {
//     byte buffer[512];
//     memcpy(buffer, queue1.readBuffer(), 256);
//     queue1.freeBuffer();
//     memcpy(buffer + 256, queue1.readBuffer(), 256);
//     queue1.freeBuffer();

//     // TIME THE SD WRITE
//     elapsedMicros writeTime = 0;
//     frec2.write(buffer, 512);

//     // Check if write took too long
//     if (writeTime > 5802) {
//       Serial.print("SLOW SD WRITE: ");
//       Serial.print(writeTime);
//       Serial.println(" us");
//     }

//     // Check for buffer overflow
//     if (queue1.available() > 50) {
//       Serial.print("QUEUE OVERFLOW: ");
//       Serial.println(queue1.available());
//     }
//   }

//   // THIS IS IMPORTANT - check for buffer starvation
//   if (queue1.available() == 0 && sMode == 1) {
//     Serial.println("QUEUE EMPTY - AUDIO GAP!");
//   }
// }

void continueRecordingS() {
  // Wait until we have MORE blocks before writing
  // This gives the SD card time to catch up
  if (queue1.available() >= 8) {  // Changed from 2 to 8
    byte buffer[512];
    memcpy(buffer, queue1.readBuffer(), 256);
    queue1.freeBuffer();
    memcpy(buffer + 256, queue1.readBuffer(), 256);
    queue1.freeBuffer();

    frec2.write(buffer, 512);
  }
}

void stopRecordingS() {
  Serial.println("stopRecording");
  queue1.end();
  if (sMode == 1) {
    while (queue1.available() > 0) {
      frec2.write((byte *)queue1.readBuffer(), 256);
      queue1.freeBuffer();
    }
    frec2.close();
  }
  sMode = 0;
}


void startPlayingS() {

  Serial.println("startPlaying");
  playRaw1.play("LISTEN.RAW");
  delay(100);
  sMode = 2;
}

void continuePlayingS() {
  if (!playRaw1.isPlaying()) {
    playRaw1.stop();
    Serial.println("STOPSTOPSTOPSTOPSTOP");
    delay(100);               //a little debounce time to avoid feedback triggering
    if (peak1.available()) {  //get rid of any old readings before going back into the loop
      Serial.print("oldPeakVal = ");
      Serial.println(peak1.read());
    }
    sMode = 0;
  }
}

void stopPlayingS() {

  Serial.println("stopPlaying");
  if (sMode == 2) playRaw1.stop();
  sMode = 0;
}
