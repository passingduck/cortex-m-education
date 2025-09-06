#!/bin/bash
# TrustZone development environment setup

echo "=== TrustZone Development Environment Setup ==="

# Check required tools
echo "Checking required tools..."

# Check ARM toolchain
if ! command -v arm-none-eabi-gcc &> /dev/null; then
    echo "❌ arm-none-eabi-gcc not found"
    echo "Install: sudo apt-get install gcc-arm-none-eabi"
    exit 1
else
    echo "✅ ARM toolchain found: $(arm-none-eabi-gcc --version | head -n1)"
fi

# Check QEMU
if ! command -v qemu-system-arm &> /dev/null; then
    echo "❌ qemu-system-arm not found"
    echo "Install: sudo apt-get install qemu-system-arm"
    exit 1
else
    echo "✅ QEMU found: $(qemu-system-arm --version | head -n1)"
fi

# Check GDB
if ! command -v gdb-multiarch &> /dev/null; then
    if ! command -v arm-none-eabi-gdb &> /dev/null; then
        echo "❌ GDB not found"
        echo "Install: sudo apt-get install gdb-multiarch"
        exit 1
    else
        echo "✅ ARM GDB found: $(arm-none-eabi-gdb --version | head -n1)"
    fi
else
    echo "✅ GDB found: $(gdb-multiarch --version | head -n1)"
fi

echo ""
echo "=== TrustZone Capabilities Check ==="

# Check QEMU TrustZone support
echo "Checking QEMU TrustZone support..."
if qemu-system-arm -machine help | grep -q "mps2-an505"; then
    echo "✅ QEMU MPS2-AN505 support found"
else
    echo "❌ QEMU MPS2-AN505 not found"
    echo "Update QEMU to a version that supports MPS2-AN505"
    exit 1
fi

# Check Cortex-M33 support
if qemu-system-arm -cpu help | grep -q "cortex-m33"; then
    echo "✅ QEMU Cortex-M33 support found"
else
    echo "❌ QEMU Cortex-M33 not found"
    echo "Update QEMU to a version that supports Cortex-M33"
    exit 1
fi

echo ""
echo "=== Project Setup ==="

# Make scripts executable
chmod +x scripts/*.sh
echo "✅ Scripts made executable"

# Create build directory
mkdir -p build
echo "✅ Build directory created"

echo ""
echo "🎉 Setup complete!"
echo ""
echo "Next steps:"
echo "  1. Build project: make"
echo "  2. Run demo: make run"
echo "  3. Debug: make debug"
echo ""
echo "For more information: make help"
