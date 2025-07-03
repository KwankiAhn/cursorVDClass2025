# C++ 로그 분석 도구 리팩터링 프로젝트 - 작업 계획서

## 전체 개요

**목표**: `01-legacy-c` 폴더에서 레거시 C 코드를 C++17 기반의 현대적인 객체지향 설계로 리팩터링하여 완전한 로그 분석 도구를 구현합니다.

**주요 작업 내용**:

1. `LogParser` 클래스 - 로그 라인 파싱 및 구조화
2. `LogStats` 클래스 - 통계 계산 및 집계
3. `LogEntry` 구조체 - 파싱된 로그 데이터 표현
4. Catch2 테스트 프레임워크 연동
5. 빌드 시스템 최적화 및 의존성 관리

**기술 스택**: C++17, CMake, Catch2, std::filesystem, std::optional

## 파일 구조

### 현재 상태 (기존)

```
01-legacy-c/
├── CMakeLists.txt          # 기본 빌드 설정
├── LogFileReader.hpp       # 파일 읽기 클래스 (완료)
├── LogFileReader.cpp       # 파일 읽기 구현 (완료)
├── main.cpp               # 기본 테스트 프로그램 (완료)
└── build/                 # 빌드 출력
```

### 목표 상태 (완료 후)

```
01-legacy-c/
├── CMakeLists.txt          # 업데이트된 빌드 설정
├── LogFileReader.hpp/.cpp  # 파일 읽기 (기존)
├── LogEntry.hpp           # 로그 엔트리 구조체 (신규)
├── LogParser.hpp/.cpp     # 로그 파싱 클래스 (신규)
├── LogStats.hpp/.cpp      # 통계 계산 클래스 (신규)
├── main.cpp              # 통합 테스트 프로그램 (업데이트)
├── tests/                # 테스트 디렉토리 (신규)
│   ├── test_main.cpp     # Catch2 메인
│   ├── test_log_parser.cpp
│   ├── test_log_stats.cpp
│   └── test_log_file_reader.cpp
└── build/
    ├── bin/LogAnalyzer    # 메인 실행 파일
    └── bin/log_analyzer_tests # 테스트 실행 파일
```

## 참고 문서

### 기술 참조

- **C++17 표준**: std::filesystem, std::optional, structured bindings
- **CMake 문서**: FetchContent, target_link_libraries, 크로스 플랫폼 빌드
- **Catch2 문서**: 단위 테스트 작성 방법, 테스트 조직화

### 프로젝트 참조

- `cursor-cpp-training/02-cpp17-refactored/` - 목표 구조 참조
- `cursor-cpp-training/03-with-tests/` - 테스트 구조 참조
- `cursor-cpp-training/docs/` - 설계 문서 및 다이어그램

### 코딩 표준 참조

- 프로젝트 네이밍 컨벤션 (camelCase, PascalCase)
- C++17 모던 기능 활용 가이드라인
- RAII 패턴 및 예외 안전성

## 리팩터링 기대 효과

### 1. 코드 품질 향상

- **타입 안전성**: C 스타일 포인터 → 스마트 포인터 및 RAII
- **메모리 안전성**: 수동 메모리 관리 → 자동 리소스 관리
- **예외 안전성**: 오류 코드 → 예외 처리 메커니즘

### 2. 유지보수성 개선

- **모듈화**: 단일 파일 → 클래스별 분리
- **책임 분리**: 단일 책임 원칙 적용
- **확장성**: 새로운 로그 형식이나 통계 추가 용이

### 3. 개발 생산성 증대

- **단위 테스트**: Catch2 프레임워크로 안정성 보장
- **자동 빌드**: CMake로 크로스 플랫폼 빌드
- **IDE 지원**: 현대적 C++ 기능으로 더 나은 IntelliSense

### 4. 성능 최적화

- **메모리 할당**: 벡터 사전 할당으로 성능 향상
- **이동 시맨틱**: 불필요한 복사 제거
- **컴파일 시간**: 헤더 분리로 컴파일 시간 단축

## 작업 체크리스트

### Phase 1: 핵심 클래스 구현

- [ ] **LogEntry.hpp 생성**

  - [ ] 로그 레벨 열거형 (ERROR, WARN, INFO, DEBUG)
  - [ ] LogEntry 구조체 (타임스탬프, 레벨, 메시지)
  - [ ] 필요한 헬퍼 함수들

- [ ] **LogParser 클래스 구현**

  - [ ] LogParser.hpp - 클래스 선언 및 인터페이스
  - [ ] LogParser.cpp - 파싱 로직 구현
  - [ ] 정규식 또는 문자열 파싱으로 로그 라인 분석
  - [ ] 키워드 검색 기능 구현

- [ ] **LogStats 클래스 구현**
  - [ ] LogStats.hpp - 통계 클래스 선언
  - [ ] LogStats.cpp - 통계 계산 및 출력 구현
  - [ ] 레벨별 카운팅, 총계, 비율 계산

### Phase 2: 빌드 시스템 업데이트

- [ ] **CMakeLists.txt 수정**

  - [ ] 새로운 소스 파일들 추가
  - [ ] 의존성 관리 설정
  - [ ] 링크 오류 해결

- [ ] **빌드 테스트**
  - [ ] `rm -rf build && mkdir build && cd build`
  - [ ] `cmake ..` 실행 및 설정 확인
  - [ ] `make` 실행 및 컴파일 오류 해결
  - [ ] 실행 파일 테스트

### Phase 3: Catch2 테스트 프레임워크 연동

- [ ] **CMakeLists.txt에 Catch2 추가**

  - [ ] FetchContent로 Catch2 다운로드 설정
  - [ ] 테스트 타겟 (`log_analyzer_tests`) 생성
  - [ ] CTest 통합 설정

- [ ] **테스트 디렉토리 생성**
  - [ ] `tests/` 폴더 생성
  - [ ] `test_main.cpp` - Catch2 메인 함수
  - [ ] `test_log_file_reader.cpp` - LogFileReader 테스트
  - [ ] `test_log_parser.cpp` - LogParser 테스트
  - [ ] `test_log_stats.cpp` - LogStats 테스트

### Phase 4: 통합 테스트 및 검증

- [ ] **단위 테스트 실행**

  - [ ] `./bin/log_analyzer_tests` 실행
  - [ ] `ctest --verbose` 실행
  - [ ] 모든 테스트 통과 확인

- [ ] **통합 테스트**

  - [ ] main.cpp에서 모든 클래스 연동
  - [ ] 샘플 로그로 전체 기능 테스트
  - [ ] 성능 및 메모리 사용량 확인

- [ ] **문서화**
  - [ ] README 업데이트
  - [ ] 클래스 다이어그램 생성
  - [ ] 사용법 예시 작성

---

**예상 소요 시간**: 2-3시간  
**난이도**: 중급 (C++ 클래스 설계, CMake, 단위 테스트)  
**성공 기준**: 모든 단위 테스트 통과, 링크 오류 없는 빌드, 기능적 로그 분석 도구 완성
