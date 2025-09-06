/*
 * Cortex-M33 메인메모리 & PC레지스터 실습 예제
 * 메모리 구조와 PC의 동작을 상세히 관찰
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

// 다양한 메모리 영역에 위치할 데이터들
const char flash_string[] = "Flash에 저장된 문자열";  // Flash 메모리
static int initialized_data = 0x12345678;            // .data 섹션 (RAM)
static int uninitialized_data;                       // .bss 섹션 (RAM)

// PC 레지스터 동작을 관찰하는 함수들
void function_a(void) {
    print_string("=== Function A 실행 ===\n");
    asm volatile ("nop"); // Breakpoint 1: function_a의 PC
    
    // 현재 PC 값 획득 방법 1: 레이블 사용
    void *current_pc;
    asm volatile ("mov %0, pc" : "=r"(current_pc));
    print_string("Function A PC: ");
    print_hex((unsigned int)current_pc);
    
    asm volatile ("nop"); // Breakpoint 2: PC 확인 후
}

void function_b(void) {
    print_string("=== Function B 실행 ===\n");
    asm volatile ("nop"); // Breakpoint 3: function_b의 PC
    
    // 다른 함수 호출로 PC 변화 관찰
    function_a();
    
    asm volatile ("nop"); // Breakpoint 4: function_a 호출 후 복귀
    print_string("Function B로 복귀\n");
}

void memory_layout_demo(void) {
    print_string("=== 메모리 레이아웃 분석 ===\n");
    
    // 지역 변수 (스택)
    int local_var = 0xAABBCCDD;
    int local_array[4] = {1, 2, 3, 4};
    
    asm volatile ("nop"); // Breakpoint 5: 메모리 주소 분석 시작
    
    // 각 메모리 영역의 주소 출력
    print_string("Flash 문자열 주소: ");
    print_hex((unsigned int)flash_string);
    
    print_string("초기화된 전역변수 주소: ");
    print_hex((unsigned int)&initialized_data);
    
    print_string("초기화되지 않은 전역변수 주소: ");
    print_hex((unsigned int)&uninitialized_data);
    
    print_string("지역변수 주소: ");
    print_hex((unsigned int)&local_var);
    
    print_string("지역배열 주소: ");
    print_hex((unsigned int)local_array);
    
    asm volatile ("nop"); // Breakpoint 6: 메모리 주소 분석 완료
}

void pc_jump_demo(void) {
    print_string("=== PC 점프 동작 분석 ===\n");
    
    asm volatile ("nop"); // Breakpoint 7: 점프 전 PC
    
    // 조건부 분기를 통한 PC 변화 관찰
    int condition = 1;
    if (condition) {
        asm volatile ("nop"); // Breakpoint 8: if 브랜치 내부
        print_string("조건이 참: PC가 if 브랜치로 점프\n");
    } else {
        asm volatile ("nop"); // Breakpoint 9: else 브랜치 (실행되지 않음)
        print_string("조건이 거짓: PC가 else 브랜치로 점프\n");
    }
    
    asm volatile ("nop"); // Breakpoint 10: 분기 후 합류점
    
    // 반복문에서의 PC 변화
    print_string("반복문 PC 변화 관찰:\n");
    for (int i = 0; i < 3; i++) {
        asm volatile ("nop"); // Breakpoint 11: 반복문 내부 PC
        print_string("반복 ");
        print_hex(i);
    }
    
    asm volatile ("nop"); // Breakpoint 12: 반복문 종료 후
}

void stack_pc_interaction(void) {
    print_string("=== 스택과 PC 상호작용 ===\n");
    
    // 함수 호출 시 LR(Link Register)에 저장되는 복귀 주소 관찰
    register uint32_t lr_value asm("lr");
    
    asm volatile ("nop"); // Breakpoint 13: LR 값 확인
    print_string("현재 LR 값 (복귀 주소): ");
    print_hex(lr_value);
    
    // 스택에 저장된 복귀 주소들 확인
    uint32_t *sp = (uint32_t*)__builtin_frame_address(0);
    print_string("스택의 복귀 주소들:\n");
    
    for (int i = 0; i < 4; i++) {
        print_string("스택[");
        print_hex(i);
        print_string("]: ");
        print_hex(sp[i]);
    }
    
    asm volatile ("nop"); // Breakpoint 14: 스택 분석 완료
}

void memory_access_patterns(void) {
    print_string("=== 메모리 접근 패턴 분석 ===\n");
    
    // 순차 접근 (Sequential Access)
    int sequential_array[8] = {0};
    asm volatile ("nop"); // Breakpoint 15: 순차 접근 시작
    
    for (int i = 0; i < 8; i++) {
        sequential_array[i] = i * 10;
        asm volatile ("nop"); // Breakpoint 16: 각 원소 접근 시
    }
    
    // 임의 접근 (Random Access)
    asm volatile ("nop"); // Breakpoint 17: 임의 접근 시작
    int indices[] = {7, 2, 5, 0, 3, 1, 6, 4};
    
    for (int i = 0; i < 8; i++) {
        int value = sequential_array[indices[i]];
        asm volatile ("nop"); // Breakpoint 18: 임의 접근 시
        (void)value; // 컴파일러 최적화 방지
    }
    
    asm volatile ("nop"); // Breakpoint 19: 메모리 접근 패턴 완료
}

void instruction_fetch_demo(void) {
    print_string("=== 명령어 인출(Fetch) 분석 ===\n");
    
    // 다양한 타입의 명령어들로 PC 변화 관찰
    
    asm volatile ("nop"); // Breakpoint 20: NOP 명령어
    
    // 산술 명령어
    int a = 10, b = 20, c;
    asm volatile ("nop"); // Breakpoint 21: 산술 연산 전
    c = a + b;
    asm volatile ("nop"); // Breakpoint 22: 산술 연산 후
    
    // 메모리 로드/스토어 명령어
    asm volatile ("nop"); // Breakpoint 23: 메모리 접근 전
    int *ptr = &initialized_data;
    int loaded_value = *ptr;
    asm volatile ("nop"); // Breakpoint 24: 메모리 접근 후
    
    // 분기 명령어
    asm volatile ("nop"); // Breakpoint 25: 분기 전
    if (loaded_value > 0) {
        print_string("로드된 값이 양수입니다\n");
    }
    asm volatile ("nop"); // Breakpoint 26: 분기 후
    
    (void)c; // 컴파일러 경고 방지
}

int main(void) {
    print_string("Cortex-M33 메인메모리 & PC레지스터 Demo\n");
    print_string("=====================================\n");
    
    // 메인 함수의 시작 주소 확인
    print_string("main 함수 주소: ");
    print_hex((unsigned int)main);
    
    asm volatile ("nop"); // Breakpoint 27: main 함수 시작
    
    // 각 데모 함수 실행
    memory_layout_demo();
    function_b();  // function_a도 내부에서 호출됨
    pc_jump_demo();
    stack_pc_interaction();
    memory_access_patterns();
    instruction_fetch_demo();
    
    print_string("모든 메모리 & PC 데모 완료!\n");
    
    // 무한 루프
    while (1) {
        asm volatile ("wfi"); // Wait for interrupt
    }
    
    return 0;
}
