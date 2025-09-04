/*
 * Stack Analysis - 스택 동작, 함수 호출, Stack Overflow 분석
 * 재귀 함수와 깊은 호출 스택을 통한 스택 메모리 이해
 */

// ARM Semihosting
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
    semihost_call(0x04, (void*)str);
}

void exit_program(int code) {
    semihost_call(0x18, &code);
    while(1);
}

// 전역 변수 (DATA/BSS 영역)
int global_depth_counter = 0;
int max_recursion_depth = 0;
int stack_overflow_detected = 0;

// 간단한 출력 함수들
void print_number(int num) {
    char buffer[20];
    int i = 0;
    
    if (num == 0) {
        print_string("0");
        return;
    }
    
    if (num < 0) {
        print_string("-");
        num = -num;
    }
    
    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    while (i > 0) {
        char c[2] = {buffer[--i], '\0'};
        print_string(c);
    }
}

void print_hex(unsigned int value) {
    print_string("0x");
    char hex_chars[] = "0123456789ABCDEF";
    
    for (int i = 7; i >= 0; i--) {
        int digit = (value >> (i * 4)) & 0xF;
        char c[2] = {hex_chars[digit], '\0'};
        print_string(c);
    }
}

// 스택 포인터 읽기
unsigned int get_stack_pointer() {
    unsigned int sp;
    __asm__ volatile ("mov %0, sp" : "=r" (sp));
    return sp;
}

// 현재 스택 프레임 정보 출력
void print_stack_info(const char* function_name, int local_var) {
    print_string("=== Stack Info: ");
    print_string(function_name);
    print_string(" ===\n");
    
    print_string("  Current SP: ");
    print_hex(get_stack_pointer());
    print_string("\n");
    
    print_string("  Local var address: ");
    print_hex((unsigned int)&local_var);
    print_string("\n");
    
    print_string("  Local var value: ");
    print_number(local_var);
    print_string("\n");
    
    print_string("  Global depth: ");
    print_number(global_depth_counter);
    print_string("\n\n");
}

// 단순한 재귀 함수 - 팩토리얼 계산
int factorial_recursive(int n, int depth) {
    int local_result = 1;
    global_depth_counter = depth;
    
    print_string("Factorial(");
    print_number(n);
    print_string(") - Depth: ");
    print_number(depth);
    print_string(", SP: ");
    print_hex(get_stack_pointer());
    print_string("\n");
    
    // 기저 조건
    if (n <= 1) {
        print_string("  Base case reached!\n");
        return 1;
    }
    
    // 재귀 호출
    local_result = n * factorial_recursive(n - 1, depth + 1);
    
    print_string("Returning from depth ");
    print_number(depth);
    print_string(", result: ");
    print_number(local_result);
    print_string("\n");
    
    return local_result;
}

// 피보나치 수열 (비효율적인 재귀) - 스택 사용량 증가
int fibonacci_recursive(int n, int depth) {
    int local_var1 = n;
    int local_var2 = depth;
    
    global_depth_counter++;
    if (global_depth_counter > max_recursion_depth) {
        max_recursion_depth = global_depth_counter;
    }
    
    // 깊이가 너무 깊어지면 스택 오버플로우 방지
    if (depth > 15) {
        print_string("  Depth limit reached to prevent stack overflow!\n");
        global_depth_counter--;
        return 0;
    }
    
    print_stack_info("fibonacci_recursive", local_var1);
    
    if (n <= 1) {
        global_depth_counter--;
        return n;
    }
    
    int result1 = fibonacci_recursive(n - 1, depth + 1);
    int result2 = fibonacci_recursive(n - 2, depth + 1);
    
    global_depth_counter--;
    return result1 + result2;
}

