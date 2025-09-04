# Cortex-M Education Project 🎓

**SungDB MCP를 활용한 Cortex-M 임베디드 시스템 핵심 개념 학습**

이 프로젝트는 Cortex-M33 마이크로컨트롤러에서 실행되는 실습을 통해 임베디드 시스템의 핵심 개념들을 이해할 수 있도록 구성된 교육 자료입니다. QEMU 에뮬레이터와 SungDB MCP를 활용하여 실제 디버깅 환경에서 학습할 수 있습니다.

## 🎯 학습 목표

- Cortex-M 아키텍처의 메모리 구조 이해
- 함수 호출과 스택의 동작 원리
- 동적 메모리 할당의 필요성과 힙의 구현
- 실제 디버깅 도구를 활용한 실습 경험

## 📚 교육 모듈

### [01. Main 함수 실행 과정 분석](./01-main-execution/)
**주제**: 부팅 과정과 main 함수 진입까지의 과정

- **학습 내용**:
  - Cortex-M33 부팅 시퀀스
  - 벡터 테이블과 리셋 핸들러
  - C 런타임 초기화 과정
  - 세미호스팅을 통한 디버깅

- **실습 환경**:
  ```bash
  cd 01-main-execution
  make && qemu-system-arm -machine mps2-an505 -cpu cortex-m33 -kernel build/cortex-m33-hello-world.elf -nographic -serial stdio -semihosting
  ```

- **SungDB MCP 활용**:
  - GDB 세션 관리
  - 브레이크포인트 설정
  - 단계별 실행 추적
  - 레지스터 및 메모리 상태 확인

### [02. 메모리 레이아웃 분석](./02-memory-layout/)
**주제**: TEXT, DATA, BSS 영역과 x^16 함수 구현

- **학습 내용**:
  - 메모리 영역별 특성과 용도
  - 전역 변수 vs 지역 변수
  - 포인터와 메모리 주소
  - 컴파일러의 메모리 배치 전략

- **핵심 예제**:
  ```c
  // DATA 영역: 초기화된 전역 변수
  int base_value = 2;
  
  // BSS 영역: 초기화되지 않은 전역 변수  
  int result_array[10];
  
  // TEXT 영역: 함수 코드
  int power_of_16_iterative(int x) { ... }
  ```

- **실습 결과**:
  ```
  text    data     bss     dec     hex filename
  2612      12      48    2672     a70 cortex-m33-hello-world.elf
  ```

### [03. 스택 분석](./03-stack-analysis/)
**주제**: 함수 호출, 재귀, 스택 오버플로우

- **학습 내용**:
  - 스택 프레임 구조
  - 함수 호출 메커니즘
  - 재귀 함수와 스택 사용량
  - 스택 오버플로우 방지

- **핵심 예제**:
  ```c
  // 재귀 함수의 스택 사용량 추적
  int factorial_recursive(int n, int depth);
  int fibonacci_recursive(int n, int depth);
  
  // 스택 포인터 모니터링
  unsigned int get_stack_pointer() {
      unsigned int sp;
      __asm__ volatile ("mov %0, sp" : "=r" (sp));
      return sp;
  }
  ```

- **관찰 결과**:
  - 함수 호출 시 스택 포인터 변화 (0x1007FFB8 → 0x1007FF40)
  - 재귀 깊이에 따른 메모리 사용량 증가
  - 안전한 스택 오버플로우 탐지

### [04. 힙 구현](./04-heap-implementation/)
**주제**: 동적 메모리 할당과 Bump Allocator

- **학습 내용**:
  - 힙의 필요성과 장점
  - 간단한 Bump Allocator 구현
  - 동적 데이터 구조 (배열, 연결 리스트)
  - 메모리 단편화와 관리 전략

- **핵심 구현**:
  ```c
  // 간단한 malloc 구현
  void* simple_malloc(int size) {
      int aligned_size = (size + 7) & ~7;  // 8바이트 정렬
      
      if (heap_current + aligned_size > heap_memory + HEAP_SIZE) {
          return NULL;  // 힙 고갈
      }
      
      void* ptr = heap_current;
      heap_current += aligned_size;
      return ptr;
  }
  ```

- **실습 결과**:
  - 1KB 힙 공간에서 968바이트 할당 성공
  - 동적 배열, 연결 리스트, 문자열 생성
  - Bump Allocator의 한계 (메모리 해제 불가) 확인

## 🛠️ 개발 환경 설정

### 필수 도구 설치
```bash
# ARM 툴체인 및 QEMU 설치
sudo apt update
sudo apt install -y gcc-arm-none-eabi gdb-multiarch qemu-system-arm

# SungDB MCP 서버 설정
cd ~/sungdb-mcp
./start_server.sh --http  # 디버깅용 HTTP 모드
```

