# 1. Main 함수 실행 과정 분석 - SungDB MCP로 디버깅하기

이 예제에서는 Cortex-M33 마이크로컨트롤러에서 C 프로그램의 main 함수가 어떻게 실행되는지 SungDB MCP를 활용하여 단계별로 분석해보겠습니다.

## 📋 학습 목표

- Cortex-M33 부팅 과정 이해
- 벡터 테이블과 리셋 벡터의 역할
- main 함수에 도달하기까지의 과정
- SungDB MCP를 활용한 실시간 디버깅

## 🛠️ 사전 준비

### 1. 프로젝트 빌드
```bash
# 프로젝트 빌드
make clean && make
```

### 2. SungDB MCP 서버 실행 (HTTP 모드 - 디버깅용)
```bash
# 새 터미널에서 실행
cd ~/sungdb-mcp
./start_server.sh --http
```

## 🔍 코드 분석

### 프로그램 구조
- `src/boot.s`: 부팅 어셈블리 코드 (벡터 테이블, 리셋 핸들러)
- `src/main.c`: C 메인 프로그램
- `linker/cortex-m33.ld`: 링커 스크립트 (메모리 배치)

## 🚀 QEMU에서 실행하며 디버깅

### 1단계: QEMU 시작 및 GDB 연결

```bash
# 터미널 1: QEMU 실행 (GDB 서버 모드)
qemu-system-arm -machine mps2-an505 -cpu cortex-m33 -kernel build/cortex-m33-hello-world.elf -nographic -monitor none -serial stdio -s -S
```

```bash
# 터미널 2: SungDB MCP로 GDB 디버깅 시작
curl -X POST http://localhost:8000/tools/call \
  -H "Content-Type: application/json" \
  -d '{
    "name": "gdb_start",
    "arguments": {
      "gdb_path": "arm-none-eabi-gdb"
    }
  }'
```

**예상 결과:**
```json
{
  "status": "success",
  "session_id": "your-session-id",
  "message": "GDB session started"
}
```

### 2단계: ELF 파일 로드

```bash
# ELF 파일을 GDB에 로드
curl -X POST http://localhost:8000/tools/call \
  -H "Content-Type: application/json" \
  -d '{
    "name": "gdb_load",
    "arguments": {
      "session_id": "your-session-id",
      "program": "build/cortex-m33-hello-world.elf"
    }
  }'
```

### 3단계: QEMU GDB 서버에 연결

```bash
# QEMU의 GDB 서버에 연결 (기본적으로 localhost:1234)
curl -X POST http://localhost:8000/tools/call \
  -H "Content-Type: application/json" \
  -d '{
    "name": "gdb_command",
    "arguments": {
      "session_id": "your-session-id",
      "command": "target remote localhost:1234"
    }
  }'
```

### 4단계: 리셋 벡터 분석

```bash
# 벡터 테이블 확인 (0x08000000 주소부터)
curl -X POST http://localhost:8000/tools/call \
  -H "Content-Type: application/json" \
  -d '{
    "name": "gdb_examine",
    "arguments": {
      "session_id": "your-session-id",
      "expression": "0x08000000",
      "count": 8,
      "format": "xw"
    }
  }'
```

**벡터 테이블 구조:**
```
0x08000000: 스택 포인터 초기값 (MSP)
0x08000004: 리셋 벡터 (Reset_Handler 주소)
0x08000008: NMI 핸들러
0x0800000c: Hard Fault 핸들러
...
```

### 5단계: 리셋 핸들러에 브레이크포인트 설정

```bash
# Reset_Handler에 브레이크포인트 설정
curl -X POST http://localhost:8000/tools/call \
  -H "Content-Type: application/json" \
  -d '{
    "name": "gdb_set_breakpoint",
    "arguments": {
      "session_id": "your-session-id",
      "location": "Reset_Handler"
    }
  }'
```

### 6단계: 프로그램 실행 시작

```bash
# 프로그램 실행 시작
curl -X POST http://localhost:8000/tools/call \
  -H "Content-Type: application/json" \
  -d '{
    "name": "gdb_continue",
    "arguments": {
      "session_id": "your-session-id"
    }
  }'
```

**예상 동작:** Reset_Handler에서 실행이 멈춥니다.

### 7단계: Reset_Handler 코드 분석

```bash
# 현재 어셈블리 코드 확인
curl -X POST http://localhost:8000/tools/call \
  -H "Content-Type: application/json" \
  -d '{
    "name": "gdb_command",
    "arguments": {
      "session_id": "your-session-id",
      "command": "disassemble"
    }
  }'
```

**Reset_Handler의 주요 작업:**
1. 스택 포인터 설정
2. BSS 섹션 초기화 (전역 변수 0으로 초기화)
3. DATA 섹션 복사 (ROM에서 RAM으로)
4. main 함수 호출

### 8단계: main 함수 진입 추적