// 깊은 함수 호출 체인
void deep_call_level_5(int value) {
    int local_array[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    print_string("Level 5 - Large local array allocated\n");
    print_string("  Array address: ");
    print_hex((unsigned int)local_array);
    print_string(", SP: ");
    print_hex(get_stack_pointer());
    print_string("\n");
    
    // 배열 사용
    for (int i = 0; i < 10; i++) {
        local_array[i] = value + i;
    }
    
    print_string("  Modified array: ");
    for (int i = 0; i < 5; i++) {
        print_number(local_array[i]);
        print_string(" ");
    }
    print_string("...\n");
}

void deep_call_level_4(int value) {
    char local_buffer[50];
    print_string("Level 4 - String buffer allocated\n");
    print_string("  Buffer address: ");
    print_hex((unsigned int)local_buffer);
    print_string(", SP: ");
    print_hex(get_stack_pointer());
    print_string("\n");
    
    // 버퍼 사용 (간단한 문자열 복사)
    const char* msg = "Hello Stack!";
    for (int i = 0; i < 12 && msg[i]; i++) {
        local_buffer[i] = msg[i];
    }
    local_buffer[12] = '\0';
    
    deep_call_level_5(value + 1);
}

void deep_call_level_3(int value) {
    double local_double = 3.14159;
    print_string("Level 3 - Double variable allocated\n");
    print_string("  Double address: ");
    print_hex((unsigned int)&local_double);
    print_string(", SP: ");
    print_hex(get_stack_pointer());
    print_string("\n");
    
    deep_call_level_4(value + 1);
}

void deep_call_level_2(int value) {
    int local_vars[5] = {10, 20, 30, 40, 50};
    print_string("Level 2 - Integer array allocated\n");
    print_string("  Array address: ");
    print_hex((unsigned int)local_vars);
    print_string(", SP: ");
    print_hex(get_stack_pointer());
    print_string("\n");
    
    deep_call_level_3(value + 1);
}

void deep_call_level_1(int value) {
    int local_int = value * 2;
    print_string("Level 1 - Single integer allocated\n");
    print_string("  Integer address: ");
    print_hex((unsigned int)&local_int);
    print_string(", SP: ");
    print_hex(get_stack_pointer());
    print_string("\n");
    
    deep_call_level_2(local_int);
}

// 스택 오버플로우 시뮬레이션 (위험한 함수 - 제한적으로 실행)
void dangerous_recursive_function(int depth) {
    char large_buffer[200];  // 큰 로컬 버퍼
    int local_vars[20];      // 추가 로컬 변수들
    
    // 버퍼 초기화 (스택 사용)
    for (int i = 0; i < 200; i++) {
        large_buffer[i] = (char)(depth % 256);
    }
    for (int i = 0; i < 20; i++) {
        local_vars[i] = depth * i;
    }
    
    print_string("Dangerous depth: ");
    print_number(depth);
    print_string(", SP: ");
    print_hex(get_stack_pointer());
    print_string("\n");
    
    // 안전 장치: 깊이 제한
    if (depth > 20) {
        print_string("SAFETY LIMIT REACHED - Preventing stack overflow!\n");
        stack_overflow_detected = 1;
        return;
    }
    
    // 현재 스택 포인터가 너무 낮아지면 중단
    unsigned int current_sp = get_stack_pointer();
    unsigned int stack_limit = 0x10070000;  // 안전한 하한선
    
    if (current_sp < stack_limit) {
        print_string("STACK LIMIT REACHED - Aborting!\n");
        stack_overflow_detected = 1;
        return;
    }
    
    dangerous_recursive_function(depth + 1);
}

void main(void) {
    print_string("===============================================\n");
    print_string("Stack Analysis - Function Calls & Stack Growth\n");
    print_string("===============================================\n");
    
    // 초기 스택 상태
    int main_local = 42;
    print_string("\n=== Initial Stack State ===\n");
    print_string("Main function SP: ");
    print_hex(get_stack_pointer());
    print_string("\n");
    print_string("Main local var address: ");
    print_hex((unsigned int)&main_local);
    print_string("\n\n");
    
    // 1. 간단한 재귀 함수 테스트
    print_string("=== Test 1: Simple Factorial Recursion ===\n");
    global_depth_counter = 0;
    int factorial_5 = factorial_recursive(5, 1);
    print_string("Result: 5! = ");
    print_number(factorial_5);
    print_string("\n\n");
    
    // 2. 피보나치 재귀 (더 복잡한 스택 사용)
    print_string("=== Test 2: Fibonacci Recursion ===\n");
    global_depth_counter = 0;
    max_recursion_depth = 0;
    int fib_7 = fibonacci_recursive(7, 1);
    print_string("Result: fib(7) = ");
    print_number(fib_7);
    print_string("\nMax recursion depth reached: ");
    print_number(max_recursion_depth);
    print_string("\n\n");
    
    // 3. 깊은 함수 호출 체인
    print_string("=== Test 3: Deep Function Call Chain ===\n");
    print_string("Starting deep call chain...\n");
    deep_call_level_1(100);
    print_string("Deep call chain completed!\n\n");
    
    // 4. 스택 오버플로우 시뮬레이션 (안전하게)
    print_string("=== Test 4: Stack Overflow Simulation ===\n");
    print_string("WARNING: Testing stack limits safely...\n");
    stack_overflow_detected = 0;
    dangerous_recursive_function(1);
    
    if (stack_overflow_detected) {
        print_string("Stack overflow was safely detected and prevented!\n");
    } else {
        print_string("Function completed without stack overflow.\n");
    }
    
    // 최종 스택 상태
    print_string("\n=== Final Stack State ===\n");
    print_string("Final SP: ");
    print_hex(get_stack_pointer());
    print_string("\n");
    
    print_string("\n===============================================\n");
    print_string("Stack analysis completed!\n");
    print_string("===============================================\n");
    
    exit_program(0);
}