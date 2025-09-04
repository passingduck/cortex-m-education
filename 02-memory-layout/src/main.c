/*
 * Memory Layout Analysis - Text, Data, BSS 영역 이해
 * x^16 함수를 통한 데이터 영역과 포인터 학습
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

// === DATA 영역에 저장되는 초기화된 전역 변수들 ===
int base_value = 2;           // DATA 영역: 초기값이 있는 전역 변수
int exponent = 16;           // DATA 영역: 초기값이 있는 전역 변수
const char* message = "Computing x^16 with memory analysis\n";  // DATA 영역: 문자열 포인터

// === BSS 영역에 저장되는 초기화되지 않은 전역 변수들 ===
int result_array[10];        // BSS 영역: 초기화되지 않은 배열
int calculation_count;       // BSS 영역: 초기화되지 않은 변수
int* dynamic_pointer;        // BSS 영역: 포인터 변수

// === TEXT 영역에 저장되는 상수 데이터 ===
const int CONSTANT_TABLE[5] = {1, 2, 4, 8, 16};  // RODATA/TEXT 영역

// 간단한 출력 함수
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
    
    // 숫자를 문자열로 변환
    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    // 역순으로 출력
    while (i > 0) {
        char c[2] = {buffer[--i], '\0'};
        print_string(c);
    }
}

// x^16을 계산하는 함수 (반복적으로)
int power_of_16_iterative(int x) {
    int result = 1;
    for (int i = 0; i < 16; i++) {
        result *= x;
    }
    return result;
}

// x^16을 계산하는 함수 (재귀적으로 - 스택 사용량 증가)
int power_of_16_recursive(int x, int exp) {
    if (exp == 0) return 1;
    if (exp == 1) return x;
    
    int half = power_of_16_recursive(x, exp / 2);
    if (exp % 2 == 0) {
        return half * half;
    } else {
        return x * half * half;
    }
}

// 메모리 영역 분석 함수
void analyze_memory_regions() {
    print_string("\n=== Memory Regions Analysis ===\n");
    
    // 지역 변수 (스택 영역)
    int local_var = 42;
    
    print_string("1. Variable Addresses:\n");
    
    print_string("   Global DATA (initialized): base_value = ");
    print_number((int)&base_value);
    print_string("\n");
    
    print_string("   Global BSS (uninitialized): calculation_count = ");
    print_number((int)&calculation_count);
    print_string("\n");
    
    print_string("   Local STACK variable: local_var = ");
    print_number((int)&local_var);
    print_string("\n");
    
    print_string("   RODATA constant table: CONSTANT_TABLE = ");
    print_number((int)&CONSTANT_TABLE);
    print_string("\n");
    
    print_string("   Function address (TEXT): power_of_16_iterative = ");
    print_number((int)power_of_16_iterative);
    print_string("\n\n");
}

// 포인터와 데이터 영역 실험
void pointer_experiments() {
    print_string("=== Pointer and Data Experiments ===\n");
    
    // 1. 전역 변수 포인터 실험
    int* ptr_to_base = &base_value;
    print_string("1. Pointer to global DATA variable:\n");
    print_string("   base_value = ");
    print_number(base_value);
    print_string(" (address: ");
    print_number((int)&base_value);
    print_string(")\n");
    
    print_string("   *ptr_to_base = ");
    print_number(*ptr_to_base);
    print_string(" (pointer address: ");
    print_number((int)ptr_to_base);
    print_string(")\n");
    
    // 2. 포인터를 통한 값 변경
    *ptr_to_base = 3;  // base_value를 2에서 3으로 변경
    print_string("   After modification: base_value = ");
    print_number(base_value);
    print_string("\n\n");
    
    // 3. 배열과 포인터
    result_array[0] = 100;
    result_array[1] = 200;
    dynamic_pointer = result_array;  // 배열의 첫 번째 요소를 가리킴
    
    print_string("2. Array and pointer relationship:\n");
    print_string("   result_array[0] = ");
    print_number(result_array[0]);
    print_string("\n");
    print_string("   *dynamic_pointer = ");
    print_number(*dynamic_pointer);
    print_string("\n");
    print_string("   *(dynamic_pointer + 1) = ");
    print_number(*(dynamic_pointer + 1));
    print_string("\n\n");
}

void main(void) {
    print_string("==========================================\n");
    print_string("Memory Layout Analysis - Text/Data/BSS\n");
    print_string("==========================================\n");
    print_string(message);
    
    // 초기화되지 않은 전역 변수는 0으로 초기화됨 (BSS 영역의 특성)
    calculation_count = 0;
    
    // 메모리 영역 분석
    analyze_memory_regions();
    
    // 포인터 실험
    pointer_experiments();
    
    // x^16 계산 (반복적)
    print_string("=== Computing x^16 ===\n");
    for (int x = 1; x <= 3; x++) {
        int result_iter = power_of_16_iterative(x);
        int result_rec = power_of_16_recursive(x, 16);
        
        print_string("x = ");
        print_number(x);
        print_string(": ");
        print_number(x);
        print_string("^16 = ");
        print_number(result_iter);
        print_string(" (iterative) = ");
        print_number(result_rec);
        print_string(" (recursive)\n");
        
        // 결과를 배열에 저장 (BSS 영역 사용)
        if (x <= 3) {
            result_array[x-1] = result_iter;
        }
        calculation_count++;
    }
    
    print_string("\n=== Results stored in BSS array ===\n");
    for (int i = 0; i < 3; i++) {
        print_string("result_array[");
        print_number(i);
        print_string("] = ");
        print_number(result_array[i]);
        print_string("\n");
    }
    
    print_string("\nTotal calculations: ");
    print_number(calculation_count);
    print_string("\n");
    
    print_string("\n=== Constant Table (RODATA) ===\n");
    for (int i = 0; i < 5; i++) {
        print_string("CONSTANT_TABLE[");
        print_number(i);
        print_string("] = ");
        print_number(CONSTANT_TABLE[i]);
        print_string("\n");
    }
    
    print_string("\n==========================================\n");
    print_string("Memory layout analysis completed!\n");
    print_string("==========================================\n");
    
    exit_program(0);
}