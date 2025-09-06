# 05. Non-secure 메모리 시뮬레이션 - NS RAM에서의 코드/스택 실행

## 🎯 학습 목표

- **Non-secure 메모리 레이아웃 이해**: NS CODE와 NS RAM 영역 구분
- **메모리 영역별 코드 배치**: 코드를 NS CODE에, 데이터/스택을 NS RAM에 배치
- **스택과 힙 분리**: 메모리 영역의 명확한 구분을 통한 안전한 메모리 관리
- **실제 멀티 메모리 시스템 이해**: 임베디드 시스템에서 자주 사용되는 메모리 구성
- **QEMU 환경에서의 메모리 매핑**: 가상 환경에서의 메모리 레이아웃 실습

## 📋 개요

이 예제는 Non-secure 메모리 영역에서 코드와 스택을 실행하는 시뮬레이션을 보여줍니다. 실제 임베디드 시스템에서 자주 사용되는 멀티 메모리 구성을 교육 목적으로 구현하여, 코드는 Flash 영역(NS_CODE)에, 데이터와 스택은 RAM 영역(NS_RAM)에 배치하는 방법을 학습합니다.

### 메모리 레이아웃

```
0x10000000 - 0x1007FFFF : S_CODE_BOOT (512KB) - Secure 영역
  ├─ Secure Vector Table (0x10000000)
  ├─ Secure Boot Code (SAU 설정, 전환 로직)
  └─ Secure Stack (사용 최소화)

0x00000000 - 0x0007FFFF : NS_CODE (512KB) - Non-secure 코드 영역  
  ├─ Non-secure Vector Table (0x00000000)
  └─ Non-secure Application Code

0x20000000 - 0x2007FFFF : NS_RAM (512KB) - Non-secure 데이터/스택 영역
  ├─ Non-secure Data/BSS
  ├─ Non-secure Heap
  └─ Non-secure Stack (0x2007FFFF부터 아래로)
```

## 🔧 빌드 및 실행

### 환경 설정
```bash
# 개발 환경 확인 및 설정
./scripts/setup.sh

# 필요한 도구들
sudo apt-get install gcc-arm-none-eabi qemu-system-arm gdb-multiarch
```

### 빌드
```bash
# 프로젝트 빌드
make

# 정리 후 다시 빌드
make clean && make

# 빌드 정보 확인
make info
```

### 실행
```bash
# 간단 실행
make run

# 스크립트를 통한 실행 (설명 포함)
./scripts/run.sh

# QEMU 직접 실행
qemu-system-arm -machine mps2-an505 -cpu cortex-m33 \
    -kernel build/trustzone-nonsecure.elf \
    -nographic -semihosting
```

## 🐛 GDB로 디버깅하기

### 기본 디버깅
```bash
# GDB 디버깅 시작
make debug

# 또는 헬퍼 스크립트 사용
./scripts/debug.sh
```

### 주요 GDB 명령어

#### 브레이크포인트 설정
```gdb
# Secure 부트 진입점
(gdb) break Reset_Handler

# Non-secure 전환 지점  
(gdb) break Reset_Handler_NS

# Non-secure 메인 애플리케이션
(gdb) break ns_main

# SAU 설정 완료 후 (대략적 위치)
(gdb) break *0x10000050
```

#### TrustZone 상태 확인
```gdb
# 현재 보안 상태 확인 (CONTROL 레지스터)
(gdb) info registers control

# SAU 레지스터 확인
(gdb) x/4xw 0xE000EDD0    # SAU_RNR, RBAR, RLAR, CTRL

# Secure/Non-secure 벡터 테이블 확인  
(gdb) x/8xw 0x10000000    # Secure vectors
(gdb) x/8xw 0x00000000    # Non-secure vectors

# 스택 포인터 상태
(gdb) print $msp          # Secure MSP
(gdb) info registers msp_ns # Non-secure MSP (GDB 버전에 따라)
```

