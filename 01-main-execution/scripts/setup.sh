#!/bin/bash
# Setup script for Cortex-M33 Hello World project
# This script installs required dependencies for Ubuntu/Debian

set -e

echo "Setting up Cortex-M33 Hello World development environment..."
echo "============================================================"

# 시스템 업데이트
echo "Updating package list..."
sudo apt update

# ARM GCC 툴체인 설치
echo "Installing ARM GCC toolchain..."
sudo apt install -y gcc-arm-none-eabi

# QEMU ARM 시스템 에뮬레이터 설치
echo "Installing QEMU ARM system emulator..."
sudo apt install -y qemu-system-arm

# GDB multiarch 설치 (디버깅용)
echo "Installing GDB multiarch..."
sudo apt install -y gdb-multiarch

# 빌드 필수 도구 설치
echo "Installing build essentials..."
sudo apt install -y build-essential git

echo ""
echo "Verifying installations..."
echo "=========================="

# 설치 확인
echo -n "ARM GCC: "
arm-none-eabi-gcc --version | head -1

echo -n "QEMU ARM: "
qemu-system-arm --version | head -1

echo -n "GDB multiarch: "
gdb-multiarch --version | head -1

echo -n "Make: "
make --version | head -1

echo ""
echo "✅ All dependencies installed successfully!"
echo ""
echo "Next steps:"
echo "  1. cd to project directory"
echo "  2. Run: make"
echo "  3. Run: make run"
echo ""
echo "For more information, see README.md"
