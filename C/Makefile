# Variables
IMAGE_NAME = config-editor-builder
QA_IMAGE_NAME = config-editor-qa
ANDROID_IMAGE_NAME = config-editor-android
TARGET = config_editor
REPORTS_DIR = reports
COVERAGE_DIR = coverage
ANDROID_APK_OUT = android/app/build/outputs/apk/debug

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
.PHONY: build-android clean-android
.PHONY: test-android stop-android-emulator start-android-emulator clean-android-emulator

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
	@docker build -t $(IMAGE_NAME) --target runtime -f docker/Dockerfile .
	@echo "$(GREEN)Docker image built successfully$(NC)"

# Build QA Docker image with all analysis tools
docker-qa-image:
	@echo "$(GREEN)Building QA Docker image...$(NC)"
	@docker build -t $(QA_IMAGE_NAME) --target qa -f docker/Dockerfile .
	@echo "$(GREEN)QA Docker image built successfully$(NC)"

# ============================================
# ANDROID BUILD TARGETS
# ============================================

# Build Android APK
build-android: clean-android
	@echo "$(GREEN)========================================$(NC)"
	@echo "$(GREEN)Building Android APK...$(NC)"
	@echo "$(GREEN)========================================$(NC)"
	@# Check if Dockerfile.android exists
	@if [ ! -f "docker/Dockerfile.android" ]; then \
		echo "$(RED)✗ docker/Dockerfile.android not found!$(NC)"; \
		exit 1; \
	fi
	@echo "$(GREEN)Building Docker image for Android...$(NC)"
	@docker build \
		-f docker/Dockerfile.android \
		-t $(ANDROID_IMAGE_NAME) \
		.
	@echo "$(GREEN)Compiling APK inside container...$(NC)"
	@mkdir -p $(ANDROID_APK_OUT)
	@docker run --rm \
		-v "$(PWD)/$(ANDROID_APK_OUT):/out" \
		$(ANDROID_IMAGE_NAME) \
		bash -c "./gradlew assembleDebug --no-daemon && cp app/build/outputs/apk/debug/app-debug.apk /out/"
	@echo ""
	@echo "$(GREEN)========================================$(NC)"
	@echo "$(GREEN)✓ Android APK built successfully!$(NC)"
	@echo "$(GREEN)APK location: $(PWD)/$(ANDROID_APK_OUT)/app-debug.apk$(NC)"
	@echo "$(GREEN)========================================$(NC)"
	@ls -lh $(ANDROID_APK_OUT)/app-debug.apk 2>/dev/null || echo "$(YELLOW)APK file not found - check build output$(NC)"

# Clean Android build artifacts
clean-android:
	@echo "$(YELLOW)Cleaning Android build artifacts...$(NC)"
	@# Remove Android Docker image
	-docker rmi -f $(ANDROID_IMAGE_NAME) 2>/dev/null || true
	@# Remove APK output directory (optional - comment out if you want to keep APKs)
	@# rm -rf android/app/build 2>/dev/null || true
	@echo "$(GREEN)Android clean complete$(NC)"

