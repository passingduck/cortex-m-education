/*
 * Cortex-M33 General Register & ALU 실습 예제
 * 다양한 연산을 통해 레지스터 상태 변화를 관찰
 */

// Semihosting을 위한 함수 선언
int print_string(const char *str) {
    register int r0 asm("r0");
    register int r1 asm("r1");
    
    r0 = 0x04;  /* SYS_WRITE0 */
    r1 = (int)str;
    
    asm volatile ("bkpt #0xAB" : "=r"(r0) : "r"(r0), "r"(r1) : "memory");
    return r0;
}

void print_hex(unsigned int value) {
    char hex_str[12] = "0x00000000\n";
    const char hex_chars[] = "0123456789ABCDEF";
    
    for (int i = 7; i >= 0; i--) {
        hex_str[2 + i] = hex_chars[value & 0xF];
        value >>= 4;
    }
    
    print_string(hex_str);
}

// 레지스터 상태를 확인하는 함수들
void register_demo_basic(void) {
    print_string("=== 기본 레지스터 연산 데모 ===\n");
    
    // 기본 산술 연산 (breakpoint에서 r0~r3 관찰)
    register int a asm("r0") = 0x12345678;
    register int b asm("r1") = 0x87654321;
    register int result asm("r2");
    
    asm volatile ("nop"); // Breakpoint 1: 초기값 확인
    
    result = a + b;       // ADD 연산
    asm volatile ("nop"); // Breakpoint 2: ADD 결과 확인
    
    result = a - b;       // SUB 연산
    asm volatile ("nop"); // Breakpoint 3: SUB 결과 확인
    
    result = a * b;       // MUL 연산 (하위 32비트)
    asm volatile ("nop"); // Breakpoint 4: MUL 결과 확인
}

void register_demo_bitwise(void) {
    print_string("=== 비트 연산 데모 ===\n");
    
    register int data asm("r0") = 0xF0F0F0F0;
    register int mask asm("r1") = 0x0F0F0F0F;
    register int result asm("r2");
    
    asm volatile ("nop"); // Breakpoint 5: 초기값 확인
    
    result = data & mask; // AND 연산
    asm volatile ("nop"); // Breakpoint 6: AND 결과 확인
    
    result = data | mask; // OR 연산
    asm volatile ("nop"); // Breakpoint 7: OR 결과 확인
    
    result = data ^ mask; // XOR 연산
    asm volatile ("nop"); // Breakpoint 8: XOR 결과 확인
    
    result = ~data;       // NOT 연산
    asm volatile ("nop"); // Breakpoint 9: NOT 결과 확인
}

void register_demo_shift(void) {
    print_string("=== 시프트 연산 데모 ===\n");
    
    register int value asm("r0") = 0x12345678;
    register int shift_count asm("r1") = 4;
    register int result asm("r2");
    
    asm volatile ("nop"); // Breakpoint 10: 초기값 확인
    
    result = value << shift_count;  // 좌측 시프트
    asm volatile ("nop"); // Breakpoint 11: LSL 결과 확인
    
    result = value >> shift_count;  // 우측 시프트
    asm volatile ("nop"); // Breakpoint 12: LSR 결과 확인
    
    // 음수값으로 산술 시프트 테스트
    value = 0x80000000;
    result = (int)value >> shift_count;  // 산술 우측 시프트
    asm volatile ("nop"); // Breakpoint 13: ASR 결과 확인
}

void register_demo_conditional(void) {
    print_string("=== 조건부 연산 및 플래그 데모 ===\n");
    
    register int a asm("r0") = 100;
    register int b asm("r1") = 50;
    register int result asm("r2");
    
    asm volatile ("nop"); // Breakpoint 14: 초기값 확인
    
    // 비교 연산 (CPSR 플래그 설정)
    asm volatile ("cmp %0, %1" : : "r"(a), "r"(b) : "cc");
    asm volatile ("nop"); // Breakpoint 15: CMP 후 플래그 확인
    
    // 조건부 실행 (GT: Greater Than) - IT 블록 사용
    asm volatile ("it gt");
    asm volatile ("movgt %0, #1" : "=r"(result) : : "cc");
    asm volatile ("nop"); // Breakpoint 16: 조건부 실행 결과 확인
    
    // 값 교체해서 다시 테스트
    a = 30;
    b = 70;
    asm volatile ("cmp %0, %1" : : "r"(a), "r"(b) : "cc");
    asm volatile ("it gt");
    asm volatile ("movgt %0, #1" : "=r"(result) : : "cc");
    asm volatile ("it le");
    asm volatile ("movle %0, #0" : "=r"(result) : : "cc");
    asm volatile ("nop"); // Breakpoint 17: 두 번째 조건부 실행 결과
}

void register_demo_stack_operations(void) {
    print_string("=== 스택 연산 데모 ===\n");
    
    register int val1 asm("r0") = 0xDEADBEEF;
    register int val2 asm("r1") = 0xCAFEBABE;
    register int val3 asm("r2") = 0x12345678;
    register int temp asm("r3");
    
    asm volatile ("nop"); // Breakpoint 18: 스택 연산 전 상태
    
    // 스택에 푸시
    asm volatile ("push {%0, %1, %2}" : : "r"(val1), "r"(val2), "r"(val3) : "memory");
    asm volatile ("nop"); // Breakpoint 19: 푸시 후 스택 확인
    
    // 레지스터 값 변경
    val1 = 0;
    val2 = 0;
    val3 = 0;
    asm volatile ("nop"); // Breakpoint 20: 레지스터 클리어 후
    
    // 스택에서 팝 (순서 주의: 스택은 LIFO)
    asm volatile ("pop {%0, %1, %2}" : "=r"(val1), "=r"(val2), "=r"(val3) : : "memory");
    asm volatile ("nop"); // Breakpoint 21: 팝 후 복원된 값 확인
}

int main(void) {
    print_string("Cortex-M33 Register & ALU Demo\n");
    print_string("===============================\n");
    
    // 각 데모 함수 실행
    register_demo_basic();
    register_demo_bitwise();
    register_demo_shift();
    register_demo_conditional();
    register_demo_stack_operations();
    
    print_string("모든 레지스터 데모 완료!\n");
    
    // 무한 루프
    while (1) {
        asm volatile ("wfi"); // Wait for interrupt
    }
    
    return 0;
}
