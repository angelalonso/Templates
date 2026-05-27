#!/bin/bash
set -e

echo "🔨 Building application..."

# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
make -j$(nproc)

echo "✅ Build complete: build/config_app"
