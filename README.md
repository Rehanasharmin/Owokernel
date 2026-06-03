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
| 12 | ELF Loader | ✅ | ELF parsing and dynamic binary loading. |
| 13 | Networking | ✅ | Modular Networking Stack and Loopback Interface. |
| 14 | Finalization | ⏳ | Planned: User Shell and Documentation. |

## Current State: Phase 13
The kernel now supports a **Modular Networking Stack**. It includes a device-independent interface and a **Loopback Interface (`lo`)** that allows processes to send and receive packets to themselves. We've also added networking system calls to allow user-land applications to perform raw packet I/O.
