#!/bin/bash

# 06. Memory & PC Register Demo 디버그 스크립트

echo "=== Cortex-M33 Memory & PC Register Demo 디버그 모드 ==="
echo

# 빌드가 되어있는지 확인
if [ ! -f "build/cortex-m33-memory-pc.elf" ]; then
    echo "빌드 파일이 없습니다. 먼저 빌드를 실행하세요:"
    echo "  make"
    exit 1
fi

echo "QEMU GDB 서버 시작 중..."
echo "다른 터미널에서 다음 명령어로 GDB 연결:"
echo "  gdb-multiarch build/cortex-m33-memory-pc.elf"
echo "  (gdb) target remote :1234"
echo "  (gdb) load"
echo "  (gdb) break main"
echo "  (gdb) continue"
echo
echo "PC 추적을 위한 유용한 GDB 명령어:"
echo "  (gdb) display /x \$pc"
echo "  (gdb) info registers pc sp lr"
echo "  (gdb) x/10i \$pc"
echo
echo "종료하려면 Ctrl+C를 누르세요."
echo

make debug
