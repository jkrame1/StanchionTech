void touchLight() {

  if (touched) {
    Serial.println(touchVal);
    ledValue = map(touchVal, 20, 64, 0, ledMaxBrightness);
    analogWrite(LED_PIN, ledValue);
  } else {
    ledValue = 0;
    analogWrite(LED_PIN, ledValue);
  }
}
