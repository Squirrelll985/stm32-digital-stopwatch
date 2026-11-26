# ---------------------------------------------------------------------------
#  digital_stopwatch / STM32F103C8T6
#  Windows 下可使用 mingw32-make，Linux/macOS 使用 make
# ---------------------------------------------------------------------------
PROJECT     := digital_stopwatch
BUILD_DIR   := build
TARGET_ELF  := $(BUILD_DIR)/$(PROJECT).elf
TARGET_HEX  := $(BUILD_DIR)/$(PROJECT).hex
TARGET_BIN  := $(BUILD_DIR)/$(PROJECT).bin

CROSS       ?= arm-none-eabi-
CC          := $(CROSS)gcc
OBJCOPY     := $(CROSS)objcopy
SIZE        := $(CROSS)size
OPENOCD     ?= openocd
CMSIS_ARM   ?= ../vendor/CMSIS_5/CMSIS/Core/Include
CMSIS_ST    ?= ../vendor/cmsis-device-f1

ifeq ($(OS),Windows_NT)
  CREATE_BUILD = if not exist "$(BUILD_DIR)" mkdir "$(BUILD_DIR)"
  CLEAN_BUILD = if exist "$(BUILD_DIR)" rmdir /S /Q "$(BUILD_DIR)"
else
  CREATE_BUILD = mkdir -p "$(BUILD_DIR)"
  CLEAN_BUILD = rm -rf "$(BUILD_DIR)"
endif

SRCS := \
  src/main.c \
  src/board.c \
  src/display.c \
  src/keys.c \
  src/stopwatch.c \
  src/ui.c \
  src/tim.c \
  src/syscalls.c \
  src/system_stm32f1xx.c \
  startup/startup_stm32f103xb.s

OBJS := $(addprefix $(BUILD_DIR)/,$(notdir $(SRCS:.c=.o)))
OBJS := $(OBJS:.s=.o)

CFLAGS := -mcpu=cortex-m3 -mthumb -mfloat-abi=soft \
          -O2 -g3 -Wall -Wextra -Wshadow \
          -ffunction-sections -fdata-sections \
          -DSTM32F103xB \
          -Iinclude -I$(CMSIS_ARM) -I$(CMSIS_ST)/Include
LDFLAGS := -mcpu=cortex-m3 -mthumb -mfloat-abi=soft \
           -Tld/STM32F103C8Tx_FLASH.ld \
           -Wl,--gc-sections \
           -Wl,--no-warn-rwx-segments \
           -Wl,-Map=$(BUILD_DIR)/$(PROJECT).map \
           --specs=nano.specs --specs=nosys.specs

vpath %.c src $(CMSIS_ST)/Source/Templates
vpath %.s $(CMSIS_ST)/Source/Templates/gcc

.PHONY: all clean size flash

all: $(TARGET_ELF) $(TARGET_HEX) $(TARGET_BIN) size

$(BUILD_DIR):
	@$(CREATE_BUILD)

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.s | $(BUILD_DIR)
	$(CC) $(CFLAGS) -x assembler-with-cpp -c $< -o $@

$(TARGET_ELF): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

$(TARGET_HEX): $(TARGET_ELF)
	$(OBJCOPY) -O ihex $< $@

$(TARGET_BIN): $(TARGET_ELF)
	$(OBJCOPY) -O binary $< $@

size: $(TARGET_ELF)
	@$(SIZE) $(TARGET_ELF)

flash: $(TARGET_ELF)
	$(OPENOCD) -f interface/stlink.cfg -f target/stm32f1x.cfg \
	  -c "program $< verify reset exit"

clean:
	@$(CLEAN_BUILD)
