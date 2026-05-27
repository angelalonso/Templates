#!/bin/bash
set -e

print_status() {
    echo "[INFO] $1"
}

print_error() {
    echo "[ERROR] $1"
    exit 1
}

print_warning() {
    echo "[WARN] $1"
}

# Format code
format_code() {
    print_status "Formatting code..."
    find src tests -name "*.c" -o -name "*.h" 2>/dev/null | xargs clang-format -i 2>/dev/null || true
    print_status "Code formatting complete"
}

# Lint code
lint_code() {
    print_status "Running linter..."
    for file in $(find src -name "*.c" -o -name "*.h" 2>/dev/null); do
        clang-tidy $file --config-file=.clang-tidy 2>/dev/null || print_error "Linting failed"
    done
    print_status "Linting passed"
}

# Build and test
build_and_test() {
    print_status "Building and testing..."
    
    mkdir -p build_test
    cd build_test
    
    cmake .. -DBUILD_TESTS=ON -DBUILD_GUI_TESTS=ON -DENABLE_COVERAGE=OFF
    make -j$(nproc)
    
    print_status "Running tests..."
    ctest --output-on-failure || print_error "Tests failed"
    
    print_status "Running Valgrind memory checks..."
    valgrind --leak-check=full --error-exitcode=1 ./test_config || print_error "Valgrind found issues"
    valgrind --leak-check=full --error-exitcode=1 ./test_safety || print_error "Valgrind found issues"
    
    cd ..
    print_status "All tests passed"
}

# Run GUI safety checks
run_gui_safety_checks() {
    print_status "Running GUI safety checks..."
    
    # Check if GUI can initialize without crashing
    print_status "Testing GUI initialization..."
    timeout 3 ./config_gui 2>&1 | grep -q "GLib-GObject-CRITICAL" && \
        print_error "GUI initialization has critical errors"
    
    # Run GUI with valgrind
    if command -v valgrind &> /dev/null; then
        print_status "Running GUI with Valgrind..."
        timeout 5 valgrind --leak-check=full --track-origins=yes ./config_gui 2>&1 | \
            grep -q "Invalid read" && print_error "GUI has memory errors"
    fi
    
    print_status "GUI safety checks passed"
}

# Safety checks with sanitizers
run_safety_checks() {
    print_status "Running AddressSanitizer..."
    mkdir -p build_asan
    cd build_asan
    cmake .. -DCMAKE_C_FLAGS="-fsanitize=address -g -O0" -DBUILD_TESTS=ON
    make -j$(nproc)
    ASAN_OPTIONS=detect_leaks=1 ./test_config || print_error "AddressSanitizer found issues"
    cd ..
    
    print_status "Running UndefinedBehaviorSanitizer..."
    mkdir -p build_ubsan
    cd build_ubsan
    cmake .. -DCMAKE_C_FLAGS="-fsanitize=undefined -g -O0" -DBUILD_TESTS=ON
    make -j$(nproc)
    ./test_config || print_error "UBSan found issues"
    cd ..
    
    print_status "Safety checks passed"
}

# Build GUI
build_gui() {
    print_status "Building GUI application..."
    mkdir -p build
    cd build
    cmake .. -DBUILD_TESTS=OFF
    make -j$(nproc)
    cp config_gui /app/config_gui 2>/dev/null || cp config_gui ../config_gui
    print_status "GUI build complete"
}

# Main
main() {
    print_status "Starting CI pipeline..."
    print_status "================================"
    
    format_code
    lint_code
    build_and_test
    run_safety_checks
    build_gui
    run_gui_safety_checks
    
    print_status "================================"
    print_status "CI pipeline completed successfully"
    print_status "Executable: ./config_gui"
}

main "$@"