#### 단계별 실행 추적
```gdb
# Secure 부트 과정 추적
(gdb) stepi               # 명령어 단위 실행
(gdb) continue            # 다음 브레이크포인트까지

# Non-secure 전환 확인
(gdb) info registers pc sp lr
(gdb) disassemble $pc,+32

# 메모리 내용 확인
(gdb) x/16xw $sp          # 스택 내용
(gdb) x/16i $pc           # 현재 코드
```

## 🔍 주요 구성 요소 분석

### 1. Secure 부트 코드 (`src/boot_secure.s`)

#### SAU 설정
```asm
/* SAU Region 0: NS CODE 0x00000000 - 0x0007FFFF */
ldr r0, =SAU_RNR
movs r1, #0                  /* Region number 0 */
str r1, [r0]
ldr r1, =NS_CODE_BASE
str r1, [r0, #4]            /* SAU_RBAR */
ldr r1, =NS_CODE_LIMIT
orr r1, r1, #1              /* RLAR.ENABLE = 1 */
str r1, [r0, #8]            /* SAU_RLAR */
```

**🔍 레지스터 분석**: SAU 설정 시 레지스터 변화 확인
```gdb
(gdb) x/w 0xE000EDD0     # SAU_RNR = 0
(gdb) x/w 0xE000EDD4     # SAU_RBAR = 0x00000000  
(gdb) x/w 0xE000EDD8     # SAU_RLAR = 0x0007FFFF | 1
```

#### Non-secure 전환
```asm
/* Set VTOR_NS to Non-secure vector table */
ldr r0, =SCB_NS_VTOR
ldr r1, =NS_CODE_BASE
str r1, [r0]

/* Load Non-secure MSP from vector table */
ldr r0, =NS_CODE_BASE
ldr r1, [r0]                /* Load initial MSP_NS */
msr MSP_NS, r1

/* Load Non-secure Reset Handler */
ldr r2, [r0, #4]            /* Load Reset_Handler_NS address */

/* Transition to Non-secure state */
bxns r2
```

**🔍 레지스터 분석**: 전환 과정에서 중요한 레지스터들
- `MSP_NS`: Non-secure 스택 포인터 (0x20080000)
- `VTOR_NS`: Non-secure 벡터 테이블 (0x00000000)
- `PC`: 전환 후 Non-secure 코드 주소

### 2. Non-secure 애플리케이션 (`src/ns_main.c`)

#### 메모리 레이아웃 분석 함수
```c
static void demonstrate_memory_layout(void) {
    print_string("Current SP (Non-secure): ");
    print_hex(get_sp());     // ~0x20070000 범위
    
    print_string("Current PC (Non-secure): ");  
    print_hex(get_pc());     // ~0x00000xxx 범위
}
```

**🔍 실행 중 분석**: 
```gdb
(gdb) break demonstrate_memory_layout
(gdb) continue
(gdb) print $sp              # Non-secure 스택 위치 확인
(gdb) print $pc              # Non-secure 코드 위치 확인
(gdb) info frame             # 프레임 정보
```

#### 스택 사용량 데모
```c
static void demonstrate_stack_usage(void) {
    char buffer[64];         // 지역 변수들이 NS 스택에 할당
    int numbers[16];
    
    print_string("Stack usage: ");
    print_hex(initial_sp - get_sp());  // 사용된 스택 크기
}
```

**🔍 스택 분석**:
```gdb
(gdb) break demonstrate_stack_usage
(gdb) continue
(gdb) info locals           # 지역 변수 위치 확인
(gdb) x/32xw $sp           # 스택 내용 덤프
(gdb) print &buffer        # buffer 주소 (NS RAM 영역)
(gdb) print &numbers       # numbers 주소 (NS RAM 영역)
```

## 📊 예상 실행 결과

