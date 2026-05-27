# Usage Instructions
## Install dependencies (Ubuntu/Debian)
sudo apt-get install build-essential cmake clang clang-tidy clang-format valgrind gcovr mingw-w64

# Or use Docker for consistent environment
docker build -f Dockerfile.cross -t config-app-builder .
Development Workflow
bash
# Format code
make format

# Lint code
make lint

# Build and test locally
make build
make test

# Run with valgrind
make valgrind

# Cross-compile for all platforms
make cross

# Using Docker (reproducible builds)
make docker-build
make docker-test
make docker-lint
