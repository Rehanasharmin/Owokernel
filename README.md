# Owokernel

A bootable educational x86_64 kernel built from scratch.

## Progress Roadmap

| Phase | Focus | Status | Description |
| :--- | :--- | :--- | :--- |
| 1 | Bootstrapping | ✅ | Minimal bootable kernel with VGA output. |
| 2 | Early Initialization | ✅ | GDT and IDT setup, basic exception handling. |
| 3 | Physical Memory | ✅ | Page Frame Allocator using a Bitmap. |
| 4 | Virtual Memory | ✅ | 4-level Paging, PML4 management, Identity Mapping. |
| 5 | Kernel Heap | ✅ | `kmalloc` and `kfree` using a Free List. |
| 6 | Interrupts | ✅ | PIC remapping, Timer and Keyboard IRQs. |
| 7 | Multitasking I | ✅ | Preemptive Round-Robin Scheduling and Context Switching. |
| 8 | Multitasking II | ✅ | Process Isolation, Separate Address Spaces (PML4). |
| 9 | System Calls | ✅ | `syscall` interface for User-to-Kernel communication. |
| 10 | VFS & RAMFS | ✅ | Virtual File System and in-memory RAMFS. |
| 11 | Drivers | ✅ | Device Driver Framework and IDE Disk Driver. |
| 12 | ELF Loader | ⏳ | Planned: Loading executable binaries. |
| 13 | Networking | ⏳ | Planned: Modular network stack. |
| 14 | Finalization | ⏳ | Planned: User Shell and Documentation. |

## Current State: Phase 11
The kernel now has a formal **Driver Framework**. We have moved our hardware logic into dedicated driver modules for the Keyboard, Timer, and Disk. This allows the kernel to be extensible and modular.
