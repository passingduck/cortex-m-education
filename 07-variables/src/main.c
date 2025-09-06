/*
 * Cortex-M33 지역변수 vs 전역변수 실습 예제
 * 변수 스코프와 메모리 배치, 생명주기를 상세히 분석
 */

#include <stdint.h>

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

// ========== 전역변수 선언 (.data, .bss 섹션) ==========

// 초기화된 전역변수 (.data 섹션)
int global_initialized = 0x12345678;
char global_string[] = "Global String";
int global_array[5] = {10, 20, 30, 40, 50};

// 초기화되지 않은 전역변수 (.bss 섹션)
int global_uninitialized;
char global_buffer[64];
int global_large_array[100];

// const 전역변수 (.rodata 섹션, Flash)
const int global_constant = 0xDEADBEEF;
const char global_const_string[] = "Constant String in Flash";

// static 전역변수 (파일 스코프)
static int file_scope_var = 0xAABBCCDD;
static int file_scope_uninitialized;

// ========== 함수별 변수 스코프 분석 ==========

void analyze_global_variables(void) {
    print_string("=== 전역변수 분석 ===\n");
    
    asm volatile ("nop"); // Breakpoint 1: 전역변수 분석 시작
    
    // 전역변수 주소 및 값 출력
    print_string("초기화된 전역변수:\n");
    print_string("  주소: ");
    print_hex((unsigned int)&global_initialized);
    print_string("  값: ");
    print_hex(global_initialized);
    
    print_string("전역 배열:\n");
    print_string("  주소: ");
    print_hex((unsigned int)global_array);
    print_string("  첫번째 원소: ");
    print_hex(global_array[0]);
    
    print_string("초기화되지 않은 전역변수:\n");
    print_string("  주소: ");
    print_hex((unsigned int)&global_uninitialized);
    print_string("  값: ");
    print_hex(global_uninitialized);
    
    print_string("상수 전역변수 (Flash):\n");
    print_string("  주소: ");
    print_hex((unsigned int)&global_constant);
    print_string("  값: ");
    print_hex(global_constant);
    
    // 전역변수 수정
    global_initialized = 0x87654321;
    global_uninitialized = 0x11223344;
    
    asm volatile ("nop"); // Breakpoint 2: 전역변수 수정 후
}

