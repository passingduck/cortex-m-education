#!/bin/bash
# TrustZone Non-secure transition run script

echo "=== TrustZone Non-secure Transition Demo ==="
echo "Building and running on QEMU MPS2-AN505..."
echo ""

# Build the project
make clean
if ! make; then
    echo "Build failed!"
    exit 1
fi

echo ""
echo "=== Execution Flow ==="
echo "1. Secure boot (Reset_Handler) starts at 0x10000000"
echo "2. SAU configuration for Non-secure regions"
echo "3. Transition to Non-secure state (BXNS)"
echo "4. Non-secure application runs at 0x00000000 with stack at 0x20000000"
echo ""
echo "Press Ctrl+A then X to exit QEMU"
echo "Starting execution..."
echo ""

# Run in QEMU
make run
