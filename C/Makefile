.PHONY: all build test test-gui lint clean cross help setup safety ci

all: lint test build

# Build GUI application
build:
	@echo "[INFO] Building GUI application..."
	@mkdir -p build
	@cd build && cmake .. -DBUILD_TESTS=OFF
	@cd build && make -j$(nproc)
	@cp build/config_gui ./config_gui
	@chmod +x ./config_gui
	@echo "[SUCCESS] Build complete: ./config_gui"

# Run linting
lint:
	@echo "[INFO] Running linter..."
	@find src tests -name "*.c" -o -name "*.h" | xargs clang-format -i
	@find src -name "*.c" -o -name "*.h" | xargs clang-tidy --config-file=.clang-tidy
	@echo "[SUCCESS] Linting passed"

# Run all tests including GUI
test: test-gui
	@echo "[INFO] Running all tests..."
	@mkdir -p build_test
	@cd build_test && cmake .. -DBUILD_TESTS=ON -DBUILD_GUI_TESTS=ON -DENABLE_COVERAGE=OFF
	@cd build_test && make -j$(nproc)
	@cd build_test && ctest --output-on-failure
	@echo "[INFO] Running memory checks with Valgrind..."
	@cd build_test && valgrind --leak-check=full --error-exitcode=1 ./test_config
	@cd build_test && valgrind --leak-check=full --error-exitcode=1 ./test_safety
	@echo "[SUCCESS] All tests passed"

# Run GUI tests only
test-gui:
	@echo "[INFO] Running GUI tests..."
	@mkdir -p build_test_gui
	@cd build_test_gui && cmake .. -DBUILD_TESTS=ON -DBUILD_GUI_TESTS=ON -DBUILD_GUI=ON
	@cd build_test_gui && make -j$(nproc)
	@cd build_test_gui && ./test_gui
	@echo "[SUCCESS] GUI tests passed"

# Run safety checks
safety:
	@echo "[INFO] Running safety checks..."
	@mkdir -p build_asan
	@cd build_asan && cmake .. -DCMAKE_C_FLAGS="-fsanitize=address -g -O0" -DBUILD_TESTS=ON
	@cd build_asan && make -j$(nproc)
	@cd build_asan && ASAN_OPTIONS=detect_leaks=1 ./test_config
	@cd build_asan && ASAN_OPTIONS=detect_leaks=1 ./test_safety
	@echo "[SUCCESS] Safety checks passed"

# Full CI
ci: lint test safety build
	@echo "[SUCCESS] CI pipeline completed successfully"

# Clean everything
clean:
	@echo "[INFO] Cleaning..."
	@rm -rf build build_test build_test_gui build_asan build_gui_asan config_gui
	@echo "[SUCCESS] Clean complete"

# Install dependencies
setup:
	@echo "[INFO] Installing dependencies..."
	@sudo apt-get update
	@sudo apt-get install -y build-essential cmake libgtk-3-dev clang clang-tidy clang-format valgrind
	@echo "[SUCCESS] Dependencies installed"

# Help
help:
	@echo "Available commands:"
	@echo "  make build      - Build GUI application"
	@echo "  make lint       - Run linter and formatter"
	@echo "  make test       - Run all tests"
	@echo "  make test-gui   - Run GUI-specific tests"
	@echo "  make safety     - Run AddressSanitizer and UBSan"
	@echo "  make ci         - Complete CI pipeline"
	@echo "  make clean      - Clean build artifacts"
	@echo "  make setup      - Install dependencies"
	@echo ""
	@echo "Quick start:"
	@echo "  make setup      # Install dependencies (first time only)"
	@echo "  make ci         # Run full quality checks and build"
	@echo "  ./config_gui    # Run the GUI application"
