# LogAnalyzer 테스트 커버리지 분석 및 향상 계획

## 📊 현재 테스트 커버리지 분석

### 커버리지 요약 (gcov 결과)

- **LogFileReader.cpp**: 83.54% (79줄 중 66줄 실행)
- **LogParser.cpp**: 92.38% (105줄 중 97줄 실행)
- **LogStats.cpp**: 89.76% (127줄 중 114줄 실행)
- **LogEntry.hpp**: 0.00% (21줄 중 0줄 실행)

### 📋 현재 테스트 현황

#### ✅ 잘 테스트된 영역

1. **LogParser 클래스 (92.38% 커버리지)**

   - 기본 파싱 기능 (ERROR, INFO, WARN, DEBUG 레벨)
   - 잘못된 입력 처리 (빈 라인, 짧은 라인, 형식 오류)
   - 여러 라인 파싱 및 성공률 계산
   - 필터링 기능 (키워드, 레벨별, 최소 레벨)
   - 통계 관리 (파싱 성공률, 초기화)

2. **LogStats 클래스 (89.76% 커버리지)**

   - 기본 통계 계산 (카운팅, 비율)
   - 시간 정보 관리
   - 출력 기능 (콘솔, 문자열, JSON)
   - 임계값 검사 및 문제 감지
   - 통계 초기화

3. **LogFileReader 클래스 (83.54% 커버리지)**
   - 파일 유효성 검사
   - 파일 읽기 (라인별, 전체)
   - 오류 처리 (존재하지 않는 파일, 디렉토리, 권한)
   - 파일 정보 조회 (크기, 경로, 라인 번호)

#### ❌ 테스트 부족 영역

1. **LogEntry.hpp (0% 커버리지)**

   - `LogEntry` 구조체의 유틸리티 메서드들이 전혀 테스트되지 않음
   - `containsKeyword()` 메서드
   - `isValid()` 메서드
   - `logLevelToString()`, `stringToLogLevel()` 유틸리티 함수들

2. **LogFileReader 미테스트 영역 (16.46% 미커버)**

   - UTF-8 BOM 처리 로직
   - 복잡한 파일 권한 시나리오
   - 이동 생성자/연산자 (실제로는 테스트됨)
   - 대용량 파일 처리 시나리오

3. **LogParser 미테스트 영역 (7.62% 미커버)**

   - 정규식 예외 처리 경로
   - 복잡한 타임스탬프 형식
   - 예외적인 메시지 추출 케이스

4. **LogStats 미테스트 영역 (10.24% 미커버)**

   - JSON 출력 오류 처리
   - 매우 큰 데이터셋 처리
   - 시간 측정 정확성

5. **main.cpp 통합 테스트 부족**
   - 명령줄 인자 파싱 테스트 없음
   - 전체 워크플로우 통합 테스트 없음
   - JSON 출력 기능 테스트 없음

## 🎯 테스트 커버리지 향상 계획

### Phase 1: LogEntry 유틸리티 함수 테스트 (우선순위: 높음)

```cpp
// 추가할 테스트 케이스들
TEST_CASE("LogEntry 유틸리티 함수 테스트", "[LogEntry]")
{
    SECTION("isValid() 메서드")
    {
        // 유효한 엔트리
        LogEntry validEntry("2023-12-01 10:00:00", LogLevel::INFO, "Test message", 1);
        REQUIRE(validEntry.isValid());

        // 무효한 엔트리들
        LogEntry unknownLevel("2023-12-01 10:00:00", LogLevel::UNKNOWN, "Message", 1);
        REQUIRE_FALSE(unknownLevel.isValid());

        LogEntry emptyMessage("2023-12-01 10:00:00", LogLevel::INFO, "", 1);
        REQUIRE_FALSE(emptyMessage.isValid());
    }

    SECTION("containsKeyword() 메서드")
    {
        LogEntry entry("2023-12-01 10:00:00", LogLevel::INFO, "Database connection failed", 1);

        // 대소문자 구분 검색
        REQUIRE(entry.containsKeyword("Database"));
        REQUIRE(entry.containsKeyword("connection"));
        REQUIRE_FALSE(entry.containsKeyword("database")); // 소문자

        // 타임스탬프에서 검색
        REQUIRE(entry.containsKeyword("2023"));

        // 빈 키워드
        REQUIRE_FALSE(entry.containsKeyword(""));
    }
}

TEST_CASE("LogLevel 유틸리티 함수 테스트", "[LogLevel]")
{
    SECTION("logLevelToString() 함수")
    {
        REQUIRE(logLevelToString(LogLevel::ERROR) == "ERROR");
        REQUIRE(logLevelToString(LogLevel::WARN) == "WARN");
        REQUIRE(logLevelToString(LogLevel::INFO) == "INFO");
        REQUIRE(logLevelToString(LogLevel::DEBUG) == "DEBUG");
        REQUIRE(logLevelToString(LogLevel::UNKNOWN) == "UNKNOWN");
    }

    SECTION("stringToLogLevel() 함수")
    {
        REQUIRE(stringToLogLevel("ERROR") == LogLevel::ERROR);
        REQUIRE(stringToLogLevel("WARNING") == LogLevel::WARN);
        REQUIRE(stringToLogLevel("WARN") == LogLevel::WARN);
        REQUIRE(stringToLogLevel("INFO") == LogLevel::INFO);
        REQUIRE(stringToLogLevel("DEBUG") == LogLevel::DEBUG);
        REQUIRE(stringToLogLevel("INVALID") == LogLevel::UNKNOWN);
    }
}
```

### Phase 2: 엣지 케이스 및 오류 처리 테스트 (우선순위: 중간)

