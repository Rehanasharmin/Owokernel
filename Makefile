CC = gcc
LD = ld

# Compiler flags
CFLAGS = -m64 -ffreestanding -O2 -Wall -Wextra \
         -fno-stack-protector -fno-pic -mno-red-zone \
         -Iinclude

# Linker flags
LDFLAGS = -m elf_x86_64 -T linker.ld

# Target
TARGET = build/kernel.bin
OBJS = src/kernel/kernel.o src/kernel/gdt.o src/kernel/idt.o src/kernel/logging.o src/kernel/pmm.o src/kernel/vmm.o src/kernel/heap.o src/kernel/pic.o src/kernel/interrupts.o src/kernel/thread.o src/kernel/scheduler.o src/kernel/context.o src/kernel/process.o src/kernel/syscall.o src/kernel/syscall_entry.o src/kernel/vfs.o src/kernel/ramfs.o src/kernel/syscall_wrappers.o src/kernel/drivers/manager.o src/kernel/drivers/keyboard.o src/kernel/drivers/timer.o src/kernel/drivers/disk.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build/*.bin src/**/*.o
