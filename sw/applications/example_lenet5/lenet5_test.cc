/* Copyright 2023 The TensorFlow Authors. All Rights Reserved.

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
#pragma message "hello_world_test.cc"
extern "C"
{
#include "lenet5_test.h"
#include <math.h>
#include <stdio.h>
#include "core_v_mini_mcu.h"
}

#include "models/lenet5_input.h"
#include "models/lenet5.h"
#include "tensorflow/lite/core/c/common.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_profiler.h"
#include "tensorflow/lite/micro/recording_micro_interpreter.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"

namespace
{
  using HelloWorldOpResolver = tflite::MicroMutableOpResolver<7>;

  TfLiteStatus RegisterOps(HelloWorldOpResolver &op_resolver)
  {
    TF_LITE_ENSURE_STATUS(op_resolver.AddFullyConnected());
    TF_LITE_ENSURE_STATUS(op_resolver.AddConv2D());
    TF_LITE_ENSURE_STATUS(op_resolver.AddAveragePool2D());
    TF_LITE_ENSURE_STATUS(op_resolver.AddTanh());
    TF_LITE_ENSURE_STATUS(op_resolver.AddReshape());
    TF_LITE_ENSURE_STATUS(op_resolver.AddSoftmax());
    TF_LITE_ENSURE_STATUS(op_resolver.AddLogistic());
    return kTfLiteOk;
  }
} // namespace

TfLiteStatus ProfileMemoryAndLatency()
{
  printf("pmal\r\n");
  const uint8_t *volatile tflite_rom = (const uint8_t *volatile)TFLITE_ROM_START_ADDRESS;
  tflite::MicroProfiler profiler;
  HelloWorldOpResolver op_resolver;
  TF_LITE_ENSURE_STATUS(RegisterOps(op_resolver));

  // Arena size just a round number. The exact arena usage can be determined
  // using the RecordingMicroInterpreter.
  constexpr int kTensorArenaSize = 0x4000;
  uint8_t tensor_arena[kTensorArenaSize];
  constexpr int kNumResourceVariables = 96;

  tflite::RecordingMicroAllocator *allocator(
      tflite::RecordingMicroAllocator::Create(tensor_arena, kTensorArenaSize));
  tflite::RecordingMicroInterpreter interpreter(
      tflite::GetModel(tflite_rom), op_resolver, allocator,
      tflite::MicroResourceVariables::Create(allocator, kNumResourceVariables),
      &profiler);

  TF_LITE_ENSURE_STATUS(interpreter.AllocateTensors());
  TfLiteTensor *input = interpreter.input(0);
  TFLITE_CHECK_NE(input, nullptr);

  TfLiteTensor *output = interpreter.output(0);
  TFLITE_CHECK_NE(output, nullptr);

  int8_t golden_outputs[10] = {-128, -128, 116, -126, -128, -128, -128, -128, -118, -128};

  TFLITE_CHECK_EQ(interpreter.inputs_size(), 1);
  memcpy(input->data.int8, &lenet_input_data, lenet_input_data_size);
  TF_LITE_ENSURE_STATUS(interpreter.Invoke());

  profiler.LogTicksPerTagCsv();
  interpreter.GetMicroAllocator().PrintAllocations();
  return kTfLiteOk;
}

// TfLiteStatus LoadFloatModelAndPerformInference() {
//   const tflite::Model* model =
//       ::tflite::GetModel(g_hello_world_float_model_data);
//   TFLITE_CHECK_EQ(model->version(), TFLITE_SCHEMA_VERSION);

//   HelloWorldOpResolver op_resolver;
//   TF_LITE_ENSURE_STATUS(RegisterOps(op_resolver));

//   // Arena size just a round number. The exact arena usage can be determined
//   // using the RecordingMicroInterpreter.
//   constexpr int kTensorArenaSize = 3000;
//   uint8_t tensor_arena[kTensorArenaSize];

//   tflite::MicroInterpreter interpreter(model, op_resolver, tensor_arena,
//                                        kTensorArenaSize);
//   TF_LITE_ENSURE_STATUS(interpreter.AllocateTensors());

//   // Check if the predicted output is within a small range of the
//   // expected output
//   float epsilon = 0.05f;
//   constexpr int kNumTestValues = 4;
//   float golden_inputs[kNumTestValues] = {0.f, 1.f, 3.f, 5.f};

//   for (int i = 0; i < kNumTestValues; ++i) {
//     interpreter.input(0)->data.f[0] = golden_inputs[i];
//     TF_LITE_ENSURE_STATUS(interpreter.Invoke());
//     float y_pred = interpreter.output(0)->data.f[0];
//     TFLITE_CHECK_LE(abs(sin(golden_inputs[i]) - y_pred), epsilon);
//   }

//   return kTfLiteOk;
// }

TfLiteStatus LoadQuantModelAndPerformInference()
{
  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.

  printf("%p\r\n", tflite_rom);

  for (int i = 0; i < 16; i++)
  {
    printf("%02X ", tflite_rom[i]);
  }
  const tflite::Model *model =
      ::tflite::GetModel(tflite_rom);
  TFLITE_CHECK_EQ(model->version(), TFLITE_SCHEMA_VERSION);
  HelloWorldOpResolver op_resolver;
  TF_LITE_ENSURE_STATUS(RegisterOps(op_resolver));
  // Arena size just a round number. The exact arena usage can be determined
  // using the RecordingMicroInterpreter.
  constexpr int kTensorArenaSize = 0x4000;
  uint8_t tensor_arena[kTensorArenaSize];

  tflite::MicroInterpreter interpreter(model, op_resolver, tensor_arena,
                                       kTensorArenaSize);

  TF_LITE_ENSURE_STATUS(interpreter.AllocateTensors());
  printf("alloc\r\n");
  TfLiteTensor *input = interpreter.input(0);
  TFLITE_CHECK_NE(input, nullptr);

  TfLiteTensor *output = interpreter.output(0);
  TFLITE_CHECK_NE(output, nullptr);

  // float output_scale = output->params.scale;
  // int output_zero_point = output->params.zero_point;
  // float output_scale = output->params.scale;
  // Check if the predicted output is within a small range of the
  // expected output
  // float epsilon = 0.05;

  // constexpr int kNumTestValues = 4;
  // float golden_inputs_float[kNumTestValues] = {0.77, 1.57, 2.3, 3.14};
  int8_t golden_outputs[10] = {-128, -128, 116, -126, -128, -128, -128, -128, -118, -128};
  // The int8 values are calculated using the following formula
  // (golden_inputs_float[i] / input->params.scale + input->params.scale)
  // int8_t golden_inputs_int8[kNumTestValues] = {-96, -63, -34, 0};

  // for (int i = 0; i < kNumTestValues; ++i) {
  memcpy(input->data.int8, &lenet_input_data, lenet_input_data_size);
  printf("inv\r\n");
  // input->data.int8[0] = golden_inputs_int8[i];
  TF_LITE_ENSURE_STATUS(interpreter.Invoke());
  for (int j = 0; j < 10; j++)
  {
    int val = output->data.int8[j];
    printf("%d (%d)\r\n", val, golden_outputs[j]);
  }
  // MicroPrintf("");
  // float y_pred = (output->data.int8[0]  - output_zero_point) * output_scale;
  // printf("%.2f %.2f\r\n", y_pred, golden_inputs_float[i]);
  // TFLITE_CHECK_LE(abs(sin(golden_inputs_float[i]) - y_pred), epsilon);

  return kTfLiteOk;
}

int tfl()
{
  tflite::InitializeTarget();
  // TF_LITE_ENSURE_STATUS(ProfileMemoryAndLatency());
  //  TF_LITE_ENSURE_STATUS(LoadFloatModelAndPerformInference());
  TF_LITE_ENSURE_STATUS(LoadQuantModelAndPerformInference());
  printf("Test passed\r\n");
  return kTfLiteOk;
}

extern "C" int RunLenet5()
{
  return tfl();
}