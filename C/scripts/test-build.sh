#!/bin/bash
set -e

print_status() {
    echo "[INFO] $1"
}

print_status "Building with tests (no lint)..."
print_status "==================================="

mkdir -p build_test
cd build_test

cmake .. -DBUILD_TESTS=ON -DBUILD_GUI=OFF
make -j$(nproc)

print_status "Running tests..."
ctest --output-on-failure

print_status "Running memory checks..."
if [ -f "./test_config" ]; then
    valgrind --leak-check=full --error-exitcode=1 ./test_config
fi

cd ..
print_status "Test build complete"
