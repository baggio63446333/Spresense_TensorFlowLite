/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

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

#include "image_provider.h"

#if defined(ARDUINO) && !defined(ARDUINO_ARCH_SPRESENSE)
#define ARDUINO_EXCLUDE_CODE
#endif  // defined(ARDUINO) && !defined(ARDUINO_ARCH_SPRESENSE)

#ifndef ARDUINO_EXCLUDE_CODE

#include <Camera.h>
#include <string.h>
#include <stdio.h>

void printError(enum CamErr err)
{
  printf("Error: ");
  switch (err)
    {
      case CAM_ERR_NO_DEVICE:
        printf("No Device");
        break;
      case CAM_ERR_ILLEGAL_DEVERR:
        printf("Illegal device error");
        break;
      case CAM_ERR_ALREADY_INITIALIZED:
        printf("Already initialized");
        break;
      case CAM_ERR_NOT_INITIALIZED:
        printf("Not initialized");
        break;
      case CAM_ERR_NOT_STILL_INITIALIZED:
        printf("Still picture not initialized");
        break;
      case CAM_ERR_CANT_CREATE_THREAD:
        printf("Failed to create thread");
        break;
      case CAM_ERR_INVALID_PARAM:
        printf("Invalid parameter");
        break;
      case CAM_ERR_NO_MEMORY:
        printf("No memory");
        break;
      case CAM_ERR_USR_INUSED:
        printf("Buffer already in use");
        break;
      case CAM_ERR_NOT_PERMITTED:
        printf("Operation not permitted");
        break;
      default:
        break;
    }
}

// Get the camera module ready
TfLiteStatus InitCamera(tflite::ErrorReporter* error_reporter) {
  CamErr err;
  TF_LITE_REPORT_ERROR(error_reporter, "Attempting to start Camera");
  err = theCamera.begin();
  if (err != CAM_ERR_SUCCESS) {
    printError(err);
    return kTfLiteError;
  }
  err = theCamera.setAutoWhiteBalanceMode(CAM_WHITE_BALANCE_DAYLIGHT);
  if (err != CAM_ERR_SUCCESS) {
    printError(err);
    return kTfLiteError;
  }
  err = theCamera.setStillPictureImageFormat(
     CAM_IMGSIZE_QVGA_H,
     CAM_IMGSIZE_QVGA_V,
     CAM_IMAGE_PIX_FMT_YUV422);
  if (err != CAM_ERR_SUCCESS) {
    printError(err);
    return kTfLiteError;
  }
  return kTfLiteOk;
}

// Get an image from the camera module
TfLiteStatus GetImage(tflite::ErrorReporter* error_reporter, int image_width,
                      int image_height, int channels, int8_t* image_data) {
  static bool g_is_camera_initialized = false;
  if (!g_is_camera_initialized) {
    TfLiteStatus init_status = InitCamera(error_reporter);
    if (init_status != kTfLiteOk) {
      TF_LITE_REPORT_ERROR(error_reporter, "InitCamera failed");
      return init_status;
    }
    g_is_camera_initialized = true;
  }
  CamErr err;
  CamImage clip;
  CamImage img = theCamera.takePicture();
  if (img.isAvailable()) {
    // Clip and resize from QVGA to the specificed size
    const int clip_width  = image_width * 2;
    const int clip_height = image_height * 2;
    const int x_s = (CAM_IMGSIZE_QVGA_H - clip_width) / 2;
    const int y_s = (CAM_IMGSIZE_QVGA_V - clip_height) / 2;
    const int x_e = x_s + clip_width - 1;
    const int y_e = y_s + clip_height - 1;
    err = img.clipAndResizeImageByHW(clip, x_s, y_s, x_e, y_e,
              image_width, image_height);
    if (err != CAM_ERR_SUCCESS) {
        printError(err);
        return kTfLiteError;
    }
    // Convert the pixel format from YUV422 to GRAY
    err = clip.convertPixFormat(CAM_IMAGE_PIX_FMT_GRAY);
    if (err != CAM_ERR_SUCCESS) {
        printError(err);
        return kTfLiteError;
    }
    memcpy(image_data, clip.getImgBuff(), clip.getImgSize());
  }
  return kTfLiteOk;
}

#endif  // ARDUINO_EXCLUDE_CODE
