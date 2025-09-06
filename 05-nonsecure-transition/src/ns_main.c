/*
 * Non-secure Application for TrustZone Example
 * This code runs in Non-secure state with code/stack in NS RAM
 */

/* 
 * Use GCC pragmas to place all symbols in Non-secure sections
 * This ensures proper memory layout in the linker script
 */
#pragma GCC push_options
#pragma GCC section text=".text_ns"
#pragma GCC section rodata=".rodata_ns" 
#pragma GCC section data=".data_ns"
#pragma GCC section bss=".bss_ns"

/*
 * ARM Semihosting implementation
 * Same interface as other examples but in Non-secure context
 */
static inline int semihost_call(int reason, void* arg) {
    int result;
    __asm__ volatile (
        "mov r0, %1\n"
        "mov r1, %2\n"
        "bkpt #0xAB\n"    /* ARM semihosting breakpoint */
        "mov %0, r0\n"
        : "=r" (result)
        : "r" (reason), "r" (arg)
        : "r0", "r1", "memory"
    );
    return result;
}

static inline void print_string(const char* str) {
    semihost_call(0x04, (void*)str);  /* SYS_WRITE0 */
}

static inline void print_hex(unsigned int value) {
    char hex_str[11] = "0x";
    char hex_chars[] = "0123456789ABCDEF";
    
    for (int i = 7; i >= 0; i--) {
        hex_str[2 + (7 - i)] = hex_chars[(value >> (i * 4)) & 0xF];
    }
    hex_str[10] = '\0';
    
    print_string(hex_str);
}

static inline void exit_program(int code) {
    semihost_call(0x18, &code);  /* SYS_EXIT */
    while(1);
}

/*
 * Get current stack pointer
 */
static inline unsigned int get_sp(void) {
    unsigned int sp;
    __asm__ volatile ("mov %0, sp" : "=r" (sp));
    return sp;
}

/*
 * Get current PC (approximate)
 */
static inline unsigned int get_pc(void) {
    unsigned int pc;
    __asm__ volatile ("mov %0, pc" : "=r" (pc));
    return pc;
}

/*
 * Demo functions to show Non-secure execution
 */
static void demonstrate_memory_layout(void) {
    print_string("\n=== Non-secure Memory Layout Analysis ===\n");
    
    /* Show current execution context */
    print_string("Current SP (Non-secure): ");
    print_hex(get_sp());
    print_string("\n");
    
    print_string("Current PC (Non-secure): ");
    print_hex(get_pc());
    print_string("\n");
    
    /* Show variable addresses */
    static int ns_static_var = 0x12345678;
    int ns_local_var = 0x87654321;
    
    print_string("NS static variable addr: ");
    print_hex((unsigned int)&ns_static_var);
    print_string(" (value: ");
    print_hex(ns_static_var);
    print_string(")\n");
    
    print_string("NS local variable addr:  ");
    print_hex((unsigned int)&ns_local_var);
    print_string(" (value: ");
    print_hex(ns_local_var);
    print_string(")\n");
    
    print_string("NS function addr:        ");
    print_hex((unsigned int)demonstrate_memory_layout);
    print_string("\n");
}

static void demonstrate_stack_usage(void) {
    print_string("\n=== Non-secure Stack Demonstration ===\n");
    
    unsigned int initial_sp = get_sp();
    print_string("Function entry SP: ");
    print_hex(initial_sp);
    print_string("\n");
    
    /* Allocate some local variables */
    char buffer[64];
    int numbers[16];
    
    /* Initialize to prevent optimization */
    for (int i = 0; i < 64; i++) buffer[i] = i;
    for (int i = 0; i < 16; i++) numbers[i] = i * i;
    
    print_string("After locals SP:   ");
    print_hex(get_sp());
    print_string("\n");
    
    print_string("Buffer address:    ");
    print_hex((unsigned int)buffer);
    print_string("\n");
    
    print_string("Numbers address:   ");
    print_hex((unsigned int)numbers);
    print_string("\n");
    
    print_string("Stack usage:       ");
    print_hex(initial_sp - get_sp());
    print_string(" bytes\n");
    
    /* Use the variables to prevent optimization */
    print_string("Buffer[10]:        ");
    print_hex(buffer[10]);
    print_string("\n");
    
    print_string("Numbers[5]:        ");
    print_hex(numbers[5]);
    print_string("\n");
}

/*
 * Recursive function to show Non-secure stack growth
 */
static void recursive_demo(int depth) {
    if (depth <= 0) {
        print_string("Max recursion reached in NS\n");
        return;
    }
    
    if (depth % 2 == 0) {
        print_string("NS recursion depth: ");
        char digit = '0' + depth;
        char depth_str[3] = {digit, '\n', '\0'};
        print_string(depth_str);
        
        print_string("  SP at depth ");
        char digit2 = '0' + depth;
        char depth_str2[2] = {digit2, '\0'};
        print_string(depth_str2);
        print_string(": ");
        print_hex(get_sp());
        print_string("\n");
    }
    
    /* Local variable in each recursion level */
    volatile int local_data = depth * 0x1000 + 0xABCD;
    
    recursive_demo(depth - 1);
    
    /* Prevent optimization */
    (void)local_data;
}

/*
 * Main Non-secure application
 */
void ns_main(void) {
    print_string("================================================\n");
    print_string("TrustZone Non-secure Application\n");
    print_string("================================================\n");
    print_string("Running in Non-secure state!\n");
    print_string("Code in NS CODE region @ 0x00000000\n");
    print_string("Stack in NS RAM region @ 0x20000000\n\n");
    
    /* Basic functionality test */
    print_string("=== Basic Non-secure Functionality ===\n");
    for (int i = 0; i < 5; i++) {
        print_string("NS Count: ");
        char num = '0' + i;
        char num_str[3] = {num, '\n', '\0'};
        print_string(num_str);
    }
    
    /* Memory layout analysis */
    demonstrate_memory_layout();
    
    /* Stack usage demonstration */
    demonstrate_stack_usage();
    
    /* Recursion demonstration */
    print_string("\n=== Non-secure Recursion Test ===\n");
    recursive_demo(8);
    
    print_string("\n=== TrustZone Transition Success! ===\n");
    print_string("Successfully transitioned from Secure to Non-secure\n");
    print_string("Non-secure application completed successfully\n");
    print_string("Code and stack running in Non-secure RAM regions\n");
    
    /* Exit gracefully */
    print_string("\nExiting Non-secure application...\n");
    exit_program(0);
}

#pragma GCC pop_options
