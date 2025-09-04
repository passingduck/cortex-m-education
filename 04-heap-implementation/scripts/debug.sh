#!/bin/bash
# GDB debugging script for Cortex-M33 Hello World

set -e

# 프로젝트 루트 디렉토리로 이동
cd "$(dirname "$0")/.."

echo "Starting GDB debugging session..."
echo "================================="

# 빌드 (필요한 경우)
if [ ! -f "build/cortex-m33-hello-world.elf" ]; then
    echo "Building project first..."
    make
fi

echo ""
echo "Starting QEMU with GDB server..."
echo "GDB will connect automatically"
echo ""

# 디버깅 시작
make debug