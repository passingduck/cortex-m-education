# Simple Cortex-M33 Hello World for MPS2-AN505
# Target: ARM Cortex-M33 on QEMU MPS2-AN505 board

# 프로젝트 설정
PROJECT_NAME = cortex-m33-hello-world
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

# 링커 플래그
LDFLAGS = -T linker/cortex-m33.ld -Wl,--gc-sections

# 소스 파일
SOURCES = src/main.c
ASM_SOURCES = src/boot.s

# 오브젝트 파일
OBJECTS = $(SOURCES:src/%.c=$(BUILD_DIR)/%.o)
ASM_OBJECTS = $(ASM_SOURCES:src/%.s=$(BUILD_DIR)/%.o)
ALL_OBJECTS = $(ASM_OBJECTS) $(OBJECTS)

# 출력 파일
ELF_FILE = $(BUILD_DIR)/$(PROJECT_NAME).elf
BIN_FILE = $(BUILD_DIR)/$(PROJECT_NAME).bin
HEX_FILE = $(BUILD_DIR)/$(PROJECT_NAME).hex
MAP_FILE = $(BUILD_DIR)/$(PROJECT_NAME).map

# 기본 타겟
.PHONY: all clean run debug help info

all: $(BIN_FILE) $(HEX_FILE) info

# 빌드 디렉토리 생성
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# C 소스 컴파일
$(BUILD_DIR)/%.o: src/%.c | $(BUILD_DIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# 어셈블리 소스 컴파일
$(BUILD_DIR)/%.o: src/%.s | $(BUILD_DIR)
	@echo "Assembling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# ELF 파일 링킹
$(ELF_FILE): $(ALL_OBJECTS) linker/cortex-m33.ld
	@echo "Linking $(ELF_FILE)..."
	$(LD) $(ALL_OBJECTS) -T linker/cortex-m33.ld -o $@ -Map=$(MAP_FILE)

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
	@echo "=== Build Information ==="
	@echo "Target: $(TARGET)"
	@echo "Project: $(PROJECT_NAME)"
	@echo "Board: MPS2-AN505"
	@echo ""
	$(SIZE) $(ELF_FILE)
	@echo ""
	$(READELF) -A $(ELF_FILE) | grep -E "(Tag_CPU|Tag_THUMB)"

# QEMU에서 실행
run: $(ELF_FILE)
	@echo "Running $(PROJECT_NAME) on QEMU MPS2-AN505..."
	@echo "Press Ctrl+A then X to exit QEMU"
	@echo ""
	qemu-system-arm -machine mps2-an505 -cpu cortex-m33 \
		-kernel $(ELF_FILE) \
		-nographic -semihosting-config enable=on,target=native

# GDB 디버깅
debug: $(ELF_FILE)
	@echo "Starting GDB debugging session..."
	@echo "QEMU will be started in background with GDB server on port 1234"
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
		-ex "b main" \
		-ex "continue"

# 디스어셈블리 생성
disasm: $(ELF_FILE)
	@echo "Generating disassembly..."
	$(OBJDUMP) -d $(ELF_FILE) > $(BUILD_DIR)/$(PROJECT_NAME).asm
	@echo "Disassembly saved to $(BUILD_DIR)/$(PROJECT_NAME).asm"

# 정리
clean:
	@echo "Cleaning build files..."
	rm -rf $(BUILD_DIR)
	@pkill -f "qemu-system-arm.*mps2-an505" || true

# 도움말
help:
	@echo "Cortex-M33 Hello World Build System"
	@echo "Target: ARM Cortex-M33 on QEMU MPS2-AN505"
	@echo ""
	@echo "Usage:"
	@echo "  make          - Build project"
	@echo "  make run      - Run in QEMU"
	@echo "  make debug    - Start GDB debugging"
	@echo "  make disasm   - Generate disassembly"
	@echo "  make info     - Show build information"
	@echo "  make clean    - Clean build files"
	@echo "  make help     - Show this help"
	@echo ""
	@echo "Quick start:"
	@echo "  make && make run"