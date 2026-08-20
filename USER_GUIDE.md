# Owokernel: User & Developer Guide

Welcome to Owokernel, a minimalist, educational x86_64 kernel covering boot through a user shell.

## 1. Getting Started

### Building
Requires `gcc` and `ld` targeting x86_64 ELF.

```
make
```

The linked kernel is written to `build/kernel.bin`.

### Booting
1. Build the kernel and a BIOS + UEFI bootable ISO:

```sh
make
make iso
```

2. Run in QEMU:

```sh
qemu-system-x86_64 -cdrom build/owokernel.iso -m 128M -serial stdio
```

See [VM-SETTINGS.md](VM-SETTINGS.md) for VirtualBox, Limbo PC Emulator, VMware, and QEMU settings.

### Using the Shell
After boot you get the **Owo Shell** prompt `Owo# `.

| Command | Description |
| :--- | :--- |
| `help` | List commands |
| `clear` | Clear the VGA console |
| `ls` | List RAMFS files |
| `write <file> <text>` | Create/overwrite a file |
| `read <file>` | Print a file |
| `send <text>` | Send a packet on `lo` |
| `recv` | Receive a packet from `lo` |
| `mem` | Physical memory usage |
| `ps` | List threads |
| `echo <text>` | Print text |
| `exit` | Halt the CPU |

---

## 2. Technical Architecture

### Phases implemented
1. **Bootstrapping** — Limine config, 64-bit freestanding kernel, VGA text.
2. **Early init** — GDT (kernel + user segments), IDT, CPU exceptions.
3. **Physical memory** — Bitmap page-frame allocator.
4. **Virtual memory** — 4-level paging, identity map, per-process PML4.
5. **Heap** — Free-list `kmalloc` / `kfree`.
6. **Interrupts** — PIC remap, PIT timer, PS/2 keyboard.
7. **Multitasking I** — Round-robin threads and context switch.
8. **Multitasking II** — Process address spaces.
9. **Syscalls** — `syscall` / `sysret` with STAR/LSTAR.
10. **VFS & RAMFS** — File table over in-memory inodes.
11. **Drivers** — Registration framework, keyboard, timer, IDE PIO.
12. **ELF loader** — PT_LOAD mapping into a process PML4.
13. **Networking** — Device table + loopback queue.
14. **Shell** — Interactive command processor.

### Memory layout
- Physical bitmap at `0x100000`.
- Kernel linked at `0x1000000` (`linker.ld`).
- Identity map of the first 32 MiB.
- Kernel heap at `0xFFFF900000000000`.
- User stacks near `0x7FFFFFFFE000`.

### System calls

| Name | Number |
| :--- | :--- |
| `sys_print` | 1 |
| `sys_exit` | 2 |
| `sys_open` | 3 |
| `sys_read` | 4 |
| `sys_write` | 5 |
| `sys_close` | 6 |
| `sys_net_send` | 7 |
| `sys_net_recv` | 8 |
| `sys_read_kbd` | 9 |

User wrappers live in `src/kernel/syscall_wrappers.S`. The interactive shell runs as a **kernel thread** and talks to VFS/net/keyboard directly so it does not depend on `sysret` from CPL0.

---

## 3. Extending the Kernel

### New syscall
1. Add a number in `include/syscall.h`.
2. Handle it in `src/kernel/syscall.c`.
3. Add a wrapper in `src/kernel/syscall_wrappers.S`.

### New driver
Implement a `Driver` and call `driver_register()` from `kernel_main`.
