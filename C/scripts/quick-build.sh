#!/bin/bash
set -e

print_status() {
    echo "[INFO] $1"
}

print_status "Quick build - compiling only..."
print_status "==================================="

# Create build directory
mkdir -p build_quick
cd build_quick

# Configure and build
cmake .. -DBUILD_TESTS=OFF -DBUILD_GUI=OFF
make -j$(nproc)

# Copy to main directory
if [ -f "./config_cli" ]; then
    cp ./config_cli /app/config_app
    chmod +x /app/config_app
    print_status "Executable copied to /app/config_app"
elif [ -f "./config_app" ]; then
    cp ./config_app /app/config_app
    chmod +x /app/config_app
    print_status "Executable copied to /app/config_app"
fi

# Create default config if missing
if [ ! -f "/app/cfg.yml" ]; then
    cat > /app/cfg.yml << 'EOF'
text: Initial content
version: 1.0
author: Developer
EOF
    print_status "Created default cfg.yml"
fi

print_status "Quick build complete"
