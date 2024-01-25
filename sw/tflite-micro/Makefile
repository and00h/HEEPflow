MAKE     = make
.PHONY: test

XHEEP_FOLDER ?= ..

TARGET   	?= sim

COMPILER_PREFIX ?= riscv32-unknown-

# Arch options are any RISC-V ISA string supported by the CPU. Default 'rv32imc'
ARCH     ?= rv32imac

RISCV_EXE_PREFIX           = $(RISCV)/bin/${COMPILER_PREFIX}elf-

CC := $(RISCV_EXE_PREFIX)gcc
CXX := $(RISCV_EXE_PREFIX)g++
AR := $(RISCV_EXE_PREFIX)ar
ARFLAGS := -r

OBJDIR := obj
BINDIR := bin

LIBTFLM := $(BINDIR)/libtflm.a

TFLITE_COMMON_FLAGS := -fno-unwind-tables -fno-exceptions -ffunction-sections -fdata-sections -fmessage-length=0 \
                    -DTF_LITE_STATIC_MEMORY \
                    -DTF_LITE_DISABLE_X86_NEON \
                    -mexplicit-relocs \
                    -DTF_LITE_MCU_DEBUG_LOG \
                    -DTF_LITE_USE_GLOBAL_CMATH_FUNCTIONS \
                    -funsigned-char \
                    -fno-delete-null-pointer-checks \
                    -fomit-frame-pointer

RISCV_FLAGS := -march=$(ARCH) \
  -mabi=ilp32 \
  -mcmodel=medany \
  -static \
  -Os \

CFLAGS := $(TFLITE_COMMON_FLAGS) $(RISCV_FLAGS) -std=c11

CXXFLAGS := $(TFLITE_COMMON_FLAGS) $(RISCV_FLAGS) -std=c++11 \
  -fno-use-cxa-atexit \
  -fpermissive \
  -fno-rtti \
  -fno-exceptions \
  -fno-threadsafe-statics \
  -Wnon-virtual-dtor \
  -DTF_LITE_USE_GLOBAL_MIN \
  -DTF_LITE_USE_GLOBAL_MAX

TFLM_INCLUDES := \
  -I . \
  -I ./third_party/flatbuffers/include \
  -I ./third_party/gemmlowp \
  -I ./third_party/kissfft \
  -I ./third_party/ruy

XHEEP_INCLUDES := \
  -I $(XHEEP_FOLDER)/device/target/$(TARGET) \
  -I $(XHEEP_FOLDER)/device/lib/base \
  -I $(XHEEP_FOLDER)/device/lib/base/freestanding \
  -I $(XHEEP_FOLDER)/device/lib/

RISCV_INCLUDES := \
  -I ${RISCV}/${COMPILER_PREFIX}elf/include \
  -I ${RISCV}/${COMPILER_PREFIX}elf/include/

TFLM_CC_SRCS := $(shell find tensorflow -name "*.cc" -o -name "*.c")
THIRD_PARTY_CC_SRCS := $(shell find third_party -name "*.cc" -o -name "*.c")
SIGNAL_CC_SRCS := $(shell find signal -name "*.cc" -o -name "*.c")

ALL_SRCS := $(TFLM_CC_SRCS) $(THIRD_PARTY_CC_SRCS) $(SIGNAL_CC_SRCS)

OBJS := $(addprefix $(OBJDIR)/, $(patsubst %.c,%.o,$(patsubst %.cc,%.o,$(ALL_SRCS))))

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(TFLM_INCLUDES) $(XHEEP_INCLUDES) $(RISCV_INCLUDES) -c $< -o $@

$(OBJDIR)/%.o: %.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(TFLM_INCLUDES) $(XHEEP_INCLUDES) $(RISCV_INCLUDES) -c $< -o $@

$(LIBTFLM): $(OBJS)
	@mkdir -p $(dir $@)
	$(AR) $(ARFLAGS) $(LIBTFLM) $(OBJS)

clean:
	rm -rf $(OBJDIR) $(BINDIR)

libtflm: $(LIBTFLM)