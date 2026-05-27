.PHONY: all build test lint clean help setup ci

all: lint test build

# Build GUI application (Linux only)
build:
	@echo "[INFO] Building Linux GUI application..."
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

# Run all tests (logic + GUI)
test:
	@echo "[INFO] Running all tests (logic + GUI)..."
	@mkdir -p build_test
	@cd build_test && cmake .. -DBUILD_TESTS=ON -DBUILD_GUI_TESTS=ON -DENABLE_COVERAGE=OFF
	@cd build_test && make -j$(nproc)
	@cd build_test && ctest --output-on-failure
	@echo "[INFO] Running memory checks with Valgrind..."
	@cd build_test && valgrind --leak-check=full --error-exitcode=1 ./test_config
	@cd build_test && valgrind --leak-check=full --error-exitcode=1 ./test_safety
	@cd build_test && valgrind --leak-check=full --error-exitcode=1 ./test_gui
	@echo "[SUCCESS] All tests passed"

# Full CI in Docker (lint + test + safety)
ci:
	@echo "[INFO] Running full CI pipeline in Docker..."
	@docker build -f docker/Dockerfile.ci -t config-app-ci .
	@docker run --rm -v $(PWD):/app config-app-ci

# Clean everything
clean:
	@echo "[INFO] Cleaning..."
	@rm -rf build build_test config_gui build_test_gui
	@echo "[SUCCESS] Clean complete"

# Install dependencies (Ubuntu/Debian)
setup:
	@echo "[INFO] Installing dependencies for local development..."
	@sudo apt-get update
	@sudo apt-get install -y build-essential cmake libgtk-3-dev clang clang-tidy clang-format valgrind
	@echo "[SUCCESS] Dependencies installed"

# Help
help:
	@echo "Available commands:"
	@echo "  make build      - Build Linux GUI application locally"
	@echo "  make lint       - Run linter and formatter"
	@echo "  make test       - Run all tests (logic + GUI)"
	@echo "  make ci         - Run full CI pipeline in Docker"
	@echo "  make clean      - Clean build artifacts"
	@echo "  make setup      - Install dependencies for local development"
	@echo ""
	@echo "Quick start:"
	@echo "  make setup      # Install dependencies (first time only)"
	@echo "  make build      # Build the application"
	@echo "  ./config_gui    # Run the GUI application"
	@echo "  make test       # Run tests"
	@echo "  make ci         # Full quality checks in Docker"
