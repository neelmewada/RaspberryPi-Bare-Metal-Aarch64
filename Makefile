TARGET_PLATFORM ?= RPI3

ARMGNU ?= aarch64-linux-gnu

COPS = -DTARGET_$(TARGET_PLATFORM)=1 -O2 -Wall -nostdlib -nostartfiles -ffreestanding -Iinclude -march=armv8-a+crc+simd+fp -mcpu=cortex-a53 -mtune=cortex-a53
ASMOPS = -Iinclude -O2 -march=armv8-a+crc+simd+fp -mcpu=cortex-a53 -mtune=cortex-a53

BUILD_DIR = build
SRC_DIR = src

all : kernel8.img armstub-new.bin

clean :
	rm -rf $(BUILD_DIR) armstub/$(BUILD_DIR) *.img *.bin

$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(COPS) -MMD -c $< -o $@

$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(ASMOPS) -MMD -c $< -o $@

C_FILES = $(wildcard $(SRC_DIR)/*.c)
C_FILES += $(wildcard $(SRC_DIR)/*/*.c)
C_FILES += $(wildcard $(SRC_DIR)/*/*/*.c)
ASM_FILES = $(wildcard $(SRC_DIR)/*.S)
ASM_FILES += $(wildcard $(SRC_DIR)/*/*.S)
OBJ_FILES = $(C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_c.o)
OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%_s.o)

DEP_FILES = $(OBJ_FILES:%.o=%.d)
-include $(DEP_FILES)

$(BUILD_DIR)/font_psf.o: $(SRC_DIR)/font.psf
	mkdir -p $(@D)
	$(ARMGNU)-ld -r -b binary -o $(BUILD_DIR)/font_psf.o $(SRC_DIR)/font.psf

$(BUILD_DIR)/font_sfn.o: $(SRC_DIR)/font.sfn
	mkdir -p $(@D)
	$(ARMGNU)-ld -r -b binary -o $(BUILD_DIR)/font_sfn.o $(SRC_DIR)/font.sfn

kernel8.img: $(BUILD_DIR)/font_psf.o $(BUILD_DIR)/font_sfn.o $(SRC_DIR)/linker.ld $(OBJ_FILES)
	$(ARMGNU)-ld -T $(SRC_DIR)/linker.ld -o $(BUILD_DIR)/kernel8.elf $(BUILD_DIR)/font_psf.o $(BUILD_DIR)/font_sfn.o $(OBJ_FILES)
	$(ARMGNU)-objcopy $(BUILD_DIR)/kernel8.elf -O binary kernel8.img

armstub/build/armstub_s.o: armstub/src/armstub.S
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(COPS) -MMD -c $< -o $@

armstub-new.bin: armstub/build/armstub_s.o
	mkdir -p $(@D)
	$(ARMGNU)-ld --section-start=.text=0 -o armstub/build/armstub.elf armstub/build/armstub_s.o
	$(ARMGNU)-objcopy armstub/build/armstub.elf -O binary armstub-new.bin