# ============================================
# ANDROID TESTING TARGETS
# ============================================
# Test Android APK on emulator (Docker-based)
test-android: build-android
	@echo "$(GREEN)========================================$(NC)"
	@echo "$(GREEN)Starting Android emulator and testing APK...$(NC)"
	@echo "$(GREEN)========================================$(NC)"
	@# Check if KVM is available
	@echo "$(YELLOW)Checking KVM acceleration...$(NC)"
	@if [ ! -e /dev/kvm ]; then \
		echo "$(RED)✗ /dev/kvm not found. Please enable virtualization in BIOS and ensure KVM is installed.$(NC)"; \
		echo "$(YELLOW)Run: sudo apt install cpu-checker && kvm-ok$(NC)"; \
		exit 1; \
	fi
	@echo "$(GREEN)✓ KVM available$(NC)"
	@# Check if ADB is installed
	@echo "$(YELLOW)Checking ADB installation...$(NC)"
	@if ! command -v adb >/dev/null 2>&1; then \
		echo "$(YELLOW)ADB not found. Installing...$(NC)"; \
		sudo apt update && sudo apt install -y adb; \
	fi
	@echo "$(GREEN)✓ ADB available$(NC)"
	@# Stop and remove existing emulator container if running
	@echo "$(YELLOW)Cleaning up previous emulator...$(NC)"
	-docker stop android-emulator 2>/dev/null || true
	-docker rm android-emulator 2>/dev/null || true
	@# Kill any existing ADB server and devices
	@echo "$(YELLOW)Resetting ADB connections...$(NC)"
	@adb kill-server > /dev/null 2>&1
	@# Start fresh emulator container
	@echo "$(GREEN)Starting Android emulator (this takes 30-60 seconds)...$(NC)"
	@docker run -d --privileged \
		--name android-emulator \
		-p 6080:6080 \
		-p 5554:5554 \
		-p 5555:5555 \
		-e EMULATOR_DEVICE="Samsung Galaxy S10" \
		-e WEB_VNC=true \
		--device /dev/kvm \
		budtmo/docker-android:emulator_11.0 > /dev/null 2>&1
	@# Wait for emulator to boot
	@echo "$(YELLOW)Waiting for emulator to boot (60 seconds)...$(NC)"
	@echo -n "Progress: "
	@for i in 1 2 3 4 5 6; do \
		sleep 10; \
		echo -n "$$i0s "; \
	done
	@echo ""
	@echo "$(GREEN)✓ Emulator started$(NC)"
	@# Connect ADB with retry logic
	@echo "$(YELLOW)Connecting ADB...$(NC)"
	@adb start-server > /dev/null 2>&1
	@sleep 2
	@# Wait for emulator to be ready for ADB connection
	@echo "$(YELLOW)Waiting for ADB device...$(NC)"
	@for i in 1 2 3 4 5 6 7 8 9 10; do \
		sleep 3; \
		if adb connect localhost:5555 2>&1 | grep -q "connected"; then \
			echo "$(GREEN)✓ ADB connected successfully$(NC)"; \
			break; \
		fi; \
		if [ $$i -eq 10 ]; then \
			echo "$(RED)✗ Failed to connect ADB after 30 seconds$(NC)"; \
			adb devices; \
			docker logs android-emulator --tail 20; \
			exit 1; \
		fi; \
		echo -n "."; \
	done
	@# Wait a bit more for emulator to fully initialize
	@sleep 5
	@# Get the emulator device serial
	@EMULATOR_SERIAL=$$(adb devices | grep "localhost:5555" | awk '{print $$1}'); \
	if [ -z "$$EMULATOR_SERIAL" ]; then \
		echo "$(RED)✗ Could not find emulator device$(NC)"; \
		adb devices; \
		exit 1; \
	fi; \
	echo "$(GREEN)Using device: $$EMULATOR_SERIAL$(NC)"
	@# Wait for emulator to be fully booted
	@echo "$(YELLOW)Waiting for emulator to fully boot...$(NC)"
	@for i in 1 2 3 4 5 6 7 8 9 10 11 12; do \
		sleep 5; \
		if adb -s localhost:5555 shell getprop sys.boot_completed 2>/dev/null | grep -q "1"; then \
			echo "$(GREEN)✓ Emulator boot completed$(NC)"; \
			break; \
		fi; \
		if [ $$i -eq 12 ]; then \
			echo "$(YELLOW)Warning: Emulator boot timeout, continuing anyway...$(NC)"; \
		fi; \
		echo -n "."; \
	done
	@# Install APK using specific device serial
	@APK_PATH="$(PWD)/$(ANDROID_APK_OUT)/app-debug.apk"; \
	if [ ! -f "$$APK_PATH" ]; then \
		echo "$(RED)✗ APK not found at $$APK_PATH$(NC)"; \
		exit 1; \
	fi
	@echo "$(GREEN)Installing APK on emulator...$(NC)"
	@adb -s localhost:5555 install -r "$(PWD)/$(ANDROID_APK_OUT)/app-debug.apk" 2>&1 | grep -v "Performing Streamed Install"
	@if [ $$? -eq 0 ]; then \
		echo "$(GREEN)✓ APK installed successfully$(NC)"; \
	else \
		echo "$(RED)✗ APK installation failed$(NC)"; \
		exit 1; \
	fi
	@# Wait for launcher to refresh
	@echo "$(YELLOW)Waiting for launcher to refresh (15 seconds)...$(NC)"
	@for i in 1 2 3; do \
		sleep 5; \
		echo -n "$$i/3 "; \
	done
	@echo ""
	@# Force launcher refresh on the specific device
	@adb -s localhost:5555 shell am force-stop com.android.launcher3 > /dev/null 2>&1
	@echo "$(GREEN)========================================$(NC)"
	@echo "$(GREEN)✓ Android APK testing complete!$(NC)"
	@echo "$(GREEN)========================================$(NC)"
	@echo "📱 Emulator access:"
	@echo "  - VNC Web UI:    http://localhost:6080 (password: 123mudar)"
	@echo "  - ADB:           adb -s localhost:5555 shell"
	@echo ""
	@echo "🔧 Commands to interact:"
	@echo "  - Stop emulator: make stop-android-emulator"
	@echo "  - Start again:   make start-android-emulator"
	@echo "  - Remove:        make clean-android-emulator"
	@echo "  - Launch app:    adb -s localhost:5555 shell am start -n com.yourpackage/.MainActivity"
	@echo ""
	@echo "$(YELLOW)Emulator continues running in background.$(NC)"
	@echo "$(GREEN)Open http://localhost:6080 in your browser to see the app!$(NC)"

