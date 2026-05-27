#!/bin/bash
set -e

print_status() {
    echo "[INFO] $1"
}

print_status "Building for Android..."

mkdir -p build_android
cd build_android

# Build for multiple Android architectures
ARCHITECTURES="armv7a-linux-androideabi23 aarch64-linux-android23 i686-linux-android23 x86_64-linux-android23"

for ARCH in $ARCHITECTURES; do
    print_status "Building for $ARCH..."
    
    mkdir -p build_$ARCH
    cd build_$ARCH
    
    cmake ../.. \
        -DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK_HOME}/build/cmake/android.toolchain.cmake \
        -DANDROID_ABI=${ARCH%%-*} \
        -DANDROID_PLATFORM=android-23 \
        -DBUILD_GUI=OFF \
        -DBUILD_TESTS=OFF
    
    make -j$(nproc)
    
    cd ..
done

print_status "Android builds complete"

# Copy to main directory with architecture suffix
for ARCH in armv7a aarch64 i686 x86_64; do
    if [ -f "build_${ARCH}-linux-android23/config_cli" ]; then
        cp "build_${ARCH}-linux-android23/config_cli" "../config_app_android_${ARCH}"
    fi
done

print_status "Android executables copied to main directory"