void analyze_local_variables(void) {
    print_string("=== 지역변수 분석 ===\n");
    
    // 다양한 타입의 지역변수
    int local_int = 0x55667788;
    char local_char = 'A';
    int local_array[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    char local_string[] = "Local String";
    
    asm volatile ("nop"); // Breakpoint 3: 지역변수 생성 후
    
    print_string("지역변수 주소 분석:\n");
    print_string("  local_int 주소: ");
    print_hex((unsigned int)&local_int);
    print_string("  local_char 주소: ");
    print_hex((unsigned int)&local_char);
    print_string("  local_array 주소: ");
    print_hex((unsigned int)local_array);
    print_string("  local_string 주소: ");
    print_hex((unsigned int)local_string);
    
    // 지역변수 값 출력
    print_string("지역변수 값:\n");
    print_string("  local_int: ");
    print_hex(local_int);
    print_string("  local_char: ");
    print_hex(local_char);
    
    // 지역변수 수정
    local_int = 0x99AABBCC;
    local_char = 'B';
    
    asm volatile ("nop"); // Breakpoint 4: 지역변수 수정 후
    
    // 스택 포인터와 지역변수 관계
    register uint32_t sp_value asm("sp");
    print_string("현재 SP: ");
    print_hex(sp_value);
    
    print_string("SP와 지역변수 거리:\n");
    print_string("  SP - local_int: ");
    print_hex(sp_value - (uint32_t)&local_int);
}

void static_variable_demo(void) {
    print_string("=== Static 변수 분석 ===\n");
    
    // static 지역변수 (함수 스코프, 하지만 전역 수명)
    static int static_local = 0xCAFEBABE;
    static int static_uninitialized;
    static int call_count = 0;
    
    call_count++;
    
    asm volatile ("nop"); // Breakpoint 5: static 변수 접근
    
    print_string("Static 지역변수:\n");
    print_string("  주소: ");
    print_hex((unsigned int)&static_local);
    print_string("  값: ");
    print_hex(static_local);
    
    print_string("호출 횟수 (static): ");
    print_hex(call_count);
    
    print_string("File scope static 변수:\n");
    print_string("  주소: ");
    print_hex((unsigned int)&file_scope_var);
    print_string("  값: ");
    print_hex(file_scope_var);
    
    // static 변수 수정 (다음 호출 때까지 유지됨)
    static_local += 0x1111;
    
    asm volatile ("nop"); // Breakpoint 6: static 변수 수정 후
}

void scope_lifetime_demo(void) {
    print_string("=== 변수 스코프 & 생명주기 데모 ===\n");
    
    // 블록 스코프 테스트
    {
        int block_local = 0x77777777;
        asm volatile ("nop"); // Breakpoint 7: 블록 스코프 내부
        
        print_string("블록 내 지역변수:\n");
        print_string("  주소: ");
        print_hex((unsigned int)&block_local);
        print_string("  값: ");
        print_hex(block_local);
        
        // 전역변수와 같은 이름의 지역변수 (shadowing)
        int global_initialized = 0x88888888;
        
        print_string("Shadowing 변수:\n");
        print_string("  지역 global_initialized: ");
        print_hex(global_initialized);
        // C에서는 전역 범위 연산자가 없으므로 외부에서 주소 전달로 해결
        
        asm volatile ("nop"); // Breakpoint 8: 변수 섀도잉
    }
    // block_local은 여기서 스코프를 벗어남
    
    asm volatile ("nop"); // Breakpoint 9: 블록 스코프 종료 후
}

void memory_layout_comparison(void) {
    print_string("=== 메모리 레이아웃 비교 ===\n");
    
    int local_var = 0x12345678;
    
    asm volatile ("nop"); // Breakpoint 10: 메모리 비교 시작
    
    // 각 변수의 메모리 주소 비교
    uint32_t addresses[] = {
        (uint32_t)&global_constant,      // Flash (.rodata)
        (uint32_t)&global_initialized,   // RAM (.data)
        (uint32_t)&global_uninitialized, // RAM (.bss)
        (uint32_t)&file_scope_var,       // RAM (.data)
        (uint32_t)&local_var             // Stack
    };
    
    const char* names[] = {
        "Global Constant (Flash)",
        "Global Initialized (Data)",
        "Global Uninitialized (BSS)",
        "File Scope Static (Data)",
        "Local Variable (Stack)"
    };
    
    print_string("메모리 주소 정렬:\n");
    for (int i = 0; i < 5; i++) {
        print_string(names[i]);
        print_string(": ");
        print_hex(addresses[i]);
    }
    
    // 주소 간격 계산
    print_string("주소 간격 분석:\n");
    print_string("  Flash-Data 간격: ");
    print_hex(addresses[1] - addresses[0]);
    print_string("  Data-BSS 간격: ");
    print_hex(addresses[2] - addresses[1]);
    print_string("  BSS-Stack 간격: ");
    print_hex(addresses[4] - addresses[2]);
    
    asm volatile ("nop"); // Breakpoint 11: 메모리 비교 완료
}

void function_parameter_analysis(int param1, char param2, int *param3) {
    print_string("=== 함수 파라미터 분석 ===\n");
    
    asm volatile ("nop"); // Breakpoint 12: 함수 파라미터 접근
    
    print_string("함수 파라미터:\n");
    print_string("  param1 주소: ");
    print_hex((unsigned int)&param1);
    print_string("  param1 값: ");
    print_hex(param1);
    
    print_string("  param2 주소: ");
    print_hex((unsigned int)&param2);
    print_string("  param2 값: ");
    print_hex(param2);
    
    print_string("  param3 주소: ");
    print_hex((unsigned int)&param3);
    print_string("  param3 값 (포인터): ");
    print_hex((unsigned int)param3);
    print_string("  param3이 가리키는 값: ");
    print_hex(*param3);
    
    // 파라미터 수정
    param1 = 0xFFFFFFFF;
    param2 = 'Z';
    *param3 = 0x00000000;
    
    asm volatile ("nop"); // Breakpoint 13: 파라미터 수정 후
}

void recursive_stack_demo(int depth) {
    print_string("=== 재귀 호출 스택 분석 ===\n");
    
    int local_in_recursion = 0x1000 + depth;
    register uint32_t sp_value asm("sp");
    
    asm volatile ("nop"); // Breakpoint 14: 재귀 호출 내부
    
    print_string("재귀 깊이: ");
    print_hex(depth);
    print_string("  지역변수 주소: ");
    print_hex((unsigned int)&local_in_recursion);
    print_string("  SP 값: ");
    print_hex(sp_value);
    
    if (depth > 0) {
        recursive_stack_demo(depth - 1);
    }
    
    asm volatile ("nop"); // Breakpoint 15: 재귀 복귀
}

int main(void) {
    print_string("Cortex-M33 지역변수 vs 전역변수 Demo\n");
    print_string("====================================\n");
    
    asm volatile ("nop"); // Breakpoint 16: main 함수 시작
    
    // 각 분석 함수 실행
    analyze_global_variables();
    analyze_local_variables();
    
    // static 변수 데모 (여러 번 호출)
    static_variable_demo();
    static_variable_demo();  // 두 번째 호출
    
    scope_lifetime_demo();
    memory_layout_comparison();
    
    // 함수 파라미터 분석
    int test_var = 0x55AA55AA;
    function_parameter_analysis(0x12345678, 'X', &test_var);
    
    // 재귀 호출 분석
    recursive_stack_demo(3);
    
    print_string("모든 변수 스코프 데모 완료!\n");
    
    // 무한 루프
    while (1) {
        asm volatile ("wfi"); // Wait for interrupt
    }
    
    return 0;
}
