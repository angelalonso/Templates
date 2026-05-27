#!/bin/bash
set -e

print_status() {
    echo "[INFO] $1"
}

print_status "Building with lint (no tests)..."
print_status "==================================="

print_status "Formatting code..."
find src -name "*.c" -o -name "*.h" | xargs clang-format -i

print_status "Running linter..."
for file in $(find src -name "*.c" -o -name "*.h"); do
    clang-tidy $file --config-file=.clang-tidy
done

mkdir -p build_lint
cd build_lint
cmake .. -DBUILD_TESTS=OFF -DBUILD_GUI=OFF
make -j$(nproc)

if [ -f "./config_cli" ]; then
    cp ./config_cli /app/config_app
    chmod +x /app/config_app
fi

print_status "Lint build complete"