```bash
# main 함수에 브레이크포인트 설정
curl -X POST http://localhost:8000/tools/call \
  -H "Content-Type: application/json" \
  -d '{
    "name": "gdb_set_breakpoint",
    "arguments": {
      "session_id": "your-session-id",
      "location": "main"
    }
  }'
```

```bash
# 계속 실행하여 main에 도달
curl -X POST http://localhost:8000/tools/call \
  -H "Content-Type: application/json" \
  -d '{
    "name": "gdb_continue",
    "arguments": {
      "session_id": "your-session-id"
    }
  }'
```

### 9단계: main 함수 내부 분석

```bash
# 현재 소스 코드 위치 확인
curl -X POST http://localhost:8000/tools/call \
  -H "Content-Type: application/json" \
  -d '{
    "name": "gdb_command",
    "arguments": {
      "session_id": "your-session-id",
      "command": "list"
    }
  }'
```

```bash
# 스택 상태 확인
curl -X POST http://localhost:8000/tools/call \
  -H "Content-Type: application/json" \
  -d '{
    "name": "gdb_backtrace",
    "arguments": {
      "session_id": "your-session-id",
      "full": true
    }
  }'
```

### 10단계: 함수 호출 추적

```bash
# print_string 함수에 브레이크포인트 설정
curl -X POST http://localhost:8000/tools/call \
  -H "Content-Type: application/json" \
  -d '{
    "name": "gdb_set_breakpoint",
    "arguments": {
      "session_id": "your-session-id",
      "location": "print_string"
    }
  }'
```

```bash
# 단계적 실행 (step into)
curl -X POST http://localhost:8000/tools/call \
  -H "Content-Type: application/json" \
  -d '{
    "name": "gdb_step",
    "arguments": {
      "session_id": "your-session-id"
    }
  }'
```

### 11단계: 세미호스팅 동작 분석

```bash
# semihost_call 함수에서 레지스터 상태 확인
curl -X POST http://localhost:8000/tools/call \
  -H "Content-Type: application/json" \
  -d '{
    "name": "gdb_info_registers",
    "arguments": {
      "session_id": "your-session-id"
    }
  }'
```

```bash
# 인라인 어셈블리 코드 분석
curl -X POST http://localhost:8000/tools/call \
  -H "Content-Type: application/json" \
  -d '{
    "name": "gdb_command",
    "arguments": {
      "session_id": "your-session-id",
      "command": "stepi"
    }
  }'
```

## 📊 학습 결과 정리

### 부팅 과정 요약

1. **하드웨어 리셋**
   - CPU가 0x00000000 (벡터 테이블 베이스) 주소를 참조
   - MSP(Main Stack Pointer) 초기화
   - Reset_Handler 주소로 점프

2. **Reset_Handler 실행**
   - 스택 포인터 설정 확인
   - BSS 섹션 0으로 초기화
   - DATA 섹션 ROM→RAM 복사
   - main 함수 호출

3. **main 함수 실행**
   - C 런타임 환경 완전 초기화 완료
   - 사용자 프로그램 로직 실행

### 핵심 개념

- **벡터 테이블**: 인터럽트와 예외 처리를 위한 함수 포인터 배열
- **리셋 벡터**: 시스템 부팅 시 실행되는 첫 번째 사용자 코드
- **세미호스팅**: 디버거를 통한 입출력 시뮬레이션

## 🔧 추가 실험

### 1. 브레이크포인트 조작
```bash
# 모든 브레이크포인트 확인
curl -X POST http://localhost:8000/tools/call \
  -H "Content-Type: application/json" \
  -d '{
    "name": "gdb_command",
    "arguments": {
      "session_id": "your-session-id",
      "command": "info breakpoints"
    }
  }'
```

### 2. 메모리 덤프
```bash
# 코드 영역 확인
curl -X POST http://localhost:8000/tools/call \
  -H "Content-Type: application/json" \
  -d '{
    "name": "gdb_examine",
    "arguments": {
      "session_id": "your-session-id",
      "expression": "$pc",
      "count": 10,
      "format": "i"
    }
  }'
```

### 3. 변수 관찰
```bash
# 지역 변수 확인
curl -X POST http://localhost:8000/tools/call \
  -H "Content-Type: application/json" \
  -d '{
    "name": "gdb_command",
    "arguments": {
      "session_id": "your-session-id",
      "command": "info locals"
    }
  }'
```

## 🎯 퀴즈

1. Reset_Handler에서 main 함수가 호출되기 전에 어떤 초기화 작업들이 수행되나요?
2. 벡터 테이블의 첫 번째 엔트리는 무엇이고 왜 중요한가요?
3. 세미호스팅은 어떤 방식으로 동작하며, 실제 하드웨어에서는 어떻게 대체되나요?

---

**다음 단계**: [02-memory-layout](../02-memory-layout/) - 메모리 영역과 변수의 이해