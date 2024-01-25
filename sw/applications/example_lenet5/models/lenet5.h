#ifndef LENET_5_H_
#define LENET_5_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "core_v_mini_mcu.h"

#include <stdint.h>

const uint8_t * volatile tflite_rom = (const uint8_t * volatile) TFLITE_ROM_START_ADDRESS;

#ifdef __cplusplus
}
#endif
#endif