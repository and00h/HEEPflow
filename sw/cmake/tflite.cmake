set(TFLITE_COMMON_FLAGS "-fno-unwind-tables -fno-exceptions -ffunction-sections -fdata-sections -fmessage-length=0 \
                    -DTF_LITE_STATIC_MEMORY \
                    -DTF_LITE_DISABLE_X86_NEON \
                    -mabi=ilp32 \
                    -mcmodel=medany \
                    -mexplicit-relocs \
                    -DTF_LITE_MCU_DEBUG_LOG \
                    -DTF_LITE_USE_GLOBAL_CMATH_FUNCTIONS \
                    -funsigned-char \
                    -fno-delete-null-pointer-checks \
                    -fomit-frame-pointer\
" )
set(TFLITE_CMAKE_C_FLAGS "${TFLITE_COMMON_FLAGS} ")
set(TFLITE_CMAKE_CXX_FLAGS "${TFLITE_COMMON_FLAGS} \
                    -fno-use-cxa-atexit \
                    -fpermissive \
                    -fno-rtti \
                    -fno-threadsafe-statics \
                    -Wnon-virtual-dtor \
                    -DTF_LITE_USE_GLOBAL_MIN \
                    -DTF_LITE_USE_GLOBAL_MAX")
set(TFLITE_INCLUDE_DIRECTORIES "-I ${SOURCE_PATH}tflite-micro/ \
                     -I ${SOURCE_PATH}tflite-micro/tensorflow \
                     -I ${SOURCE_PATH}tflite-micro/third_party/kissfft \
                     -I ${SOURCE_PATH}tflite-micro/third_party/flatbuffers/include \
                     -I ${SOURCE_PATH}tflite-micro/third_party/gemmlowp \
                     -I ${SOURCE_PATH}tflite-micro/third_party/ruy")
message( "TFLITE INCLUDED")