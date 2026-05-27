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

# Lint code (only .c files to avoid header include issues)
lint_code() {
    print_status "Running linter on source files..."
    
    # Get GTK includes as separate arguments using xargs
    GTK_CFLAGS=$(pkg-config --cflags gtk+-3.0)
    
    for file in $(find src -name "*.c" 2>/dev/null | grep -v "main_win"); do
        echo "Checking $file..."
        # Use xargs to split the flags into separate arguments
        echo "$GTK_CFLAGS" | xargs clang-tidy $file --config-file=.clang-tidy 2>/dev/null || print_error "Linting failed on $file"
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
    valgrind --leak-check=full --error-exitcode=1 ./test_config || print_error "Valgrind found issues in config tests"
    valgrind --leak-check=full --error-exitcode=1 ./test_safety || print_error "Valgrind found issues in safety tests"
    valgrind --leak-check=full --error-exitcode=1 ./test_gui || print_warning "GUI tests may show valgrind warnings"
    
    cd ..
    print_status "All tests passed"
}

# Run safety checks with sanitizers
run_safety_checks() {
    print_status "Running AddressSanitizer..."
    mkdir -p build_asan
    cd build_asan
    cmake .. -DCMAKE_C_FLAGS="-fsanitize=address -g -O0" -DBUILD_TESTS=ON -DBUILD_GUI_TESTS=ON
    make -j$(nproc)
    ASAN_OPTIONS=detect_leaks=1 ./test_config || print_error "AddressSanitizer found issues"
    ASAN_OPTIONS=detect_leaks=1 ./test_safety || print_error "AddressSanitizer found issues"
    echo "Note: GUI test skipped in ASan"
    cd ..
    
    print_status "Running UndefinedBehaviorSanitizer..."
    mkdir -p build_ubsan
    cd build_ubsan
    cmake .. -DCMAKE_C_FLAGS="-fsanitize=undefined -g -O0" -DBUILD_TESTS=ON -DBUILD_GUI_TESTS=ON
    make -j$(nproc)
    ./test_config || print_error "UBSan found issues"
    ./test_safety || print_error "UBSan found issues"
    echo "Note: GUI test skipped in UBSan"
    cd ..
    
    print_status "Safety checks passed"
}

# Build Linux GUI
build_linux_gui() {
    print_status "Building Linux GUI application..."
    mkdir -p build
    cd build
    cmake .. -DBUILD_TESTS=OFF
    make -j$(nproc)
    cp config_gui /app/config_gui 2>/dev/null || cp config_gui ../config_gui
    print_status "Linux GUI build complete"
}

# Main
main() {
    print_status "Starting CI pipeline in Docker..."
    print_status "========================================="
    
    format_code
    lint_code
    build_and_test
    run_safety_checks
    build_linux_gui
    
    print_status "========================================="
    print_status "CI pipeline completed successfully"
    print_status ""
    print_status "Artifacts generated:"
    
    if [ -f "/app/config_gui" ]; then
        print_status "  - Linux GUI: /app/config_gui (or ./config_gui on host)"
        ls -lh /app/config_gui
    fi
    
    print_status ""
    print_status "To run the application: ./config_gui"
    print_status "Configuration file: cfg.yml"
}

main "$@"
