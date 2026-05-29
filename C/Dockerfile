# Dockerfile
FROM ubuntu:22.04 AS builder

RUN apt-get update && apt-get install -y \
    build-essential \
    pkg-config \
    libgtk-3-dev \
    libwebkit2gtk-4.1-dev \
    libjson-c-dev \
    git \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build

# Copy source files from src directory
COPY src/main.c .
COPY src/backend/ ./backend/
COPY src/middleware/ ./middleware/
COPY src/web/ ./web/

# List web files for debugging
RUN echo "=== Web files in build ===" && ls -la ./web/

# Build the application
RUN gcc -o config_editor \
    main.c \
    backend/config_manager.c \
    backend/api_handler.c \
    backend/webkit_handler.c \
    $(pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.1) \
    -ljson-c -Wall -Wextra -O2 && \
    echo "Build successful!" && \
    ls -la config_editor

# Runtime stage
FROM ubuntu:22.04 AS runtime
RUN apt-get update && apt-get install -y \
    libgtk-3-0 \
    libwebkit2gtk-4.1-0 \
    libjson-c5 \
    && rm -rf /var/lib/apt/lists/*

# Create the web directory
RUN mkdir -p /usr/local/share/config-editor/web

# Copy from builder stage
COPY --from=builder /build/config_editor /usr/local/bin/config_editor
COPY --from=builder /build/web/ /usr/local/share/config-editor/web/

# Verify web files were copied
RUN echo "=== Web files in runtime ===" && ls -la /usr/local/share/config-editor/web/

# Set working directory
WORKDIR /usr/local/share/config-editor

# Run the application
CMD ["config_editor"]
