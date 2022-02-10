# Spresense_TensorFlowLite Arduino Library

## Overview

This library is based on `Arduino_TensorFlowLite` and ported to the Spresense board.

It allows you to run machine learning models locally on your device.

This library runs TensorFlow machine learning models on microcontrollers, allowing you to build AI/ML applications powered by deep learning and neural networks. With the included examples, you can recognize speech, detect people using a camera, and recognise "magic wand" gestures using an accelerometer.

Please see for more details.
https://www.tensorflow.org/lite/microcontrollers/overview

## Examples

### hello_world

This example is designed to demonstrate the absolute basics of using TensorFlow Lite for Microcontrollers. It includes the full end-to-end workflow of training a model, converting it for use with TensorFlow Lite for Microcontrollers for running inference on a microcontroller.

https://github.com/tensorflow/tflite-micro/tree/main/tensorflow/lite/micro/examples/hello_world

### magic_wand

This is an example of gesture recognition using an accelerometer sensor `LSM9DS1`.

This example shows how you can use TensorFlow Lite to run a 20 kilobyte neural network model to recognize gestures with an accelerometer.

https://github.com/tensorflow/tflite-micro/tree/main/tensorflow/lite/micro/examples/magic_wand

### micro_speech

This is an example of speech recognition using an analog MEMS microphones on the spresense board.

This example shows how to run a 20 kB model that can recognize 2 keywords, "yes" and "no", from speech data. The application listens to its surroundings with a microphone and indicates when it has detected a word by lighting an LED or displaying data on a screen, depending on the capabilities of the device.

https://github.com/tensorflow/tflite-micro/tree/main/tensorflow/lite/micro/examples/micro_speech

### person_detection

This is an example of person detection using a spresense camera board.

This example shows how you can use Tensorflow Lite to run a 250 kilobyte neural network to recognize people in images.

https://github.com/tensorflow/tflite-micro/tree/main/tensorflow/lite/micro/examples/person_detection

