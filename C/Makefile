IMAGE_NAME = config-editor-builder
CONTAINER_NAME = config-editor-build
TARGET = config_editor

# Build the executable using Docker
all: build

# Build Docker image
docker-image:
	@echo "Building Docker image..."
	docker build -t $(IMAGE_NAME) .

# Build executable in Docker container and copy to host
build: docker-image
	@echo "Building executable in Docker..."
	@docker rm -f $(CONTAINER_NAME) 2>/dev/null || true
	@docker run --name $(CONTAINER_NAME) \
		-v "$(PWD):/output" \
		$(IMAGE_NAME) \
		bash -c "cp /app/config_editor /output/ 2>/dev/null && chmod +x /output/config_editor" || \
		(echo "Build failed! Check Docker logs for details" && exit 1)
	@docker rm $(CONTAINER_NAME) 2>/dev/null || true
	@if [ -f "./$(TARGET)" ]; then \
		echo "✓ Build successful! Executable: ./$(TARGET)"; \
		file ./$(TARGET); \
		ldd ./$(TARGET) | head -5; \
	else \
		echo "✗ Build failed!"; \
		exit 1; \
	fi

# Quick build (no image rebuild)
quick-build:
	@echo "Quick building in Docker (using cached image)..."
	@docker run --name $(CONTAINER_NAME) \
		-v "$(PWD):/output" \
		$(IMAGE_NAME) \
		bash -c "cd /app && \
			gcc -Wall -Wextra -Wno-unused-parameter -O2 \
			`pkg-config --cflags gtk+-3.0 webkit2gtk-4.1` \
			-c src/main.c -o main.o && \
			gcc -Wall -Wextra -Wno-unused-parameter -O2 \
			`pkg-config --cflags gtk+-3.0 webkit2gtk-4.1` \
			-c src/config.c -o config.o && \
			gcc -o config_editor main.o config.o \
			`pkg-config --libs gtk+-3.0 webkit2gtk-4.1` && \
			cp config_editor /output/ && \
			chmod +x /output/config_editor" || \
		(echo "Build failed!" && exit 1)
	@docker rm $(CONTAINER_NAME) 2>/dev/null || true
	@echo "✓ Build successful! Executable: ./$(TARGET)"

# Clean - remove executable and Docker image
clean:
	@echo "Cleaning up..."
	rm -f $(TARGET)
	-docker rmi $(IMAGE_NAME) 2>/dev/null || true
	-docker rm $(CONTAINER_NAME) 2>/dev/null || true
	@echo "Cleanup complete"

# Run the executable (must be built first)
run: $(TARGET)
	@if [ -f "./$(TARGET)" ]; then \
		./$(TARGET); \
	else \
		echo "Executable not found. Run 'make build' first."; \
		exit 1; \
	fi

# Rebuild without cache
rebuild: clean build

.PHONY: all docker-image build clean run rebuild quick-build
