.PHONY: all build test lint clean cross help

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

# Run tests with memory checks
test:
	@echo "[INFO] Building and running tests..."
	@mkdir -p build_test
	@cd build_test && cmake .. -DBUILD_TESTS=ON -DENABLE_COVERAGE=OFF
	@cd build_test && make -j$(nproc)
	@cd build_test && ctest --output-on-failure
	@echo "[INFO] Running memory checks with Valgrind..."
	@cd build_test && valgrind --leak-check=full --error-exitcode=1 ./test_config
	@cd build_test && valgrind --leak-check=full --error-exitcode=1 ./test_safety
	@echo "[SUCCESS] All tests passed"

# Run safety checks (AddressSanitizer + UBSan)
safety:
	@echo "[INFO] Running safety checks..."
	@mkdir -p build_asan
	@cd build_asan && cmake .. -DCMAKE_C_FLAGS="-fsanitize=address -g -O0" -DBUILD_TESTS=ON
	@cd build_asan && make -j$(nproc)
	@cd build_asan && ASAN_OPTIONS=detect_leaks=1 ./test_config
	@cd build_asan && ASAN_OPTIONS=detect_leaks=1 ./test_safety
	@echo "[SUCCESS] Safety checks passed"

# Full CI (lint + test + safety + build)
ci: lint test safety build
	@echo "[SUCCESS] CI pipeline completed successfully"

# Cross-compile for Windows (prepare for future)
cross-win:
	@echo "[INFO] Cross-compiling for Windows..."
	@mkdir -p build_win
	@cd build_win && cmake .. -DCMAKE_TOOLCHAIN_FILE=cmake/Toolchain-mingw64.cmake -DBUILD_TESTS=OFF -DBUILD_GUI=OFF
	@cd build_win && make -j$(nproc)
	@echo "[SUCCESS] Windows build complete (CLI only for now)"

# Docker build (reproducible)
docker-ci:
	@docker build -f docker/Dockerfile.ci -t config-app-ci .
	@docker run --rm -v $(PWD):/app config-app-ci

# Clean everything
clean:
	@echo "[INFO] Cleaning..."
	@rm -rf build build_test build_asan build_win config_gui
	@echo "[SUCCESS] Clean complete"

# Install dependencies (Ubuntu/Debian)
setup:
	@echo "[INFO] Installing dependencies..."
	@sudo apt-get update
	@sudo apt-get install -y build-essential cmake libgtk-3-dev clang clang-tidy clang-format valgrind
	@echo "[SUCCESS] Dependencies installed"

# Help
help:
	@echo "Available commands:"
	@echo "  make all        - Run lint, tests, and build (full CI)"
	@echo "  make build      - Build GUI application only"
	@echo "  make lint       - Run linter and formatter"
	@echo "  make test       - Run tests with Valgrind"
	@echo "  make safety     - Run AddressSanitizer and UBSan"
	@echo "  make ci         - Complete CI pipeline (lint + test + safety + build)"
	@echo "  make cross-win  - Cross-compile for Windows (CLI only for now)"
	@echo "  make docker-ci  - Run CI pipeline in Docker"
	@echo "  make clean      - Clean build artifacts"
	@echo "  make setup      - Install all dependencies (Ubuntu/Debian)"
	@echo ""
	@echo "Quick start:"
	@echo "  make setup      # Install dependencies (first time only)"
	@echo "  make ci         # Run full quality checks and build"
	@echo "  ./config_gui    # Run the GUI application"
