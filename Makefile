CC = gcc
LD = ld
AS = gcc

CFLAGS = -m64 -ffreestanding -O2 -Wall -Wextra \
         -fno-stack-protector -fno-pic -mno-red-zone \
         -fno-asynchronous-unwind-tables -Iinclude

ASFLAGS = -m64 -ffreestanding -c
LDFLAGS = -m elf_x86_64 -T linker.ld -nostdlib

TARGET = build/kernel.bin

OBJS = \
	src/kernel/kernel.o \
	src/kernel/gdt.o \
	src/kernel/idt.o \
	src/kernel/logging.o \
	src/kernel/string.o \
	src/kernel/pmm.o \
	src/kernel/vmm.o \
	src/kernel/heap.o \
	src/kernel/pic.o \
	src/kernel/interrupts.o \
	src/kernel/thread.o \
	src/kernel/scheduler.o \
	src/kernel/context.o \
	src/kernel/process.o \
	src/kernel/syscall.o \
	src/kernel/syscall_entry.o \
	src/kernel/vfs.o \
	src/kernel/ramfs.o \
	src/kernel/syscall_wrappers.o \
	src/kernel/drivers/manager.o \
	src/kernel/drivers/keyboard.o \
	src/kernel/drivers/timer.o \
	src/kernel/drivers/disk.o \
	src/kernel/elf.o \
	src/kernel/net/core.o \
	src/kernel/net/loopback.o \
	src/kernel/shell.o

all: $(TARGET)

$(TARGET): $(OBJS)
	mkdir -p build
	$(LD) $(LDFLAGS) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -rf build
	find src -name '*.o' -delete

.PHONY: all clean