```cpp
// LogFileReader 추가 테스트
TEST_CASE("LogFileReader 고급 기능 테스트", "[LogFileReader][advanced]")
{
    SECTION("UTF-8 BOM 처리")
    {
        // UTF-8 BOM이 있는 파일 생성 및 테스트
    }

    SECTION("대용량 파일 처리")
    {
        // 10MB 이상 파일 생성 및 성능 테스트
    }

    SECTION("특수 문자 포함 파일명")
    {
        // 공백, 한글 등이 포함된 파일명 테스트
    }
}

// LogParser 추가 테스트
TEST_CASE("LogParser 엣지 케이스 테스트", "[LogParser][edge]")
{
    SECTION("복잡한 타임스탬프 형식")
    {
        // ISO 8601, 밀리초 포함 등 다양한 형식
    }

    SECTION("정규식 예외 처리")
    {
        // 매우 긴 라인, 특수 문자 등으로 정규식 예외 유발
    }

    SECTION("메모리 효율성")
    {
        // 대량의 로그 엔트리 처리 테스트
    }
}
```

### Phase 3: 통합 테스트 및 main.cpp 테스트 (우선순위: 중간)

```cpp
// 새로운 파일: tests/test_integration.cpp
TEST_CASE("통합 테스트", "[integration]")
{
    SECTION("전체 워크플로우 테스트")
    {
        // 파일 읽기 → 파싱 → 통계 계산 → 출력
    }

    SECTION("JSON 출력 기능")
    {
        // JSON 파일 생성 및 내용 검증
    }

    SECTION("키워드 검색 통합")
    {
        // 전체 프로세스에서 키워드 검색 테스트
    }
}

// 명령줄 인자 처리 테스트 (main.cpp 함수 분리 필요)
TEST_CASE("명령줄 인자 파싱 테스트", "[cmdline]")
{
    SECTION("기본 사용법")
    {
        // argc, argv 시뮬레이션
    }

    SECTION("옵션 조합")
    {
        // --output-json과 키워드 동시 사용
    }

    SECTION("잘못된 인자")
    {
        // 존재하지 않는 파일, 잘못된 옵션 등
    }
}
```

### Phase 4: 성능 및 스트레스 테스트 (우선순위: 낮음)

```cpp
TEST_CASE("성능 테스트", "[performance]")
{
    SECTION("대용량 로그 파일 처리")
    {
        // 100MB+ 파일 처리 시간 측정
    }

    SECTION("메모리 사용량")
    {
        // 메모리 리크 검사, 최대 메모리 사용량
    }

    SECTION("동시성 테스트")
    {
        // 여러 파일 동시 처리 (미래 확장 고려)
    }
}
```

## 🔧 구현 전략

### 1. 코드 리팩터링 (테스트 가능하도록)

**main.cpp 함수 분리**:

```cpp
// 테스트 가능한 함수들로 분리
struct CommandLineArgs {
    std::string filePath;
    std::string keyword;
    std::string outputJsonPath;
};

CommandLineArgs parseCommandLine(int argc, char* argv[]);
int analyzeLogFile(const CommandLineArgs& args);
```

**LogEntry 인라인 함수 분리**:

- 헤더의 인라인 함수들을 별도 cpp 파일로 이동하여 테스트 가능하게 만들기

### 2. 테스트 파일 구조 개선

```
tests/
├── test_main.cpp              # Catch2 메인
├── test_log_entry.cpp         # 새로 추가
├── test_log_file_reader.cpp   # 기존 + 확장
├── test_log_parser.cpp        # 기존 + 확장
├── test_log_stats.cpp         # 기존 + 확장
├── test_integration.cpp       # 새로 추가
├── test_performance.cpp       # 새로 추가
└── test_utils.hpp             # 테스트 유틸리티
```

### 3. 커버리지 측정 자동화

**CMakeLists.txt 개선**:

```cmake
# 커버리지 타겟 추가
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(LogAnalyzerLib PRIVATE --coverage)
    target_link_libraries(LogAnalyzerLib PRIVATE --coverage)

    # 커버리지 보고서 생성 타겟
    add_custom_target(coverage
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/bin/log_analyzer_tests
        COMMAND gcov -o CMakeFiles/LogAnalyzerLib.dir *.cpp
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generate coverage report"
    )
endif()
```

## 📈 목표 커버리지

- **Phase 1 완료 후**: 95% 이상 (LogEntry 커버리지 100% 달성)
- **Phase 2 완료 후**: 98% 이상 (엣지 케이스 포함)
- **Phase 3 완료 후**: 99% 이상 (통합 테스트 포함)

## 🚀 실행 계획

### Week 1: LogEntry 테스트 구현

- [ ] LogEntry 유틸리티 함수 테스트 작성
- [ ] LogLevel 변환 함수 테스트 작성
- [ ] 첫 번째 커버리지 측정

### Week 2: 엣지 케이스 테스트

- [ ] LogFileReader 고급 기능 테스트
- [ ] LogParser 엣지 케이스 테스트
- [ ] LogStats 오류 처리 테스트

### Week 3: 통합 테스트

- [ ] main.cpp 리팩터링
- [ ] 통합 테스트 작성
- [ ] JSON 출력 테스트

### Week 4: 성능 및 최종 검증

- [ ] 성능 테스트 구현
- [ ] 최종 커버리지 측정
- [ ] 문서화 완료

## 📝 기대 효과

1. **품질 향상**: 99% 커버리지로 버그 발생 가능성 최소화
2. **유지보수성**: 모든 기능에 대한 테스트로 안전한 리팩터링 가능
3. **문서화**: 테스트 코드가 살아있는 문서 역할
4. **교육 효과**: 포괄적인 테스트 작성 경험 제공
5. **CI/CD 준비**: 자동화된 테스트로 지속적 통합 기반 마련
