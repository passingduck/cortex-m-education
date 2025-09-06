# Makefile for Cortex-M33 Register & ALU Demo

CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump

CFLAGS = -mcpu=cortex-m33 -mthumb -Wall -g -O0 -ffunction-sections -fdata-sections
LDFLAGS = -mcpu=cortex-m33 -mthumb -nostartfiles -T linker/cortex-m33.ld -Wl,-Map=build/cortex-m33-register-demo.map

TARGET = cortex-m33-register-demo
SRCDIR = src
BUILDDIR = build

SOURCES = $(SRCDIR)/boot.s $(SRCDIR)/main.c
OBJECTS = $(BUILDDIR)/boot.o $(BUILDDIR)/main.o

.PHONY: all clean run debug

all: $(BUILDDIR)/$(TARGET).bin

$(BUILDDIR)/$(TARGET).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

$(BUILDDIR)/$(TARGET).bin: $(BUILDDIR)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

$(BUILDDIR)/$(TARGET).hex: $(BUILDDIR)/$(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

$(BUILDDIR)/boot.o: $(SRCDIR)/boot.s
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILDDIR)/main.o: $(SRCDIR)/main.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

run: $(BUILDDIR)/$(TARGET).elf
	qemu-system-arm -machine mps2-an505 -cpu cortex-m33 -kernel $< -nographic -semihosting

debug: $(BUILDDIR)/$(TARGET).elf
	qemu-system-arm -machine mps2-an505 -cpu cortex-m33 -kernel $< -nographic -semihosting -s -S

clean:
	rm -rf $(BUILDDIR)
