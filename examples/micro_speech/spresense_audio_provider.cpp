/* Copyright 2018 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

/* Copyright 2018 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#if defined(ARDUINO) && !defined(ARDUINO_ARCH_SPRESENSE)
#define ARDUINO_EXCLUDE_CODE
#endif  // defined(ARDUINO) && !defined(ARDUINO_ARCH_SPRESENSE)

#ifndef ARDUINO_EXCLUDE_CODE

#include "audio_provider.h"

#include <Audio.h>
AudioClass *theAudio;

#include "micro_features_micro_model_settings.h"
#define DEFAULT_PDM_BUFFER_SIZE 512
namespace {
bool g_is_audio_initialized = false;
// An internal buffer able to fit 16x our sample size
//constexpr int kAudioCaptureBufferSize = kAudioSampleFrequency * 0.5;
constexpr int kAudioCaptureBufferSize = DEFAULT_PDM_BUFFER_SIZE * 40;
int16_t g_audio_capture_buffer[kAudioCaptureBufferSize];
// A buffer that holds our output
int16_t g_audio_output_buffer[kMaxAudioSampleSize];
// Mark as volatile so we can check in a while loop to see if
// any samples have arrived yet.
volatile int32_t g_latest_audio_timestamp = 0;
}  // namespace

void CaptureSamples() {
  static int capture_index = 0;
  int number_of_samples;
  uint32_t read_size;

  err_t err;
  uint32_t remain_size;
  while (1) {
    remain_size = (kAudioCaptureBufferSize - capture_index) * sizeof(int16_t);
    err = theAudio->readFrames((char*)&g_audio_capture_buffer[capture_index], remain_size, &read_size);
    if (((err == AUDIOLIB_ECODE_OK) || (err == AUDIOLIB_ECODE_INSUFFICIENT_BUFFER_AREA)) && (read_size > 0)) {
      number_of_samples = read_size / sizeof(int16_t);
      // Determine the index of next sample in our ring buffer
      capture_index += number_of_samples;
      capture_index %= kAudioCaptureBufferSize;
      // Calculate what timestamp the last audio sample represents
      g_latest_audio_timestamp += (number_of_samples / (kAudioSampleFrequency / 1000));
    } else {
      break;
    }
  }
}

TfLiteStatus InitAudioRecording(tflite::ErrorReporter* error_reporter) {
  theAudio = AudioClass::getInstance();
  theAudio->begin();
  /* Select input device as microphone */
  theAudio->setRecorderMode(AS_SETRECDR_STS_INPUTDEVICE_MIC, 100);
  // Start listening for audio: MONO @ 16KHz
  theAudio->initRecorder(AS_CODECTYPE_PCM, "/mnt/sd0/BIN", AS_SAMPLINGRATE_16000, AS_CHANNEL_MONO);
  theAudio->startRecorder();
  return kTfLiteOk;
}

TfLiteStatus GetAudioSamples(tflite::ErrorReporter* error_reporter,
                             int start_ms, int duration_ms,
                             int* audio_samples_size, int16_t** audio_samples) {
  // Set everything up to start receiving audio
  if (!g_is_audio_initialized) {
    TfLiteStatus init_status = InitAudioRecording(error_reporter);
    if (init_status != kTfLiteOk) {
      return init_status;
    }
    g_is_audio_initialized = true;
  }
  // TBD: sync
  CaptureSamples();

  // This next part should only be called when the main thread notices that the
  // latest audio sample data timestamp has changed, so that there's new data
  // in the capture ring buffer. The ring buffer will eventually wrap around and
  // overwrite the data, but the assumption is that the main thread is checking
  // often enough and the buffer is large enough that this call will be made
  // before that happens.

  // Determine the index, in the history of all samples, of the first
  // sample we want
  const int start_offset = start_ms * (kAudioSampleFrequency / 1000);
  // Determine how many samples we want in total
  const int duration_sample_count =
      duration_ms * (kAudioSampleFrequency / 1000);
  for (int i = 0; i < duration_sample_count; ++i) {
    // For each sample, transform its index in the history of all samples into
    // its index in g_audio_capture_buffer
    const int capture_index = (start_offset + i) % kAudioCaptureBufferSize;
    // Write the sample to the output buffer
    g_audio_output_buffer[i] = g_audio_capture_buffer[capture_index];
  }

  // Set pointers to provide access to the audio
  *audio_samples_size = kMaxAudioSampleSize;
  *audio_samples = g_audio_output_buffer;

  return kTfLiteOk;
}

int32_t LatestAudioTimestamp() { return g_latest_audio_timestamp; }

#endif  // ARDUINO_EXCLUDE_CODE
