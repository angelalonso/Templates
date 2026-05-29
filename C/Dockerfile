FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    pkg-config \
    libgtk-3-dev \
    libwebkit2gtk-4.1-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy source code
COPY src/ ./src/

# Build the executable directly
RUN gcc -Wall -Wextra -Wno-unused-parameter -O2 \
    `pkg-config --cflags gtk+-3.0 webkit2gtk-4.1` \
    -c src/main.c -o main.o && \
    gcc -Wall -Wextra -Wno-unused-parameter -O2 \
    `pkg-config --cflags gtk+-3.0 webkit2gtk-4.1` \
    -c src/config.c -o config.o && \
    gcc -o config_editor main.o config.o \
    `pkg-config --libs gtk+-3.0 webkit2gtk-4.1` && \
    rm *.o

# Verify the executable exists
RUN test -f /app/config_editor && echo "✓ Executable built successfully" || (echo "✗ Executable not found" && exit 1)

# Keep the container alive for copying
CMD ["/bin/sh", "-c", "while true; do sleep 1; done"]
