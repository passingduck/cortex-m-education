# TrustZone Non-secure Transition Example for Cortex-M33
# Target: ARM Cortex-M33 on QEMU MPS2-AN505 board
# Demonstrates Secure to Non-secure state transition

# 프로젝트 설정
PROJECT_NAME = trustzone-nonsecure
TARGET = cortex-m33
BUILD_DIR = build

# 툴체인 설정
CROSS_COMPILE = arm-none-eabi-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
SIZE = $(CROSS_COMPILE)size
READELF = $(CROSS_COMPILE)readelf

# 컴파일 플래그
CFLAGS = -mcpu=cortex-m33 -mthumb -g -O2 -Wall -Wextra \
         -nostdlib -nostartfiles -ffreestanding \
         -fdata-sections -ffunction-sections

# 링커 설정
LINKER_SCRIPT = linker/cortex-m33-tz.ld

# 소스 파일
SECURE_ASM = src/boot_secure.s
NS_ASM = src/ns_vectors.s
NS_C = src/ns_main.c

# 오브젝트 파일
SECURE_OBJ = $(BUILD_DIR)/boot_secure.o
NS_VECTOR_OBJ = $(BUILD_DIR)/ns_vectors.o
NS_MAIN_OBJ = $(BUILD_DIR)/ns_main.o

ALL_OBJECTS = $(SECURE_OBJ) $(NS_VECTOR_OBJ) $(NS_MAIN_OBJ)

# 출력 파일
ELF_FILE = $(BUILD_DIR)/$(PROJECT_NAME).elf
BIN_FILE = $(BUILD_DIR)/$(PROJECT_NAME).bin
HEX_FILE = $(BUILD_DIR)/$(PROJECT_NAME).hex
MAP_FILE = $(BUILD_DIR)/$(PROJECT_NAME).map

# 기본 타겟
.PHONY: all clean run debug help info disasm

all: $(BIN_FILE) $(HEX_FILE) info

# 빌드 디렉토리 생성
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Secure 어셈블리 컴파일
$(SECURE_OBJ): $(SECURE_ASM) | $(BUILD_DIR)
	@echo "Assembling Secure boot code: $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Non-secure 어셈블리 컴파일
$(NS_VECTOR_OBJ): $(NS_ASM) | $(BUILD_DIR)
	@echo "Assembling Non-secure vectors: $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Non-secure C 코드 컴파일
$(NS_MAIN_OBJ): $(NS_C) | $(BUILD_DIR)
	@echo "Compiling Non-secure application: $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# ELF 파일 링킹
$(ELF_FILE): $(ALL_OBJECTS) $(LINKER_SCRIPT)
	@echo "Linking TrustZone image: $(ELF_FILE)..."
	$(LD) $(ALL_OBJECTS) -T $(LINKER_SCRIPT) -o $@ -Map=$(MAP_FILE)

# 바이너리 파일 생성
$(BIN_FILE): $(ELF_FILE)
	@echo "Creating binary file..."
	$(OBJCOPY) -O binary $< $@

# HEX 파일 생성
$(HEX_FILE): $(ELF_FILE)
	@echo "Creating hex file..."
	$(OBJCOPY) -O ihex $< $@

# 빌드 정보 출력
info: $(ELF_FILE)
	@echo ""
	@echo "=== TrustZone Build Information ==="
	@echo "Target: $(TARGET)"
	@echo "Project: $(PROJECT_NAME)"
	@echo "Board: MPS2-AN505"
	@echo "Features: TrustZone Secure -> Non-secure transition"
	@echo ""
	$(SIZE) $(ELF_FILE)
	@echo ""
	$(READELF) -A $(ELF_FILE) | grep -E "(Tag_CPU|Tag_THUMB)"
	@echo ""
	@echo "=== Memory Layout ==="
	@echo "Secure Boot:     0x10000000 (S_CODE_BOOT)"
	@echo "Non-secure Code: 0x00000000 (NS_CODE)"
	@echo "Non-secure RAM:  0x20000000 (NS_RAM)"

# QEMU에서 실행
run: $(ELF_FILE)
	@echo "Running TrustZone example on QEMU MPS2-AN505..."
	@echo "Secure boot -> Non-secure application transition"
	@echo "Press Ctrl+A then X to exit QEMU"
	@echo ""
	qemu-system-arm -machine mps2-an505 -cpu cortex-m33 \
		-kernel $(ELF_FILE) \
		-nographic -semihosting-config enable=on,target=native

# GDB 디버깅
debug: $(ELF_FILE)
	@echo "Starting GDB debugging session for TrustZone..."
	@echo "QEMU will be started in background with GDB server on port 1234"
	@echo "Note: Debugging both Secure and Non-secure states"
	@echo ""
	@pkill -f "qemu-system-arm.*mps2-an505" || true
	@qemu-system-arm -machine mps2-an505 -cpu cortex-m33 \
		-kernel $(ELF_FILE) \
		-nographic -semihosting-config enable=on,target=native \
		-s -S &
	@sleep 2
	@echo "Connect with: $(CROSS_COMPILE)gdb $(ELF_FILE) -ex 'target remote localhost:1234'"
	$(CROSS_COMPILE)gdb $(ELF_FILE) \
		-ex "target remote localhost:1234" \
		-ex "b Reset_Handler" \
		-ex "b Reset_Handler_NS" \
		-ex "continue"

# 디스어셈블리 생성
disasm: $(ELF_FILE)
	@echo "Generating disassembly..."
	$(OBJDUMP) -d $(ELF_FILE) > $(BUILD_DIR)/$(PROJECT_NAME).asm
	@echo "Disassembly saved to $(BUILD_DIR)/$(PROJECT_NAME).asm"
	@echo ""
	@echo "Key sections:"
	@echo "  - Secure boot code (Reset_Handler)"
	@echo "  - Non-secure vectors and main"
	@echo "  - TrustZone transition sequence"

# 메모리 맵 분석
memory-map: $(ELF_FILE)
	@echo "=== Memory Map Analysis ==="
	$(READELF) -l $(ELF_FILE)
	@echo ""
	@echo "=== Section Headers ==="
	$(READELF) -S $(ELF_FILE)

# 정리
clean:
	@echo "Cleaning TrustZone build files..."
	rm -rf $(BUILD_DIR)
	@pkill -f "qemu-system-arm.*mps2-an505" || true

# 도움말
help:
	@echo "TrustZone Non-secure Transition Build System"
	@echo "Target: ARM Cortex-M33 on QEMU MPS2-AN505"
	@echo ""
	@echo "This example demonstrates:"
	@echo "  - Secure boot and SAU configuration"
	@echo "  - Secure to Non-secure state transition"
	@echo "  - Non-secure code execution in NS RAM"
	@echo ""
	@echo "Usage:"
	@echo "  make          - Build TrustZone project"
	@echo "  make run      - Run in QEMU"
	@echo "  make debug    - Start GDB debugging (both S/NS)"
	@echo "  make disasm   - Generate disassembly"
	@echo "  make memory-map - Show memory layout"
	@echo "  make info     - Show build information"
	@echo "  make clean    - Clean build files"
	@echo "  make help     - Show this help"
	@echo ""
	@echo "Quick start:"
	@echo "  make && make run"
	@echo ""
	@echo "Memory Layout:"
	@echo "  Secure Boot:     0x10000000-0x1007FFFF (S_CODE_BOOT)"
	@echo "  Non-secure Code: 0x00000000-0x0007FFFF (NS_CODE)"
	@echo "  Non-secure RAM:  0x20000000-0x2007FFFF (NS_RAM)"
