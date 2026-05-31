# Dockerfile
FROM ubuntu:22.04 AS build

# Set non-interactive mode to prevent timezone prompts
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=UTC

RUN apt-get update && apt-get install -y \
    build-essential \
    pkg-config \
    libgtk-3-dev \
    libwebkit2gtk-4.1-dev \
    libjson-c-dev \
    git \
    tzdata \
    && ln -fs /usr/share/zoneinfo/$TZ /etc/localtime \
    && dpkg-reconfigure --frontend noninteractive tzdata \
    && rm -rf /var/lib/apt/lists/*

ENV DEBIAN_FRONTEND=

WORKDIR /build

# Copy source files from src directory
COPY src/main.c .
COPY src/backend/ ./backend/
COPY src/middleware/ ./middleware/
COPY src/web/ ./web/

# Build the application
RUN gcc -o config_editor \
    main.c \
    backend/config_manager.c \
    backend/api_handler.c \
    backend/webkit_handler.c \
    $(pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.1) \
    -ljson-c -Wall -Wextra -O2

# ============================================
# QA STAGE - Includes all analysis tools
# ============================================
FROM build AS qa

# Install QA and analysis tools
RUN apt-get update && apt-get install -y \
    cppcheck \
    clang-tidy \
    clang-format \
    flawfinder \
    valgrind \
    gcovr \
    lcov \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace

CMD ["/bin/bash"]

# ============================================
# RUNTIME STAGE
# ============================================
FROM ubuntu:22.04 AS runtime

RUN apt-get update && apt-get install -y \
    libgtk-3-0 \
    libwebkit2gtk-4.1-0 \
    libjson-c5 \
    && rm -rf /var/lib/apt/lists/*

# Create the web directory
RUN mkdir -p /usr/local/share/config-editor/web

# Copy from build stage
COPY --from=build /build/config_editor /usr/local/bin/config_editor
COPY --from=build /build/web/ /usr/local/share/config-editor/web/

# Set working directory
WORKDIR /usr/local/share/config-editor

# Run the application
CMD ["config_editor"]