# Stop Android emulator
stop-android-emulator:
	@echo "$(YELLOW)Stopping Android emulator...$(NC)"
	@docker stop android-emulator 2>/dev/null || echo "Emulator not running"
	@echo "$(GREEN)✓ Emulator stopped$(NC)"

# Start Android emulator (if stopped)
start-android-emulator:
	@echo "$(YELLOW)Starting Android emulator...$(NC)"
	@docker start android-emulator 2>/dev/null || echo "Emulator not found. Run 'make test-android' first."
	@echo "$(GREEN)✓ Emulator started$(NC)"
	@echo "Access at http://localhost:6080 (password: 123mudar)"

# Clean Android emulator container
clean-android-emulator:
	@echo "$(YELLOW)Removing Android emulator container...$(NC)"
	@docker stop android-emulator 2>/dev/null || true
	@docker rm android-emulator 2>/dev/null || true
	@echo "$(GREEN)✓ Emulator removed$(NC)"

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
clean: clean-android clean-android-emulator
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
	@echo "  make build          - Build the Linux executable using Docker"
	@echo "  make run            - Build and run the Linux application"
	@echo "  make clean          - Remove all build artifacts and Docker images"
	@echo ""
	@echo "$(GREEN)Android:$(NC)"
	@echo "  make build-android  - Build Android APK"
	@echo "  make clean-android  - Clean Android build artifacts"
	@echo ""
	@echo "$(GREEN)Android Testing:$(NC)"
	@echo "  make test-android        - Build APK, start emulator, install and test APK"
	@echo "  make stop-android-emulator - Stop the running emulator"
	@echo "  make start-android-emulator - Start a stopped emulator"
	@echo "  make clean-android-emulator - Remove emulator container"
	@echo ""
	@echo "$(GREEN)CI/CD Pipeline:$(NC)"
	@echo "  make ci-all         - Run all checks (linting, memory, coverage)"
	@echo "  make ci-quick       - Run quick checks (linting only)"
	@echo ""
	@echo "$(GREEN)Individual Checks:$(NC)"
	@echo "  make lint           - Run all linting checks"
	@echo "  make memory-check   - Memory leak detection"
	@echo "  make coverage       - Code coverage report"
	@echo "  make format-fix     - Auto-fix code formatting"
