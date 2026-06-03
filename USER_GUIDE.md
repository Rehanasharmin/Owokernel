# Nova Kernel: User & Developer Guide

Welcome to Nova, a minimalist, educational x86_64 kernel.

## 1. Getting Started

### Booting the Kernel
1. Build the project using `make`.
2. Create an ISO using the Limine bootloader (see Phase 1 for instructions).
3. Run the ISO in QEMU:
   `qemu-system-x86_64 -cdrom nova.iso`

### Using the Shell
Once the kernel boots, you will enter the **Nova Shell**.
- The prompt is `Nova# `.
- Type a command and press **Enter**.

### Available Commands
- `help`: Displays this list of commands.
- `clear`: Clears the terminal screen.
- `write <filename> <text>`: Creates a file in the RAMFS and writes text to it.
- `read <filename>`: Reads the content of a file from RAMFS and prints it.
- `send <text>`: Sends a raw network packet via the loopback interface.
- `exit`: Shuts down the system.

---

## 2. Technical Architecture

### Memory Layout
- **Physical Memory:** Managed via a Bitmap Allocator (PMM).
- **Virtual Memory:** 4-level paging. The kernel resides in the "Higher Half" (`0xFFFF800000000000`), and user processes reside in the "Lower Half".
- **Kernel Heap:** A free-list allocator providing `kmalloc` and `kfree`.

### The System Call Interface
User processes communicate with the kernel via the `syscall` instruction.
| Syscall | Number | Description |
| :--- | :--- | :--- |
| `sys_print` | 1 | Prints a string to the VGA console. |
| `sys_exit` | 2 | Terminates the current process. |
| `sys_open` | 3 | Opens/Creates a file in RAMFS. |
| `sys_read` | 4 | Reads data from an open file. |
| `sys_write` | 5 | Writes data to an open file. |
| `sys_close` | 6 | Closes a file handle. |
| `sys_net_send` | 7 | Sends a packet through a network device. |
| `sys_net_recv` | 8 | Receives a packet from a network device. |
| `sys_read_kbd` | 9 | Reads a single character from the keyboard. |

### Multitasking
The kernel uses a **Preemptive Round Robin Scheduler**. Every timer tick, the kernel saves the current thread's registers and swaps to the next available thread.

---

## 3. Extending the Kernel

### Adding a New Syscall
1. Define a new number in `include/syscall.h`.
2. Implement the handler in `src/kernel/syscall.c`.
3. Add a wrapper in `src/kernel/syscall_wrappers.S`.

### Adding a Driver
1. Create a new `Driver` struct in `src/kernel/drivers/`.
2. Implement the `init`, `read`, and `write` functions.
3. Register the driver in `kernel_main` using `driver_register()`.

### Porting to New Hardware
To port Nova to a different architecture (e.g., ARM64), you would need to replace the `gdt.c`, `idt.c`, `vmm.c` (paging), and the assembly in `context.c` and `syscall_entry.c`.
