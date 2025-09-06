#!/bin/bash

# 07. Variables Scope Demo 실행 스크립트

echo "=== Cortex-M33 Variables Scope Demo 실행 ==="
echo

# 빌드가 되어있는지 확인
if [ ! -f "build/cortex-m33-variables.elf" ]; then
    echo "빌드 파일이 없습니다. 먼저 빌드를 실행하세요:"
    echo "  make"
    exit 1
fi

echo "QEMU에서 Cortex-M33 Variables Demo 실행 중..."
echo "종료하려면 Ctrl+A, X를 누르세요."
echo

make run
