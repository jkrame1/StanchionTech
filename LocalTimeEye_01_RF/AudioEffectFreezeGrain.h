#pragma once
#include <Arduino.h>
#include <Audio.h>

#define MAX_GRAINS 6
#define FREEZE_BUFFER_SIZE 16384  // try 16384 if you want even longer freezes / 8192

enum MixMode {
  MIXMODE_INSERT,
  MIXMODE_WETDRY
};

class AudioEffectFreezeGrain : public AudioStream {
public:
  AudioEffectFreezeGrain();
  virtual void update(void);

  void toggleFreeze();
  void setGrainSize(int size);
  void setEnvelopeSize(int size);
  void setDecayTimeMs(int ms);
  void setMix(float dryWet);
  void setMixMode(MixMode mode);
  void setAutoMode(bool enable);
  void setThreshold(float value);
  void setGrainCount(int count);  // for overlapping grains

  void setEnvelopeFallTimeMs(int ms);  // fall time for envelope follower
  void setTriggerThresholds(float rise, float fall);
  void setRetriggerHoldTimeMs(int ms);

  void setGrainStartFraction(float fraction);
  void updateGrainOffsets();

  uint32_t ledColor = 0;       // NeoPixel color to be set
  bool ledUpdateFlag = false;  // Indicates color change occurred

  bool frozen = false;

private:
  audio_block_t *inputQueue;

  float buffer[FREEZE_BUFFER_SIZE];
  float tempBuffer[FREEZE_BUFFER_SIZE];
  int bufferLength = FREEZE_BUFFER_SIZE;

  int writeIndex = 0;
  int readIndex = 0;
  int grainSize = 512;
  int envelopeSize = 128;  //128
  int decaySamples = 44100;
  int sampleRate = 44100;
  int grainStartOffset = 0;  // offset from the trigger moment
  int bufferWriteIndex = 0;  // current write head in buffer


  float smoothedLevel = 0.0f;
  // Hold timer

float grainStartOffsetFraction = 0.0f;


  float dryWetMix = 1.0f;
  MixMode mixMode = MIXMODE_WETDRY;

  // Auto-retrigger
  bool autoMode = false;
  float threshold = 0.2f;
  int autoWriteIndex = 0;
  bool readyToRefreeze = false;

  // Overlapping grains
  int numGrains = 1;
  int grainCounters[MAX_GRAINS] = { 0 };
  int grainOffsets[MAX_GRAINS] = { 0 };

  // Envelope follower
  float envelope = 0.0f;
  float envelopeFallCoef = 0.99f;  // default fall time coefficient

  // Peak detector with hysteresis
  bool triggerArmed = true;
  float triggerHighThreshold = 0.2f;
  float triggerLowThreshold = 0.1f;
  int retriggerHoldSamples = 200;  // e.g. 11025 = 0.25 seconds at 44.1kHz
  int retriggerCooldown = 0;       // countdown in samples
  bool lastEnvelopeHigh = false;
};
