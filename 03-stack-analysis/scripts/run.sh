#!/bin/bash
# Simple run script for Cortex-M33 Hello World

set -e

# 프로젝트 루트 디렉토리로 이동
cd "$(dirname "$0")/.."

echo "Building and running Cortex-M33 Hello World..."
echo "=============================================="

# 빌드
echo "Building project..."
make

echo ""
echo "Running on QEMU MPS2-AN505..."
echo "Press Ctrl+A then X to exit QEMU"
echo ""

# 실행
make run