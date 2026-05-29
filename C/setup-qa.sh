#!/bin/bash

echo "Setting up QA environment..."

# Create necessary directories
mkdir -p build coverage reports tests

# Create a sample config for testing if it doesn't exist
if [ ! -f tests/test_config.yml ]; then
    cat > tests/test_config.yml << 'EOF'
text: |
  This is a test configuration.
  It contains multiple lines.
  Testing YAML parsing.
EOF
    echo "Created test config in tests/test_config.yml"
fi

# Make scripts executable
chmod +x setup-qa.sh

echo "Setup complete! Run 'make qa-all' to run all quality checks"