### 프로젝트 구조
```
cortex-m-education/
├── 01-main-execution/          # 부팅 과정 분석
│   ├── src/main.c             # Hello World + 부팅 추적
│   └── README.md              # 상세 학습 가이드
├── 02-memory-layout/          # 메모리 영역 분석  
│   ├── src/main.c             # x^16 + 메모리 분석
│   └── README.md              # 메모리 맵 학습
├── 03-stack-analysis/         # 스택 동작 분석
│   ├── src/main.c             # 재귀 + 스택 추적
│   └── README.md              # 스택 오버플로우 학습
├── 04-heap-implementation/    # 힙 구현 및 분석
│   ├── src/main.c             # Bump Allocator 구현
│   └── README.md              # 동적 할당 학습
└── README.md                  # 이 파일
```

## 🚀 빠른 시작

### 1. 전체 프로젝트 실행
```bash
# 각 디렉토리에서 빌드 및 실행
for dir in 01-main-execution 02-memory-layout 03-stack-analysis 04-heap-implementation; do
    echo "=== Testing $dir ==="
    cd $dir
    make clean && make
    timeout 10 qemu-system-arm -machine mps2-an505 -cpu cortex-m33 \
        -kernel build/cortex-m33-hello-world.elf \
        -nographic -serial stdio -semihosting
    cd ..
done
```

### 2. SungDB MCP 디버깅 세션
```bash
# HTTP 모드로 MCP 서버 시작
cd ~/sungdb-mcp && ./start_server.sh --http

# GDB 세션 시작 예제
curl -X POST http://localhost:8000/tools/call \
  -H "Content-Type: application/json" \
  -d '{
    "name": "gdb_start",
    "arguments": {"gdb_path": "arm-none-eabi-gdb"}
  }'
```

## 📊 학습 성과 확인

### 메모리 사용량 비교
| 모듈 | TEXT | DATA | BSS | 총합 | 특징 |
|------|------|------|-----|------|------|
| 01-main | 491 | 0 | 0 | 491 | 기본 Hello World |
| 02-memory | 2612 | 12 | 48 | 2672 | 메모리 분석 추가 |
| 03-stack | 3739 | 0 | 12 | 3751 | 복잡한 재귀 함수 |
| 04-heap | 4577 | 4 | 1272 | 5853 | 힙 구현 (1KB) |

### 핵심 개념 정리

#### 메모리 영역별 특성
- **TEXT**: 프로그램 코드 + 상수, Flash/ROM, 읽기 전용
- **DATA**: 초기화된 전역변수, RAM, 부팅 시 ROM에서 복사
- **BSS**: 초기화되지 않은 전역변수, RAM, 부팅 시 0으로 설정
- **Stack**: 지역변수 + 함수 호출, RAM, 자동 관리
- **Heap**: 동적 할당, RAM, 프로그래머 관리

#### 디버깅 기법
- **브레이크포인트**: 특정 지점에서 실행 중단
- **단계 실행**: 한 줄씩 또는 함수 단위 실행
- **메모리 덤프**: 특정 주소의 메모리 내용 확인
- **스택 추적**: 함수 호출 경로 확인
- **변수 관찰**: 실시간 변수 값 모니터링

## 🎓 교육 효과

이 프로젝트를 통해 학습자는:

1. **실제 하드웨어 환경** 시뮬레이션 경험
2. **전문 디버깅 도구** 활용 능력 습득  
3. **메모리 관리** 원리의 깊은 이해
4. **임베디드 시스템** 개발의 실무 감각
5. **문제 해결** 능력 향상

## 🔧 고급 실습 아이디어

### 추가 학습 주제
1. **인터럽트 처리**: 타이머, UART, GPIO 인터럽트
2. **RTOS 기초**: 태스크 스케줄링, 동기화
3. **저전력 모드**: Sleep, Stop, Standby 모드
4. **통신 프로토콜**: SPI, I2C, UART 구현
5. **Flash 프로그래밍**: 부트로더, OTA 업데이트

### 성능 최적화
1. **코드 최적화**: 컴파일러 옵션, 어셈블리 최적화
2. **메모리 최적화**: 구조체 패킹, 배열 정렬
3. **전력 최적화**: 클럭 관리, 모듈 비활성화

## 📝 라이선스

이 프로젝트는 교육 목적으로 제작되었으며, MIT 라이선스 하에 배포됩니다.

---

**Happy Learning! 🚀**

*SungDB MCP와 함께하는 임베디드 시스템 마스터 여정을 시작하세요!*
