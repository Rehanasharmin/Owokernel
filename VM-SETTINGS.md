# Running Owokernel in a virtual machine

`make` produces the kernel; `make iso` produces the bootable
`build/owokernel.iso`. The ISO contains a Limine BIOS and UEFI boot path, so
it works with both legacy and modern virtual machines. `make iso` downloads a
pinned Limine binary release the first time and stores it under `build/`.
Install `xorriso` and `curl` first. To use an already downloaded Limine tree:

```sh
LIMINE_DIR=/path/to/limine-binary make iso
```

## Recommended common settings

| Setting | Value |
|---|---|
| Guest OS | Other / Other 64-bit (x86_64) |
| Firmware | BIOS/Legacy (UEFI also supported) |
| CPUs | 1 (2 is fine) |
| RAM | 128 MiB minimum; 256 MiB recommended |
| Storage | None required; attach a disk only for future IDE work |
| Network | Disabled or NAT (the current network driver is loopback only) |
| Audio/USB/3D | Disabled or defaults |
| Boot media | CD/DVD, `build/owokernel.iso`, connected at power-on |

Do not select an ARM/aarch64 machine: Owokernel is x86_64 only. If the VM
shows a blank screen, select BIOS instead of UEFI and enable a VGA-compatible
display adapter. The kernel writes to VGA text memory and does not currently
provide a graphical framebuffer console.

## QEMU

```sh
make iso
qemu-system-x86_64 -cdrom build/owokernel.iso -m 128M -serial stdio
# Linux hosts can use hardware acceleration:
make run-kvm
```

QEMU's default `pc` machine is the most compatible choice. `-m 128M` is
important: the early allocator is intentionally configured for 128 MiB.

## VirtualBox

1. New → **Other/Unknown (64-bit)**; RAM 128–256 MB; one CPU.
2. Settings → Storage → Optical Drive → choose `build/owokernel.iso`.
3. System → Boot Order: Optical before Hard Disk. Disable EFI initially.
4. Start the VM. Remove the ISO later to boot another medium.

## Limbo PC Emulator (Android)

Create a new **x86_64** machine and set Architecture to `x86_64`, Machine to
`pc` (or `q35` if `pc` is unavailable), RAM to `128 MB`, CPU to the default
x86_64 CPU, and BIOS to the default SeaBIOS. Set CDROM/ISO to
`owokernel.iso`, leave HDD empty, disable networking, and boot from CDROM.
Use BIOS/Legacy rather than UEFI unless your Limbo build explicitly provides
UEFI firmware. Avoid very old Limbo builds that only emulate 32-bit x86.

## VMware

Create a custom VM: **Other 64-bit**, 128–256 MB RAM, one processor, and attach
the ISO as the CD/DVD device. Use BIOS firmware first; VMware's default VGA
adapter is sufficient. No virtual disk or network adapter is needed.

## Troubleshooting

- `make iso` says `xorriso` is missing: install `xorriso` (and `curl`) or use a
  local Limine build and an ISO-capable host tool.
- “No bootable device”: ensure the ISO is attached as a CD/DVD, not as a hard
  disk, and use BIOS firmware.
- Immediate reset: use one CPU and at least 128 MB RAM; check that the guest is
  x86_64 and that hardware virtualization is not being confused with nested
  virtualization.
- The shell prompt is `Owo#`. Type `help`; keyboard input uses the PS/2 device.
