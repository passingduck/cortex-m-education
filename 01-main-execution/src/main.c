/*
 * Hello World using working example structure
 * MPS2-AN505 + Cortex-M33
 */

// ARM Semihosting - 공식 예제와 동일한 구조
int semihost_call(int reason, void* arg) {
    int result;
    __asm__ volatile (
        "mov r0, %1\n"
        "mov r1, %2\n"
        "bkpt #0xAB\n"
        "mov %0, r0\n"
        : "=r" (result)
        : "r" (reason), "r" (arg)
        : "r0", "r1", "memory"
    );
    return result;
}

void print_string(const char* str) {
    semihost_call(0x04, (void*)str);  // SYS_WRITE0
}

void exit_program(int code) {
    semihost_call(0x18, &code);  // SYS_EXIT
    while(1);
}

// 공식 예제와 동일하게 void main 사용
void main(void) {
    print_string("=================================\n");
    print_string("Hello World from Fixed Project!\n");
    print_string("=================================\n");
    print_string("MPS2-AN505 + Cortex-M33\n");
    print_string("Using Official Example Structure\n");
    print_string("Semihosting is working!\n\n");
    
    // 카운터 테스트
    for (int i = 0; i < 5; i++) {
        print_string("Count: ");
        char num = '0' + i;
        char num_str[3] = {num, '\n', '\0'};
        print_string(num_str);
    }
    
    print_string("\nAll tests completed successfully!\n");
    print_string("Project fixed using official example!\n");
    exit_program(0);
}