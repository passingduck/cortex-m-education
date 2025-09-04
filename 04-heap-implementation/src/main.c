/*
 * Simple Heap Implementation - Bump Allocator
 * 왜 Stack과 Data만으로는 부족한지, Heap의 필요성과 동작 원리
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

// === 간단한 Bump Allocator Heap 구현 ===

#define HEAP_SIZE 1024  // 1KB 힙 공간

// BSS 영역에 힙 메모리 할당
static char heap_memory[HEAP_SIZE];
static char* heap_current = heap_memory;  // 현재 할당 위치
static int total_allocated = 0;
static int allocation_count = 0;

// 메모리 할당 추적을 위한 구조체
typedef struct {
    void* address;
    int size;
    int allocation_id;
} allocation_info_t;

#define MAX_ALLOCATIONS 20
static allocation_info_t allocations[MAX_ALLOCATIONS];

// 간단한 malloc 구현 (Bump Allocator)
void* simple_malloc(int size) {
    // 8바이트 정렬
    int aligned_size = (size + 7) & ~7;
    
    // 힙 공간 확인
    if (heap_current + aligned_size > heap_memory + HEAP_SIZE) {
        print_string("ERROR: Heap exhausted!\n");
        return 0;  // NULL
    }
    
    void* ptr = heap_current;
    heap_current += aligned_size;
    total_allocated += aligned_size;
    
    // 할당 정보 기록
    if (allocation_count < MAX_ALLOCATIONS) {
        allocations[allocation_count].address = ptr;
        allocations[allocation_count].size = aligned_size;
        allocations[allocation_count].allocation_id = allocation_count + 1;
        allocation_count++;
    }
    
    print_string("  MALLOC: ");
    print_number(size);
    print_string(" bytes (aligned: ");
    print_number(aligned_size);
    print_string(") at ");
    print_hex((unsigned int)ptr);
    print_string("\n");
    
    return ptr;
}

// 간단한 free 구현 (Bump Allocator는 실제로 free하지 않음)
void simple_free(void* ptr) {
    if (!ptr) return;
    
    print_string("  FREE: ");
    print_hex((unsigned int)ptr);
    print_string(" (Note: Bump allocator doesn't actually free)\n");
    
    // Bump allocator는 실제로 메모리를 해제하지 않습니다
    // 실제 구현에서는 더 복잡한 알고리즘이 필요합니다
}

// 힙 상태 출력
void print_heap_status() {
    print_string("\n=== Heap Status ===\n");
    print_string("Heap base: ");
    print_hex((unsigned int)heap_memory);
    print_string("\n");
    print_string("Current position: ");
    print_hex((unsigned int)heap_current);
    print_string("\n");
    print_string("Total allocated: ");
    print_number(total_allocated);
    print_string(" / ");
    print_number(HEAP_SIZE);
    print_string(" bytes\n");
    print_string("Allocations count: ");
    print_number(allocation_count);
    print_string("\n");
    
    print_string("\nAllocation details:\n");
    for (int i = 0; i < allocation_count; i++) {
        print_string("  #");
        print_number(allocations[i].allocation_id);
        print_string(": ");
        print_hex((unsigned int)allocations[i].address);
        print_string(" (");
        print_number(allocations[i].size);
        print_string(" bytes)\n");
    }
    print_string("\n");
}

// 동적 배열 예제
typedef struct {
    int* data;
    int size;
    int capacity;
} dynamic_array_t;

dynamic_array_t* create_dynamic_array(int initial_capacity) {
    print_string("Creating dynamic array with capacity ");
    print_number(initial_capacity);
    print_string("\n");
    
    // 배열 구조체 할당
    dynamic_array_t* arr = (dynamic_array_t*)simple_malloc(sizeof(dynamic_array_t));
    if (!arr) return 0;
    
    // 데이터 배열 할당
    arr->data = (int*)simple_malloc(initial_capacity * sizeof(int));
    if (!arr->data) {
        simple_free(arr);
        return 0;
    }
    
    arr->size = 0;
    arr->capacity = initial_capacity;
    
    return arr;
}

void add_to_array(dynamic_array_t* arr, int value) {
    if (arr->size >= arr->capacity) {
        print_string("  Array capacity exceeded, need to resize!\n");
        // 실제 구현에서는 realloc을 사용하여 크기를 늘림
        return;
    }
    
    arr->data[arr->size] = value;
    arr->size++;
    
    print_string("  Added ");
    print_number(value);
    print_string(" to array (size now: ");
    print_number(arr->size);
    print_string(")\n");
}

void print_array(dynamic_array_t* arr) {
    print_string("  Array contents: [");
    for (int i = 0; i < arr->size; i++) {
        print_number(arr->data[i]);
        if (i < arr->size - 1) print_string(", ");
    }
    print_string("]\n");
}

void destroy_dynamic_array(dynamic_array_t* arr) {
    if (arr) {
        simple_free(arr->data);
        simple_free(arr);
        print_string("Dynamic array destroyed\n");
    }
}

// 연결 리스트 예제
typedef struct list_node {
    int data;
    struct list_node* next;
} list_node_t;

typedef struct {
    list_node_t* head;
    int size;
} linked_list_t;

linked_list_t* create_linked_list() {
    print_string("Creating linked list\n");
    linked_list_t* list = (linked_list_t*)simple_malloc(sizeof(linked_list_t));
    if (list) {
        list->head = 0;  // NULL
        list->size = 0;
    }
    return list;
}

void list_add(linked_list_t* list, int value) {
    print_string("  Adding node with value ");
    print_number(value);
    print_string("\n");
    
    list_node_t* new_node = (list_node_t*)simple_malloc(sizeof(list_node_t));
    if (!new_node) return;
    
    new_node->data = value;
    new_node->next = list->head;
    list->head = new_node;
    list->size++;
}

void print_list(linked_list_t* list) {
    print_string("  List contents: ");
    list_node_t* current = list->head;
    while (current) {
        print_number(current->data);
        if (current->next) print_string(" -> ");
        current = current->next;
    }
    print_string(" (size: ");
    print_number(list->size);
    print_string(")\n");
}

void destroy_linked_list(linked_list_t* list) {
    if (!list) return;
    
    list_node_t* current = list->head;
    while (current) {
        list_node_t* next = current->next;
        simple_free(current);
        current = next;
    }
    simple_free(list);
    print_string("Linked list destroyed\n");
}

// 가변 크기 문자열 예제
char* create_string(const char* source) {
    int len = 0;
    
    // 문자열 길이 계산
    while (source[len]) len++;
    
    print_string("Creating string copy of length ");
    print_number(len);
    print_string("\n");
    
    char* new_str = (char*)simple_malloc(len + 1);
    if (!new_str) return 0;
    
    // 문자열 복사
    for (int i = 0; i <= len; i++) {
        new_str[i] = source[i];
    }
    
    return new_str;
}

void main(void) {
    print_string("===============================================\n");
    print_string("Simple Heap Implementation - Why We Need Heap\n");
    print_string("===============================================\n");
    
    // 초기 힙 상태
    print_heap_status();
    
    // 1. 간단한 메모리 할당 테스트
    print_string("=== Test 1: Basic Memory Allocation ===\n");
    void* ptr1 = simple_malloc(16);
    void* ptr2 = simple_malloc(32);
    void* ptr3 = simple_malloc(64);
    
    print_heap_status();
    
    // 2. 동적 배열 테스트
    print_string("=== Test 2: Dynamic Array ===\n");
    dynamic_array_t* arr = create_dynamic_array(5);
    
    if (arr) {
        for (int i = 1; i <= 4; i++) {
            add_to_array(arr, i * 10);
        }
        print_array(arr);
        
        // 용량 초과 시도
        add_to_array(arr, 50);
        add_to_array(arr, 60);  // 이것은 실패해야 함
        
        print_heap_status();
    }
    
    // 3. 연결 리스트 테스트
    print_string("=== Test 3: Linked List ===\n");
    linked_list_t* list = create_linked_list();
    
    if (list) {
        for (int i = 100; i <= 130; i += 10) {
            list_add(list, i);
        }
        print_list(list);
        
        print_heap_status();
    }
    
    // 4. 가변 크기 문자열 테스트
    print_string("=== Test 4: Dynamic Strings ===\n");
    char* str1 = create_string("Hello");
    char* str2 = create_string("Dynamic Memory");
    char* str3 = create_string("Allocation Example");
    
    if (str1 && str2 && str3) {
        print_string("Created strings:\n");
        print_string("  str1: ");
        print_string(str1);
        print_string("\n");
        print_string("  str2: ");
        print_string(str2);
        print_string("\n");
        print_string("  str3: ");
        print_string(str3);
        print_string("\n");
    }
    
    print_heap_status();
    
    // 5. 메모리 해제 테스트 (Bump allocator 한계 보여주기)
    print_string("=== Test 5: Memory Deallocation ===\n");
    print_string("Attempting to free allocated memory...\n");
    simple_free(ptr1);
    simple_free(ptr2);
    simple_free(ptr3);
    destroy_dynamic_array(arr);
    destroy_linked_list(list);
    simple_free(str1);
    simple_free(str2);
    simple_free(str3);
    
    print_heap_status();
    print_string("Note: Bump allocator doesn't reclaim memory!\n");
    
    // 6. 힙 고갈 테스트
    print_string("=== Test 6: Heap Exhaustion ===\n");
    print_string("Attempting to allocate large chunks until heap is exhausted...\n");
    
    int chunk_size = 100;
    int chunks_allocated = 0;
    
    while (1) {
        void* chunk = simple_malloc(chunk_size);
        if (!chunk) {
            print_string("Heap exhausted after ");
            print_number(chunks_allocated);
            print_string(" chunks of ");
            print_number(chunk_size);
            print_string(" bytes\n");
            break;
        }
        chunks_allocated++;
    }
    
    print_heap_status();
    
    // 최종 분석
    print_string("\n=== Analysis: Why We Need Heap ===\n");
    print_string("1. Stack: Limited size, automatic management, LIFO order\n");
    print_string("2. Data/BSS: Fixed size, compile-time allocation\n");
    print_string("3. Heap: Dynamic size, runtime allocation, flexible\n");
    print_string("\nHeap enables:\n");
    print_string("- Variable-sized data structures\n");
    print_string("- Memory allocation based on runtime decisions\n");
    print_string("- Data that outlives the function that created it\n");
    print_string("- Complex data structures (trees, graphs, etc.)\n");
    
    print_string("\n===============================================\n");
    print_string("Heap implementation analysis completed!\n");
    print_string("===============================================\n");
    
    exit_program(0);
}