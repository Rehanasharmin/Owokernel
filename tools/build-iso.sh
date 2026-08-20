#!/usr/bin/env bash
set -euo pipefail

# Build a BIOS + UEFI bootable ISO using Limine.  Override LIMINE_DIR to use a
# local checkout (useful for offline builds), or LIMINE_VERSION to pin a release.
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD="$ROOT/build"
ISO="$BUILD/owokernel.iso"
LIMINE_DIR="${LIMINE_DIR:-$BUILD/limine}"
LIMINE_VERSION="${LIMINE_VERSION:-v9.x-binary}"

command -v xorriso >/dev/null || { echo "error: xorriso is required (install it with your package manager)" >&2; exit 1; }

if [[ ! -f "$LIMINE_DIR/limine-bios-cd.bin" ]]; then
    mkdir -p "$BUILD"
    if [[ ! -d "$LIMINE_DIR" ]]; then
        command -v curl >/dev/null || { echo "error: curl is required to download Limine" >&2; exit 1; }
        archive="$BUILD/limine.tar.gz"
        echo "Downloading Limine ($LIMINE_VERSION)..."
        curl -fL --retry 3 "https://github.com/limine-bootloader/limine/releases/download/${LIMINE_VERSION}/limine-${LIMINE_VERSION#v}.tar.gz" -o "$archive"
        tar -xzf "$archive" -C "$BUILD"
        found="$(find "$BUILD" -maxdepth 1 -type d -name 'limine-*binary' -print -quit)"
        [[ -n "$found" ]] || { echo "error: Limine archive layout changed" >&2; exit 1; }
        mv "$found" "$LIMINE_DIR"
    fi
fi

[[ -f "$ROOT/build/kernel.bin" ]] || { echo "error: build/kernel.bin missing; run 'make' first" >&2; exit 1; }
rm -rf "$BUILD/iso-root"
mkdir -p "$BUILD/iso-root/boot/limine" "$BUILD/iso-root/EFI/BOOT"
cp "$ROOT/build/kernel.bin" "$BUILD/iso-root/boot/kernel.bin"
cp "$ROOT/limine.cfg" "$BUILD/iso-root/boot/limine/limine.cfg"
cp "$LIMINE_DIR/limine-bios-cd.bin" "$BUILD/iso-root/boot/limine/"
cp "$LIMINE_DIR/limine-bios.sys" "$BUILD/iso-root/boot/limine/"
cp "$LIMINE_DIR/BOOTX64.EFI" "$BUILD/iso-root/EFI/BOOT/"
cp "$LIMINE_DIR/BOOTIA32.EFI" "$BUILD/iso-root/EFI/BOOT/" 2>/dev/null || true
xorriso -as mkisofs -b boot/limine/limine-bios-cd.bin \
  -no-emul-boot -boot-load-size 4 -boot-info-table \
  --efi-boot EFI/BOOT/BOOTX64.EFI -efi-boot-part --efi-boot-image \
  -o "$ISO" "$BUILD/iso-root" >/dev/null 2>&1
"$LIMINE_DIR/limine" bios-install "$ISO"
echo "Created $ISO"
