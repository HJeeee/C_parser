# C Parser Project

C 언어의 부분집합을 파싱하고 타입 검사를 수행하는 컴파일러 프로젝트입니다.

## 기능

- C 언어 기본 타입 (int, char) 지원
- 포인터, 배열, 구조체 타입 지원
- 함수 선언 및 호출
- 타입 검사 및 오류 보고
- 메모리 누수 방지 (플랫폼별 최적화)

## 빌드 방법

```bash
cd src
make
```

## 테스트 실행

```bash
make test
```

## 플랫폼 지원

- macOS: 메모리 추적 기능 활성화
- Linux (Ubuntu): AddressSanitizer 호환 메모리 관리

## 파일 구조

- `src/subc.l`: Flex lexical analyzer
- `src/subc.y`: Bison parser with semantic analysis
- `src/subc.h`: Header file with type definitions
- `src/Makefile`: Build configuration
- `test/`: Test input files
- `expected/`: Expected output files

## 주요 수정사항

1. **플랫폼별 메모리 관리**: Linux에서 AddressSanitizer 호환성 향상
2. **메모리 누수 방지**: 적절한 메모리 할당/해제 관리
3. **타입 시스템**: 완전한 C 타입 시스템 구현 