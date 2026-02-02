//Local Time Eye
//This is a grandular freeze that self-triggers

#include <Audio.h>
#include "AudioEffectFreezeGrain.h"
#include <Bounce.h>

#include <SPI.h>
#include <RF24.h>

#include <WS2812Serial.h>  // for Neopixels

//********Variables and objects for Neopixels
const int numled = 3;
const int pixelPin = 14;

byte drawingMemory[numled * 3];          //  3 bytes per LED
DMAMEM byte displayMemory[numled * 12];  // 12 bytes per LED

WS2812Serial leds(numled, displayMemory, drawingMemory, pixelPin, WS2812_GRB);  //create led object

AudioInputI2S input;
AudioOutputI2S output;
AudioEffectFreezeGrain freezeEffect;
AudioConnection patchCord1(input, 0, freezeEffect, 0);
AudioConnection patchCord2(freezeEffect, 0, output, 0);
AudioControlSGTL5000 sgtl5000;

#define FREEZE_BUTTON_PIN 0
#define AUTO_BUTTON_PIN 1

Bounce freezeButton(FREEZE_BUTTON_PIN, 10);
Bounce autoButton(AUTO_BUTTON_PIN, 10);

const int knobGrainPin = A1;
const int knobMixPin = A3;
const int knobThreshPin = A2;

float thresholdHysteresis = 0.05;

bool autoEnabled = false;

void setup() {
  leds.begin();  //start the Neopixel (WS2812Serial) library
  int ledColor = rgbColor(0, 0, 0);
  leds.setPixel(0, ledColor);
  leds.setPixel(1, ledColor);
  leds.setPixel(2, ledColor);
  leds.show();  //update the pixel color
  AudioMemory(20);
  sgtl5000.enable();
  sgtl5000.inputSelect(1);
  sgtl5000.micGain(30);
  sgtl5000.volume(0.9);
  freezeEffect.setMixMode(MIXMODE_INSERT);  //MIXMODE_INSERT or MIXMODE_WETDRY - wet/dry lets fully clockwise have only FX, no dry signal

  pinMode(FREEZE_BUTTON_PIN, INPUT_PULLUP);
  pinMode(AUTO_BUTTON_PIN, INPUT_PULLUP);
  freezeButton.update();
  autoButton.update();
}

void loop() {
  freezeButton.update();
  autoButton.update();

  if (freezeEffect.ledUpdateFlag) {
    leds.setPixelColor(2, freezeEffect.ledColor);
    leds.show();
    freezeEffect.ledUpdateFlag = false;
  }

  if (freezeButton.fallingEdge()) {
    freezeEffect.toggleFreeze();
    if (freezeEffect.frozen) {
      //use a custom function to combine the rgb values into the right format for the pixel library
      int ledColor = rgbColor(10, 10, 10);
      leds.setPixel(0, ledColor);
      leds.show();  //update the pixel colors
    } else {
      int ledColor = rgbColor(0, 0, 0);
      leds.setPixel(0, ledColor);
      leds.show();  //update the pixel color
    }
  }

  if (autoButton.fallingEdge()) {
    autoEnabled = !autoEnabled;
    freezeEffect.setAutoMode(autoEnabled);

    if (autoEnabled) {  //use a custom function to combine the rgb values into the right format for the pixel library
      int ledColor = rgbColor(10, 10, 0);
      leds.setPixel(1, ledColor);
      leds.show();  //update the pixel colors
    } else {
      int ledColor = rgbColor(0, 0, 0);
      leds.setPixel(1, ledColor);
      leds.show();  //update the pixel color
    }
  }


  int grainVal = analogRead(knobGrainPin);
  int grainSamples = map(grainVal, 10, 1023, 0, 16384);
  freezeEffect.setGrainSize(grainSamples);
  freezeEffect.setGrainStartFraction(grainVal / 1023);

  //float mixVal = analogRead(knobMixPin) / 1023.0f;
  freezeEffect.setMix(1);

  float threshVal = analogRead(knobThreshPin) / 4096.0f;  // dividing by 1023.0f gives a val between 0 - 1. divide by 4096.0f for 0 - 0.25 (a better range for our case)
  freezeEffect.setTriggerThresholds(threshVal, threshVal - thresholdHysteresis);
}

//This is a custom function to allow the user to input RGB values in a range of 0 - 255
//It will convert the result into a number that is formatted appropriately for the WS2812Serial library
int rgbColor(byte r, byte g, byte b) {
  int grbVal = ((g << 16) + (r << 8) + b);  //the input values are shuffled, bit shifted, and added together
  return grbVal;
}



/*
void loop() {
  freezeButton.update();
  autoButton.update();

  if (freezeButton.fallingEdge()) {
    freezeEffect.toggleFreeze();
  }

  if (autoButton.fallingEdge()) {
    autoMode = !autoMode;
    freezeEffect.setAutoRetrigger(autoMode);
  }

  int grainVal = analogRead(knobGrainPin);
  int grainSamples = map(grainVal, 0, 1023, 128, 8192);
  freezeEffect.setGrainSize(grainSamples);

  int envKnob = analogRead(knobDecayPin);
  int envSamples = map(envKnob, 0, 1023, 4, 1024);
  freezeEffect.setEnvelopeSize(envSamples);

  int decayMs = map(envKnob, 0, 1023, 10, 3000);
  freezeEffect.setDecayTimeMs(decayMs);

  float mixVal = analogRead(knobMixPin) / 1023.0f;
  freezeEffect.setMix(mixVal);
}
*/