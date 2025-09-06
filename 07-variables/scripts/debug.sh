#!/bin/bash

# 07. Variables Scope Demo 디버그 스크립트

echo "=== Cortex-M33 Variables Scope Demo 디버그 모드 ==="
echo

# 빌드가 되어있는지 확인
if [ ! -f "build/cortex-m33-variables.elf" ]; then
    echo "빌드 파일이 없습니다. 먼저 빌드를 실행하세요:"
    echo "  make"
    exit 1
fi

echo "QEMU GDB 서버 시작 중..."
echo "다른 터미널에서 다음 명령어로 GDB 연결:"
echo "  gdb-multiarch build/cortex-m33-variables.elf"
echo "  (gdb) target remote :1234"
echo "  (gdb) load"
echo "  (gdb) break main"
echo "  (gdb) continue"
echo
echo "변수 분석을 위한 유용한 GDB 명령어:"
echo "  (gdb) info variables     # 전역변수 목록"
echo "  (gdb) info locals        # 지역변수 목록"
echo "  (gdb) print &variable    # 변수 주소"
echo "  (gdb) x/8wx \$sp         # 스택 내용"
echo
echo "종료하려면 Ctrl+C를 누르세요."
echo

make debug
