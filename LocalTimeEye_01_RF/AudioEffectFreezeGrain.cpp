
#include "AudioEffectFreezeGrain.h"
#include <arm_math.h>

static bool lastPeakActive = false;

AudioEffectFreezeGrain::AudioEffectFreezeGrain()
  : AudioStream(1, &inputQueue) {}

void AudioEffectFreezeGrain::setGrainSize(int size) {
  grainSize = constrain(size, 128, bufferLength);
}

void AudioEffectFreezeGrain::setEnvelopeSize(int size) {
  envelopeSize = constrain(size, 4, grainSize);
}

void AudioEffectFreezeGrain::setDecayTimeMs(int ms) {
  decaySamples = (ms * sampleRate) / 1000;
}

void AudioEffectFreezeGrain::setMix(float dryWet) {
  dryWetMix = constrain(dryWet, 0.0f, 1.0f);
}

void AudioEffectFreezeGrain::setMixMode(MixMode mode) {
  mixMode = mode;
}

void AudioEffectFreezeGrain::toggleFreeze() {
  frozen = !frozen;
  if (frozen) {
    memcpy(buffer, tempBuffer, sizeof(buffer));
    updateGrainOffsets();
  }
}

void AudioEffectFreezeGrain::setAutoMode(bool enable) {
  autoMode = enable;
}

void AudioEffectFreezeGrain::setGrainCount(int count) {
  numGrains = constrain(count, 1, MAX_GRAINS);
}

void AudioEffectFreezeGrain::setEnvelopeFallTimeMs(int ms) {
  float tau = (float)ms / 1000.0f;
  envelopeFallCoef = expf(-1.0f / (tau * sampleRate));
}

void AudioEffectFreezeGrain::setTriggerThresholds(float rise, float fall) {
  fall = constrain(fall, 0.001, 1.0);
  triggerHighThreshold = rise;
  triggerLowThreshold = fall;
}

void AudioEffectFreezeGrain::setRetriggerHoldTimeMs(int ms) {
  retriggerHoldSamples = (ms * sampleRate) / 1000;
}

void AudioEffectFreezeGrain::setGrainStartFraction(float fraction) {
  grainStartOffsetFraction = constrain(fraction, 0.0f, 1.0f);
}

void AudioEffectFreezeGrain::updateGrainOffsets() {
  int maxStart = bufferLength - grainSize;
  int startOffset = (int)(grainStartOffsetFraction * maxStart);

  for (int g = 0; g < numGrains; g++) {
    grainCounters[g] = 0;
    grainOffsets[g] = (startOffset + (g * grainSize) / numGrains) % bufferLength;
  }
}

void AudioEffectFreezeGrain::update() {
  audio_block_t *block = receiveReadOnly(0);
  if (!block) return;

  for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
    float sample = (float)block->data[i] / 32768.0f;
    float absSample = fabsf(sample);

    tempBuffer[autoWriteIndex] = sample;
    autoWriteIndex = (autoWriteIndex + 1) % bufferLength;

    if (absSample > envelope) {
      envelope = absSample;
    } else {
      envelope = envelopeFallCoef * envelope + (1.0f - envelopeFallCoef) * absSample;
    }

    if (envelope < triggerLowThreshold) {
      triggerArmed = true;
    }

    bool peakActive = (envelope >= triggerHighThreshold);
    if (peakActive != lastPeakActive) {
      lastPeakActive = peakActive;
      ledColor = peakActive ? 0x160000 : 0x000000;
      ledUpdateFlag = true;
    }

    if (retriggerCooldown > 0) {
      retriggerCooldown--;
    }

    if (frozen && autoMode && triggerArmed && envelope > triggerHighThreshold && retriggerCooldown == 0) {
      int copyStart = (autoWriteIndex + bufferLength - grainSize) % bufferLength;
      for (int j = 0; j < bufferLength; j++) {
        int srcIndex = (copyStart + j) % bufferLength;
        buffer[j] = tempBuffer[srcIndex];
      }

      updateGrainOffsets();
      retriggerCooldown = retriggerHoldSamples;
      triggerArmed = false;
    }
  }

  audio_block_t *out = allocate();
  if (!out) {
    release(block);
    return;
  }

  for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
    float input = (float)block->data[i] / 32768.0f;
    float frozenOut = 0.0f;

    if (frozen) {
      for (int g = 0; g < numGrains; g++) {
        int pos = (grainOffsets[g] + grainCounters[g]) % bufferLength;
        float env = 1.0f;
        if (grainCounters[g] < envelopeSize)
          env = (float)grainCounters[g] / envelopeSize;
        else if (grainCounters[g] > (grainSize - envelopeSize))
          env = (float)(grainSize - grainCounters[g]) / envelopeSize;

        frozenOut += buffer[pos] * env;

        grainCounters[g]++;
        if (grainCounters[g] >= grainSize) {
          grainCounters[g] = 0;
        }
      }
      frozenOut /= numGrains;
    }

    float output = 0.0f;
    if (mixMode == MIXMODE_WETDRY) {
      if (frozen) output = (1.0f - dryWetMix) * input + dryWetMix * frozenOut;
      else output = input * (1.0f - dryWetMix);
    } else if (mixMode == MIXMODE_INSERT) {
      if (frozen) output = (1.0f - dryWetMix) * input + dryWetMix * frozenOut;
      else output = input;
    }

    out->data[i] = (int16_t)(output * 32767.0f);
  }

  transmit(out);
  release(out);
  release(block);
}
