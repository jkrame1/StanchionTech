void outLight(float gain){
  amp2.gain(gain);
  if (outRMS.available()){

    ledValue = outRMS.read() * ledMaxBrightness;
    analogWrite(LED_PIN,ledValue);

  }
}

void inLight(float gain){
    amp1.gain(gain);
    if (rms1.available()){

    ledValue = rms1.read() * ledMaxBrightness;
    analogWrite(LED_PIN,ledValue);

  }
}