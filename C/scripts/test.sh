#!/bin/bash
set -e

echo "🧪 Running tests..."

# Build with tests enabled
mkdir -p build_test
cd build_test
cmake .. -DBUILD_TESTS=ON -DENABLE_COVERAGE=ON
make -j$(nproc)

# Run tests
ctest --output-on-failure

# Generate coverage report if gcov is available
if command -v gcovr &> /dev/null; then
    echo "📊 Generating coverage report..."
    gcovr --root .. --html --output coverage.html
    echo "Coverage report: coverage.html"
fi

echo "✅ All tests passed!"
