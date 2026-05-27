#!/bin/bash
set -e

print_status() {
    echo "[INFO] $1"
}

print_status "Cross-compiling (no tests)..."
print_status "==================================="

mkdir -p build_cross

# Linux x86_64
print_status "Building for Linux x86_64..."
gcc src/*.c -o build_cross/config_app_linux -O2 -D_GNU_SOURCE
cp build_cross/config_app_linux /app/config_app
chmod +x /app/config_app

# Linux i686
if gcc -m32 --version &>/dev/null; then
    print_status "Building for Linux i686..."
    gcc -m32 src/*.c -o build_cross/config_app_linux_i686 -O2 -D_GNU_SOURCE
fi

# Windows x86_64
print_status "Building for Windows x86_64..."
x86_64-w64-mingw32-gcc -static src/*.c -o build_cross/config_app_windows.exe -O2
cp build_cross/config_app_windows.exe /app/config_app.exe

print_status "Cross-compilation complete"
print_status "Binaries: /app/config_app (Linux), /app/config_app.exe (Windows)"
