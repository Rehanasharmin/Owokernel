# Owokernel

A bootable educational x86_64 kernel built from scratch.

## Progress Roadmap

| Phase | Focus | Status | Description |
| :--- | :--- | :--- | :--- |
| 1 | Bootstrapping | ✅ | Minimal bootable kernel with VGA output. |
| 2 | Early Initialization | ✅ | GDT (kernel/user) and IDT, exception handling. |
| 3 | Physical Memory | ✅ | Page Frame Allocator using a Bitmap. |
| 4 | Virtual Memory | ✅ | 4-level Paging, PML4 management, Identity Mapping. |
| 5 | Kernel Heap | ✅ | `kmalloc` and `kfree` using a Free List. |
| 6 | Interrupts | ✅ | PIC remapping, Timer and Keyboard IRQs. |
| 7 | Multitasking I | ✅ | Preemptive Round-Robin Scheduling and Context Switching. |
| 8 | Multitasking II | ✅ | Process Isolation, Separate Address Spaces (PML4). |
| 9 | System Calls | ✅ | `syscall` interface for User-to-Kernel communication. |
| 10 | VFS & RAMFS | ✅ | Virtual File System and in-memory RAMFS. |
| 11 | Drivers | ✅ | Device Driver Framework, keyboard, timer, IDE. |
| 12 | ELF Loader | ✅ | ELF parsing and PT_LOAD mapping. |
| 13 | Networking | ✅ | Modular Networking Stack and Loopback Interface. |
| 14 | Finalization | ✅ | User Shell and Documentation. |

## Current State: Phase 14

All planned phases are implemented. Build with `make`; see `USER_GUIDE.md` for architecture notes and shell commands.

```
make
```

## Run it in a VM

```sh
make                 # build build/kernel.bin
make iso             # build/build/owokernel.iso (BIOS + UEFI)
make run             # boot with QEMU
```

VM-specific settings, including VirtualBox, Limbo PC Emulator, VMware and
QEMU, are documented in [VM-SETTINGS.md](VM-SETTINGS.md). Owokernel is an
x86_64 VGA-text kernel; use at least 128 MiB RAM and BIOS/Legacy firmware for
the broadest compatibility.
