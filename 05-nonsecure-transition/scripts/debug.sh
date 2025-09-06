#!/bin/bash
# TrustZone GDB debugging script

echo "=== TrustZone Debugging Helper ==="
echo "Starting QEMU with GDB server..."

# Kill any existing QEMU instances
pkill -f "qemu-system-arm.*mps2-an505" 2>/dev/null || true

# Start QEMU in background with GDB server
qemu-system-arm -machine mps2-an505 -cpu cortex-m33 \
    -kernel build/trustzone-nonsecure.elf \
    -nographic -semihosting-config enable=on,target=native \
    -s -S &

QEMU_PID=$!
echo "QEMU started with PID: $QEMU_PID"
sleep 2

echo ""
echo "=== GDB Commands for TrustZone Debugging ==="
echo "Useful breakpoints:"
echo "  (gdb) b Reset_Handler       # Secure boot entry"
echo "  (gdb) b Reset_Handler_NS    # Non-secure entry"
echo "  (gdb) b ns_main             # Non-secure main"
echo ""
echo "TrustZone specific commands:"
echo "  (gdb) info registers        # Current state registers"
echo "  (gdb) x/8xw 0x10000000      # Secure vector table"
echo "  (gdb) x/8xw 0x00000000      # Non-secure vector table"
echo "  (gdb) x/4xw 0xE000EDD0      # SAU registers"
echo ""
echo "Starting GDB..."

arm-none-eabi-gdb build/trustzone-nonsecure.elf \
    -ex "target remote localhost:1234" \
    -ex "b Reset_Handler" \
    -ex "b Reset_Handler_NS" \
    -ex "b ns_main" \
    -ex "continue"

# Cleanup
kill $QEMU_PID 2>/dev/null || true
