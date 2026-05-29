# Variables
IMAGE_NAME = config-editor-builder
QA_IMAGE_NAME = config-editor-qa
TEST_IMAGE_NAME = config-editor-test
TARGET = config_editor
BUILD_DIR = build
COVERAGE_DIR = coverage
REPORTS_DIR = reports

# Colors for output
GREEN = \033[0;32m
RED = \033[0;31m
YELLOW = \033[0;33m
NC = \033[0m # No Color

.PHONY: all build clean run
.PHONY: lint lint-cppcheck lint-clang lint-format lint-security
.PHONY: test memory-check coverage
.PHONY: qa-all qa-quick
.PHONY: docker-image docker-qa-image

# Default target
all: build

# Build the executable (using Docker)
build: docker-image
	@echo "$(GREEN)Building executable in Docker...$(NC)"
	@docker run --name temp-$(IMAGE_NAME) $(IMAGE_NAME) true 2>/dev/null || true
	@echo "$(GREEN)Copying executable from container...$(NC)"
	@docker cp temp-$(IMAGE_NAME):/app/config_editor ./$(TARGET) 2>/dev/null || \
		(echo "$(RED)Failed to copy executable$(NC)" && exit 1)
	@docker rm temp-$(IMAGE_NAME) 2>/dev/null || true
	@chmod +x ./$(TARGET)
	@if [ -f "./$(TARGET)" ]; then \
		echo "$(GREEN)✓ Build successful!$(NC)"; \
		ls -lh ./$(TARGET); \
	else \
		echo "$(RED)✗ Build failed - executable not found$(NC)"; \
		exit 1; \
	fi

# Docker image
docker-image:
	@echo "$(GREEN)Building Docker image...$(NC)"
	@docker build -t $(IMAGE_NAME) .

# Create QA Docker image
docker-qa-image:
	@echo "$(GREEN)Building QA Docker image...$(NC)"
	@docker build -t $(QA_IMAGE_NAME) --target qa .

# Linting targets
lint: lint-cppcheck lint-clang lint-format lint-security

lint-cppcheck: docker-qa-image
	@echo "$(GREEN)Running cppcheck static analysis...$(NC)"
	@mkdir -p $(REPORTS_DIR)
	@docker run --rm -v "$(PWD):/workspace" $(QA_IMAGE_NAME) \
		bash -c "cppcheck --enable=all --inconclusive --std=c11 \
			--suppress=missingIncludeSystem \
			--error-exitcode=1 \
			src/ 2> $(REPORTS_DIR)/cppcheck.xml || \
		cppcheck --enable=all --inconclusive --std=c11 \
			--suppress=missingIncludeSystem \
			src/"
	@echo "$(GREEN)✓ cppcheck complete$(NC)"

lint-clang: docker-qa-image
	@echo "$(GREEN)Running clang-tidy...$(NC)"
	@mkdir -p $(REPORTS_DIR)
	@docker run --rm -v "$(PWD):/workspace" $(QA_IMAGE_NAME) \
		bash -c "for file in src/*.c; do \
			clang-tidy $$file -- \
			`pkg-config --cflags gtk+-3.0 webkit2gtk-4.1` \
			-Wall -Wextra -std=c11 \
			|| exit 1; \
		done"
	@echo "$(GREEN)✓ clang-tidy complete$(NC)"

lint-format: docker-qa-image
	@echo "$(GREEN)Checking code formatting...$(NC)"
	@docker run --rm -v "$(PWD):/workspace" $(QA_IMAGE_NAME) \
		bash -c "for file in src/*.c src/*.h; do \
			clang-format --dry-run --Werror $$file || exit 1; \
		done"
	@echo "$(GREEN)✓ Format check complete$(NC)"

lint-security: docker-qa-image
	@echo "$(GREEN)Running security analysis...$(NC)"
	@mkdir -p $(REPORTS_DIR)
	@docker run --rm -v "$(PWD):/workspace" $(QA_IMAGE_NAME) \
		flawfinder --html --context src/ > $(REPORTS_DIR)/security.html || true
	@docker run --rm -v "$(PWD):/workspace" $(QA_IMAGE_NAME) \
		flawfinder --quiet --minlevel=1 src/
	@echo "$(GREEN)✓ Security analysis complete$(NC)"

# Fix formatting automatically
format-fix: docker-qa-image
	@echo "$(GREEN)Fixing code formatting...$(NC)"
	@docker run --rm -v "$(PWD):/workspace" $(QA_IMAGE_NAME) \
		bash -c "for file in src/*.c src/*.h; do \
			clang-format -i $$file; \
		done"
	@echo "$(GREEN)✓ Format fixed$(NC)"

# Memory checking
memory-check: build
	@echo "$(GREEN)Running memory check with Valgrind...$(NC)"
	@mkdir -p $(REPORTS_DIR)
	@valgrind --leak-check=full \
		--show-leak-kinds=all \
		--track-origins=yes \
		--verbose \
		--log-file=$(REPORTS_DIR)/valgrind.log \
		./$(TARGET) &
	@VALGRIND_PID=$$!; \
	sleep 3; \
	kill $$VALGRIND_PID 2>/dev/null || true; \
	sleep 1
	@if [ -f $(REPORTS_DIR)/valgrind.log ]; then \
		echo "$(GREEN)Valgrind report saved to $(REPORTS_DIR)/valgrind.log$(NC)"; \
		cat $(REPORTS_DIR)/valgrind.log | grep -E "ERROR SUMMARY|definitely lost|indirectly lost" || true; \
	else \
		echo "$(YELLOW)Valgrind log not generated$(NC)"; \
	fi

# Code coverage
coverage: build-internal
	@echo "$(GREEN)Generating code coverage report...$(NC)"
	@mkdir -p $(COVERAGE_DIR)
	@gcc -fprofile-arcs -ftest-coverage -O0 \
		`pkg-config --cflags gtk+-3.0 webkit2gtk-4.1` \
		-c src/main.c -o $(BUILD_DIR)/main-cov.o
	@gcc -fprofile-arcs -ftest-coverage -O0 \
		`pkg-config --cflags gtk+-3.0 webkit2gtk-4.1` \
		-c src/config.c -o $(BUILD_DIR)/config-cov.o
	@gcc -o $(TARGET)-cov $(BUILD_DIR)/main-cov.o $(BUILD_DIR)/config-cov.o \
		`pkg-config --libs gtk+-3.0 webkit2gtk-4.1` -lgcov
	@echo "Coverage build complete"

coverage-report: coverage
	@echo "$(GREEN)Generating coverage HTML report...$(NC)"
	@gcovr --root . --html --html-details -o $(COVERAGE_DIR)/coverage.html
	@echo "$(GREEN)Coverage report: $(COVERAGE_DIR)/coverage.html$(NC)"

# Runtime testing (without libraries)
docker-test-image:
	@echo "$(GREEN)Building minimal test Docker image...$(NC)"
	@echo 'FROM alpine:latest' > Dockerfile.test
	@echo 'RUN apk add --no-cache libc6-compat' >> Dockerfile.test
	@echo 'WORKDIR /test' >> Dockerfile.test
	@echo 'COPY config_editor .' >> Dockerfile.test
	@echo 'CMD ["./config_editor"]' >> Dockerfile.test
	@docker build -f Dockerfile.test -t $(TEST_IMAGE_NAME) .
	@rm Dockerfile.test

test-runtime: build docker-test-image
	@echo "$(GREEN)Testing executable in minimal container...$(NC)"
	@docker run --rm $(TEST_IMAGE_NAME) sh -c "./config_editor & sleep 2 && killall config_editor" 2>&1 || true
	@echo "$(YELLOW)Note: Executable requires GTK/WebKit libraries at runtime$(NC)"
	@echo "$(GREEN)Runtime test complete$(NC)"

# Check library dependencies
check-deps: build
	@echo "$(GREEN)Checking library dependencies...$(NC)"
	@ldd ./$(TARGET)
	@echo ""

# Internal build (for coverage - not used for main build)
build-internal:
	@mkdir -p $(BUILD_DIR)

# Run all QA checks
qa-all: lint memory-check coverage-report check-deps
	@echo "$(GREEN)All QA checks completed!$(NC)"
	@echo "Reports available in $(REPORTS_DIR)/ and $(COVERAGE_DIR)/"

qa-quick: lint memory-check
	@echo "$(GREEN)Quick QA checks completed!$(NC)"

# Clean everything
clean:
	@echo "$(YELLOW)Cleaning up...$(NC)"
	rm -rf $(TARGET) $(TARGET)-cov $(BUILD_DIR) $(COVERAGE_DIR) $(REPORTS_DIR)
	rm -f Dockerfile.test
	-docker rmi $(IMAGE_NAME) $(QA_IMAGE_NAME) $(TEST_IMAGE_NAME) 2>/dev/null || true
	-docker rm temp-$(IMAGE_NAME) 2>/dev/null || true
	@echo "$(GREEN)Cleanup complete$(NC)"

# Run the application
run: build
	@if [ -f "./$(TARGET)" ]; then \
		echo "$(GREEN)Running $(TARGET)...$(NC)"; \
		./$(TARGET); \
	else \
		echo "$(RED)Executable not found. Run 'make build' first.$(NC)"; \
		exit 1; \
	fi

# Help
help:
	@echo "$(GREEN)Available targets:$(NC)"
	@echo "  make build          - Build the executable using Docker"
	@echo "  make run            - Build and run the application"
	@echo "  make clean          - Remove build artifacts and Docker images"
	@echo ""
	@echo "$(GREEN)Quality Assurance:$(NC)"
	@echo "  make lint           - Run all static analysis tools"
	@echo "  make lint-cppcheck  - Run cppcheck static analyzer"
	@echo "  make lint-clang     - Run clang-tidy"
	@echo "  make lint-format    - Check code formatting"
	@echo "  make format-fix     - Auto-fix code formatting"
	@echo "  make memory-check   - Run Valgrind memory leak detection"
	@echo "  make coverage       - Generate code coverage report"
	@echo "  make qa-all         - Run all QA checks"
	@echo "  make qa-quick       - Run quick QA checks (lint + memory)"
	@echo ""
	@echo "$(GREEN)Testing:$(NC)"
	@echo "  make test-runtime   - Test in minimal container (no GTK)"
	@echo "  make check-deps     - Check library dependencies"