```
================================================
TrustZone Non-secure Application  
================================================
Running in Non-secure state!
Code in NS CODE region @ 0x00000000
Stack in NS RAM region @ 0x20000000

=== Basic Non-secure Functionality ===
NS Count: 0
NS Count: 1  
NS Count: 2
NS Count: 3
NS Count: 4

=== Non-secure Memory Layout Analysis ===
Current SP (Non-secure): 0x2007FF80
Current PC (Non-secure): 0x000001A4
NS static variable addr: 0x20000000 (value: 0x12345678)
NS local variable addr:  0x2007FF7C (value: 0x87654321)
NS function addr:        0x00000180

=== Non-secure Stack Demonstration ===
Function entry SP: 0x2007FF88
After locals SP:   0x2007FF38
Buffer address:    0x2007FF40
Numbers address:   0x2007FF00
Stack usage:       0x00000050 bytes
Buffer[10]:        0x0000000A
Numbers[5]:        0x00000019

=== Non-secure Recursion Test ===
NS recursion depth: 8
  SP at depth 8: 0x2007FF68
NS recursion depth: 6  
  SP at depth 6: 0x2007FF48
NS recursion depth: 4
  SP at depth 4: 0x2007FF28
NS recursion depth: 2
  SP at depth 2: 0x2007FF08
Max recursion reached in NS

=== TrustZone Transition Success! ===
Successfully transitioned from Secure to Non-secure
Non-secure application completed successfully
Code and stack running in Non-secure RAM regions

Exiting Non-secure application...
```

## 🎓 핵심 개념 정리

### TrustZone 아키텍처
- **Secure World**: 보안이 중요한 코드/데이터가 실행되는 환경
- **Non-secure World**: 일반 애플리케이션이 실행되는 환경  
- **SAU**: 메모리 영역의 보안 속성을 정의하는 하드웨어 유닛
- **BXNS**: Branch and Exchange Non-secure (보안 상태 전환 명령어)

### 메모리 보안 속성
- **Secure 메모리**: Secure 상태에서만 접근 가능
- **Non-secure 메모리**: 양쪽 상태에서 모두 접근 가능
- **NSC**: Non-secure Callable (Secure에서 NS로 호출 허용)

### 실무 활용 시나리오
- **보안 부트로더**: 인증서 검증 후 메인 애플리케이션 실행
- **보안 서비스**: 암호화, 키 관리 등을 Secure에서 제공
- **애플리케이션 격리**: 중요 기능과 일반 기능의 격리 실행

## 🔧 고급 활용

### 추가 SAU 영역 설정
SAU는 최대 8개 영역을 지원합니다. 더 세밀한 메모리 보호를 위해 추가 영역을 설정할 수 있습니다.

### NSC (Non-secure Callable) 구현
Secure 함수를 Non-secure에서 호출할 수 있는 게이트웨이를 구현할 수 있습니다.

### 보안 인터럽트 처리  
Secure 전용 인터럽트와 Non-secure 인터럽트를 분리하여 처리할 수 있습니다.

## 🚨 주의사항

1. **SAU 설정 오류**: 잘못된 메모리 영역 설정시 HardFault 발생
2. **스택 포인터 설정**: MSP_NS가 올바르게 설정되지 않으면 즉시 오류
3. **벡터 테이블**: VTOR_NS 설정 누락시 Non-secure 인터럽트 처리 불가
4. **QEMU 한계**: 실제 하드웨어와 일부 TrustZone 동작이 다를 수 있음

## 📚 참고 자료

- [ARM Cortex-M33 Technical Reference Manual](https://developer.arm.com/documentation/100230/latest/)
- [ARMv8-M Architecture Reference Manual](https://developer.arm.com/documentation/ddi0553/latest/)
- [TrustZone for Cortex-M User Guide](https://developer.arm.com/documentation/100690/latest/)
- [QEMU MPS2-AN505 Documentation](https://qemu.readthedocs.io/en/latest/system/arm/mps2.html)

---

*GDB와 함께하는 임베디드 보안 시스템 마스터 여정을 시작하세요!* 🔐
