# Variables
IMAGE_NAME = config-editor-builder
QA_IMAGE_NAME = config-editor-qa
TARGET = config_editor
REPORTS_DIR = reports
COVERAGE_DIR = coverage

# Colors for output
GREEN = \033[0;32m
RED = \033[0;31m
YELLOW = \033[0;33m
NC = \033[0m # No Color

.PHONY: all build clean run
.PHONY: lint lint-cppcheck lint-clang lint-format lint-security
.PHONY: test memory-check coverage
.PHONY: ci-all ci-quick
.PHONY: docker-image docker-qa-image

# Default target
all: build

# Build the executable using Docker
build: docker-image
	@echo "$(GREEN)Creating temporary container...$(NC)"
	@docker create --name temp-$(IMAGE_NAME) $(IMAGE_NAME) > /dev/null 2>&1 || true
	@echo "$(GREEN)Copying executable from container...$(NC)"
	@if docker cp temp-$(IMAGE_NAME):/usr/local/bin/config_editor ./$(TARGET) 2>/dev/null; then \
		echo "$(GREEN)✓ Executable copied successfully$(NC)"; \
		chmod +x ./$(TARGET); \
		ls -lh ./$(TARGET); \
	else \
		echo "$(RED)✗ Failed to copy executable$(NC)"; \
		echo "$(YELLOW)Debug: Checking container contents...$(NC)"; \
		docker run --rm $(IMAGE_NAME) ls -la /usr/local/bin/ 2>/dev/null || echo "No /usr/local/bin directory"; \
		docker rm temp-$(IMAGE_NAME) > /dev/null 2>&1 || true; \
		exit 1; \
	fi
	@docker rm temp-$(IMAGE_NAME) > /dev/null 2>&1 || true
	@echo "$(GREEN)✓ Build successful!$(NC)"

# Build Docker image (runtime stage)
docker-image:
	@echo "$(GREEN)Building Docker image...$(NC)"
	@docker build -t $(IMAGE_NAME) --target runtime .
	@echo "$(GREEN)Docker image built successfully$(NC)"

# Build QA Docker image with all analysis tools
docker-qa-image:
	@echo "$(GREEN)Building QA Docker image...$(NC)"
	@docker build -t $(QA_IMAGE_NAME) --target qa .
	@echo "$(GREEN)QA Docker image built successfully$(NC)"

# ============================================
# LINTING TARGETS
# ============================================

lint: lint-cppcheck lint-clang lint-format lint-security
	@echo "$(GREEN)✓ All linting checks passed$(NC)"

# Static analysis with cppcheck
lint-cppcheck: docker-qa-image
	@echo "$(GREEN)Running cppcheck static analysis...$(NC)"
	@mkdir -p $(REPORTS_DIR)
	@docker run --rm -v "$(PWD):/workspace" $(QA_IMAGE_NAME) \
		bash -c "cd /workspace && \
			cppcheck --enable=all --inconclusive --std=c11 \
			--suppress=missingIncludeSystem \
			--suppress=unmatchedSuppression \
			--error-exitcode=1 \
			--inline-suppr \
			--xml --xml-version=2 \
			-I src -I src/backend -I src/middleware \
			src/ 2> $(REPORTS_DIR)/cppcheck.xml \
			&& echo '✓ No critical issues found' \
			|| (echo '$(RED)✗ Issues found - check $(REPORTS_DIR)/cppcheck.xml$(NC)' && exit 1)"
	@echo "$(GREEN)✓ cppcheck complete$(NC)"

# Clang-tidy for deeper analysis
lint-clang: docker-qa-image
	@echo "$(GREEN)Running clang-tidy...$(NC)"
	@mkdir -p $(REPORTS_DIR)
	@docker run --rm -v "$(PWD):/workspace" $(QA_IMAGE_NAME) \
		bash -c "cd /workspace && \
			for file in src/*.c src/backend/*.c src/middleware/*.c; do \
				if [ -f \"$$file\" ]; then \
					echo \"Checking $$file...\"; \
					clang-tidy $$file -- \
					`pkg-config --cflags gtk+-3.0 webkit2gtk-4.1` \
					-I src -I src/backend -I src/middleware \
					-Wall -Wextra -std=c11 \
					|| exit 1; \
				fi \
			done"
	@echo "$(GREEN)✓ clang-tidy complete$(NC)"

# Code formatting check
lint-format: docker-qa-image
	@echo "$(GREEN)Checking code formatting...$(NC)"
	@docker run --rm -v "$(PWD):/workspace" $(QA_IMAGE_NAME) \
		bash -c "cd /workspace && \
			errors=0; \
			for file in src/*.c src/*.h src/backend/*.c src/backend/*.h src/middleware/*.h; do \
				if [ -f \"$$file\" ]; then \
					clang-format --dry-run --Werror $$file || errors=1; \
				fi \
			done; \
			exit $$errors"
	@echo "$(GREEN)✓ Code formatting is correct$(NC)"

# Security vulnerability scan
lint-security: docker-qa-image
	@echo "$(GREEN)Running security analysis...$(NC)"
	@mkdir -p $(REPORTS_DIR)
	@docker run --rm -v "$(PWD):/workspace" $(QA_IMAGE_NAME) \
		bash -c "cd /workspace && \
			flawfinder --html --context src/ > $(REPORTS_DIR)/security.html && \
			flawfinder --quiet --minlevel=2 src/ && \
			echo '✓ No high-severity issues found'"
	@echo "$(GREEN)✓ Security analysis complete$(NC)"
	@echo "$(YELLOW)Full report: $(REPORTS_DIR)/security.html$(NC)"

# Auto-fix formatting (not for CI, for developers)
format-fix: docker-qa-image
	@echo "$(GREEN)Fixing code formatting...$(NC)"
	@docker run --rm -v "$(PWD):/workspace" $(QA_IMAGE_NAME) \
		bash -c "cd /workspace && \
			for file in src/*.c src/*.h src/backend/*.c src/backend/*.h src/middleware/*.h; do \
				if [ -f \"$$file\" ]; then \
					clang-format -i $$file; \
				fi \
			done"
	@echo "$(GREEN)✓ Format fixed$(NC)"

# ============================================
# TESTING & MEMORY CHECK
# ============================================

# Memory leak detection with Valgrind
memory-check: build
	@echo "$(GREEN)Running memory check with Valgrind...$(NC)"
	@mkdir -p $(REPORTS_DIR)
	@echo "$(YELLOW)Starting application for 5 seconds...$(NC)"
	@timeout 5 valgrind --leak-check=full \
		--show-leak-kinds=all \
		--track-origins=yes \
		--error-exitcode=1 \
		--verbose \
		--log-file=$(REPORTS_DIR)/valgrind.log \
		./$(TARGET) 2>&1 || true
	@if [ -f $(REPORTS_DIR)/valgrind.log ]; then \
		echo "$(GREEN)Valgrind report saved to $(REPORTS_DIR)/valgrind.log$(NC)"; \
		if grep -q "ERROR SUMMARY: 0" $(REPORTS_DIR)/valgrind.log; then \
			echo "$(GREEN)✓ No memory leaks detected$(NC)"; \
		else \
			echo "$(RED)✗ Memory leaks detected!$(NC)"; \
			grep -E "ERROR SUMMARY|definitely lost|indirectly lost" $(REPORTS_DIR)/valgrind.log; \
			exit 1; \
		fi \
	else \
		echo "$(YELLOW)Valgrind log not generated$(NC)"; \
	fi

# Code coverage analysis
coverage: docker-qa-image
	@echo "$(GREEN)Generating code coverage report...$(NC)"
	@mkdir -p $(COVERAGE_DIR)
	@docker run --rm -v "$(PWD):/workspace" $(QA_IMAGE_NAME) \
		bash -c "cd /workspace && \
			# Compile with coverage flags \
			gcc -fprofile-arcs -ftest-coverage -O0 -g \
				`pkg-config --cflags gtk+-3.0 webkit2gtk-4.1` \
				-I src -I src/backend -I src/middleware \
				-c src/main.c -o $(COVERAGE_DIR)/main-cov.o && \
			gcc -fprofile-arcs -ftest-coverage -O0 -g \
				`pkg-config --cflags gtk+-3.0 webkit2gtk-4.1` \
				-I src -I src/backend -I src/middleware \
				-c src/backend/config_manager.c -o $(COVERAGE_DIR)/config_manager-cov.o && \
			gcc -fprofile-arcs -ftest-coverage -O0 -g \
				`pkg-config --cflags gtk+-3.0 webkit2gtk-4.1` \
				-I src -I src/backend -I src/middleware \
				-c src/backend/api_handler.c -o $(COVERAGE_DIR)/api_handler-cov.o && \
			gcc -fprofile-arcs -ftest-coverage -O0 -g \
				`pkg-config --cflags gtk+-3.0 webkit2gtk-4.1` \
				-I src -I src/backend -I src/middleware \
				-c src/backend/webkit_handler.c -o $(COVERAGE_DIR)/webkit_handler-cov.o && \
			# Link with coverage \
			gcc -o $(COVERAGE_DIR)/config_editor-cov \
				$(COVERAGE_DIR)/main-cov.o \
				$(COVERAGE_DIR)/config_manager-cov.o \
				$(COVERAGE_DIR)/api_handler-cov.o \
				$(COVERAGE_DIR)/webkit_handler-cov.o \
				`pkg-config --libs gtk+-3.0 webkit2gtk-4.1` -lgcov -ljson-c && \
			# Run the instrumented binary \
			timeout 3 ./$(COVERAGE_DIR)/config_editor-cov 2>/dev/null || true && \
			# Generate coverage report \
			gcovr --root . --html --html-details -o $(COVERAGE_DIR)/coverage.html \
				--filter='src/.*' \
				--exclude='src/backend/webkit_handler\.c' \
				--exclude-unreachable-branches && \
			# Print summary \
			gcovr --root . --print-summary \
				--filter='src/.*' \
				--exclude='src/backend/webkit_handler\.c'"
	@echo "$(GREEN)✓ Coverage report generated: $(COVERAGE_DIR)/coverage.html$(NC)"

# ============================================
# CI/CD PIPELINE TARGETS
# ============================================

# Full CI pipeline (all checks)
ci-all: lint memory-check coverage
	@echo "$(GREEN)========================================$(NC)"
	@echo "$(GREEN)✓ All CI checks passed!$(NC)"
	@echo "$(GREEN)========================================$(NC)"
	@echo "Reports available in:"
	@echo "  - Linting:     $(REPORTS_DIR)/cppcheck.xml"
	@echo "  - Security:    $(REPORTS_DIR)/security.html"
	@echo "  - Memory:      $(REPORTS_DIR)/valgrind.log"
	@echo "  - Coverage:    $(COVERAGE_DIR)/coverage.html"
	@echo "$(GREEN)========================================$(NC)"

# Quick CI (linting only, faster for development)
ci-quick: lint
	@echo "$(GREEN)✓ Quick CI checks passed!$(NC)"

# ============================================
# CLEANUP
# ============================================

# Clean everything
clean:
	@echo "$(YELLOW)Cleaning up...$(NC)"
	# Remove executable and report directories
	rm -rf $(TARGET) $(COVERAGE_DIR) $(REPORTS_DIR)
	# Remove temporary containers
	-docker rm temp-$(IMAGE_NAME) 2>/dev/null || true
	# Remove specific images
	-docker rmi -f $(IMAGE_NAME) 2>/dev/null || true
	-docker rmi -f $(QA_IMAGE_NAME) 2>/dev/null || true
	# Remove any dangling/untagged images
	-docker image prune -f 2>/dev/null || true
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
	@echo ""
	@echo "$(GREEN)Build:$(NC)"
	@echo "  make build       - Build the executable using Docker"
	@echo "  make run         - Build and run the application"
	@echo "  make clean       - Remove build artifacts and Docker images"
	@echo ""
	@echo "$(GREEN)CI/CD Pipeline:$(NC)"
	@echo "  make ci-all      - Run all checks (linting, memory, coverage)"
	@echo "  make ci-quick    - Run quick checks (linting only)"
	@echo ""
	@echo "$(GREEN)Individual Checks:$(NC)"
	@echo "  make lint            - Run all linting checks"
	@echo "  make memory-check    - Memory leak detection"
	@echo "  make coverage        - Code coverage report"
	@echo "  make format-fix      - Auto-fix code formatting"
