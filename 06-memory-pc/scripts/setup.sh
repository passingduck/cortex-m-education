#!/bin/bash

# 06. Memory & PC Register Demo 환경 설정

echo "=== Cortex-M33 Memory & PC Register Demo 환경 설정 ==="
echo

# 빌드 디렉토리 생성
mkdir -p build

# 프로젝트 빌드
echo "프로젝트 빌드 중..."
make clean
make

if [ $? -eq 0 ]; then
    echo "✓ 빌드 성공!"
    echo "✓ 실행 파일: build/cortex-m33-memory-pc.elf"
    echo "✓ 바이너리: build/cortex-m33-memory-pc.bin"
    echo
    echo "다음 명령어로 실행하세요:"
    echo "  make run    # 일반 실행"
    echo "  make debug  # 디버그 모드 실행"
else
    echo "✗ 빌드 실패!"
    exit 1
fi
