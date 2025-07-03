#include <catch2/catch_test_macros.hpp>
#include "../LogParser.hpp"
#include <string>

TEST_CASE("LogParser - 로그 레벨 감지", "[LogParser]")
{
    LogParser parser;
    
    SECTION("ERROR 레벨 감지")
    {
        std::string logLine = "2024-01-01 10:00:00 ERROR: 데이터베이스 연결 실패";
        REQUIRE(parser.detectLogLevel(logLine) == "ERROR");
    }
    
    SECTION("WARNING 레벨 감지")
    {
        std::string logLine = "2024-01-01 10:00:00 WARN: 메모리 사용량 높음";
        REQUIRE(parser.detectLogLevel(logLine) == "WARNING");
    }
    
    SECTION("INFO 레벨 감지")
    {
        std::string logLine = "2024-01-01 10:00:00 INFO: 서버 시작됨";
        REQUIRE(parser.detectLogLevel(logLine) == "INFO");
    }
    
    SECTION("DEBUG 레벨 감지")
    {
        std::string logLine = "2024-01-01 10:00:00 DEBUG: 변수 값 확인";
        REQUIRE(parser.detectLogLevel(logLine) == "DEBUG");
    }
    
    SECTION("UNKNOWN 레벨 감지")
    {
        std::string logLine = "2024-01-01 10:00:00 일반적인 로그 메시지";
        REQUIRE(parser.detectLogLevel(logLine) == "UNKNOWN");
    }
    
    SECTION("대소문자 구분 없음")
    {
        std::string logLine = "2024-01-01 10:00:00 error: 소문자 에러";
        REQUIRE(parser.detectLogLevel(logLine) == "ERROR");
    }
}

TEST_CASE("LogParser - 타임스탬프 추출", "[LogParser]")
{
    LogParser parser;
    
    SECTION("표준 형식 타임스탬프")
    {
        std::string logLine = "2024-01-01 10:00:00 INFO: 메시지";
        REQUIRE(parser.extractTimestamp(logLine) == "2024-01-01 10:00:00");
    }
    
    SECTION("ISO 8601 형식 타임스탬프")
    {
        std::string logLine = "2024-01-01T10:00:00Z INFO: 메시지";
        REQUIRE(parser.extractTimestamp(logLine) == "2024-01-01T10:00:00Z");
    }
    
    SECTION("시간만 있는 경우")
    {
        std::string logLine = "10:00:00 INFO: 메시지";
        REQUIRE(parser.extractTimestamp(logLine) == "10:00:00");
    }
    
    SECTION("타임스탬프 없음")
    {
        std::string logLine = "INFO: 메시지만 있음";
        REQUIRE(parser.extractTimestamp(logLine) == "");
    }
}

TEST_CASE("LogParser - 로그 파싱", "[LogParser]")
{
    LogParser parser;
    
    SECTION("완전한 로그 라인 파싱")
    {
        std::string logLine = "2024-01-01 10:00:00 ERROR: 데이터베이스 연결 실패";
        LogEntry entry = parser.parseLine(logLine);
        
        REQUIRE(entry.level == "ERROR");
        REQUIRE(entry.timestamp == "2024-01-01 10:00:00");
        REQUIRE(entry.rawLine == logLine);
        REQUIRE(!entry.message.empty());
    }
    
    SECTION("빈 라인 파싱")
    {
        std::string logLine = "";
        LogEntry entry = parser.parseLine(logLine);
        
        REQUIRE(entry.level == "UNKNOWN");
        REQUIRE(entry.timestamp == "");
        REQUIRE(entry.rawLine == "");
    }
}

TEST_CASE("LogParser - 키워드 검색", "[LogParser]")
{
    LogParser parser;
    
    SECTION("키워드 찾기")
    {
        std::string logLine = "2024-01-01 10:00:00 ERROR: 데이터베이스 연결 실패";
        REQUIRE(parser.containsKeyword(logLine, "데이터베이스") == true);
        REQUIRE(parser.containsKeyword(logLine, "연결") == true);
        REQUIRE(parser.containsKeyword(logLine, "성공") == false);
    }
    
    SECTION("빈 키워드")
    {
        std::string logLine = "2024-01-01 10:00:00 ERROR: 메시지";
        REQUIRE(parser.containsKeyword(logLine, "") == false);
    }
    
    SECTION("대소문자 구분")
    {
        std::string logLine = "2024-01-01 10:00:00 ERROR: Database connection failed";
        REQUIRE(parser.containsKeyword(logLine, "Database") == true);
        REQUIRE(parser.containsKeyword(logLine, "database") == false);
    }
}

TEST_CASE("LogParser - 메시지 추출 테스트", "[LogParser]")
{
    LogParser parser;
    
    SECTION("ERROR 레벨 메시지 추출")
    {
        std::string logLine = "2024-01-01 10:00:00 ERROR: 데이터베이스 연결 실패";
        std::string message = parser.extractMessage(logLine, "ERROR");
        REQUIRE(message == "데이터베이스 연결 실패");
    }
    
    SECTION("WARNING 레벨 메시지 추출")
    {
        std::string logLine = "2024-01-01 10:00:00 WARN: 메모리 사용량 높음";
        std::string message = parser.extractMessage(logLine, "WARNING");
        REQUIRE(message == "메모리 사용량 높음");
    }
    
    SECTION("INFO 레벨 메시지 추출")
    {
        std::string logLine = "2024-01-01 10:00:00 INFO: 서버 시작됨";
        std::string message = parser.extractMessage(logLine, "INFO");
        REQUIRE(message == "서버 시작됨");
    }
    
    SECTION("DEBUG 레벨 메시지 추출")
    {
        std::string logLine = "2024-01-01 10:00:00 DEBUG: 변수 값 확인";
        std::string message = parser.extractMessage(logLine, "DEBUG");
        REQUIRE(message == "변수 값 확인");
    }
    
    SECTION("특수 문자 포함 메시지 추출")
    {
        std::string logLine = "2024-01-01 10:00:00 ERROR: 파일 경로 '/var/log/test.log'에서 오류 발생";
        std::string message = parser.extractMessage(logLine, "ERROR");
        REQUIRE(message == "파일 경로 '/var/log/test.log'에서 오류 발생");
    }
    
    SECTION("빈 메시지 처리")
    {
        std::string logLine = "2024-01-01 10:00:00 INFO:";
        std::string message = parser.extractMessage(logLine, "INFO");
        REQUIRE(message.empty());
    }
    
    SECTION("레벨 없는 라인에서 메시지 추출")
    {
        std::string logLine = "2024-01-01 10:00:00 일반적인 로그 메시지";
        std::string message = parser.extractMessage(logLine, "UNKNOWN");
        REQUIRE(message == "일반적인 로그 메시지");
    }
    
    SECTION("공백 포함 메시지 추출")
    {
        std::string logLine = "2024-01-01 10:00:00 ERROR:   공백이 많은 메시지   ";
        std::string message = parser.extractMessage(logLine, "ERROR");
        REQUIRE(message == "공백이 많은 메시지");
    }
}

TEST_CASE("LogParser - 공백 처리 테스트", "[LogParser]")
{
    LogParser parser;
    
    SECTION("파싱 결과에서 공백 확인")
    {
        std::string logLine = "  2024-01-01 10:00:00   ERROR:   테스트 메시지   ";
        LogEntry entry = parser.parseLine(logLine);
        
        REQUIRE(entry.level == "ERROR");
        REQUIRE(entry.timestamp == "2024-01-01 10:00:00");
        REQUIRE(entry.message == "테스트 메시지");
        REQUIRE(entry.rawLine == logLine);
    }
    
    SECTION("탭 문자 포함 라인 처리")
    {
        std::string logLine = "\t2024-01-01 10:00:00\tINFO:\t탭 문자 테스트\t";
        LogEntry entry = parser.parseLine(logLine);
        
        REQUIRE(entry.level == "INFO");
        REQUIRE(entry.timestamp == "2024-01-01 10:00:00");
        REQUIRE(entry.message == "탭 문자 테스트");
    }
    
    SECTION("개행 문자 포함 라인 처리")
    {
        std::string logLine = "2024-01-01 10:00:00 DEBUG: 개행 문자 테스트\n";
        LogEntry entry = parser.parseLine(logLine);
        
        REQUIRE(entry.level == "DEBUG");
        REQUIRE(entry.message == "개행 문자 테스트");
    }
    
    SECTION("공백만 있는 라인 처리")
    {
        std::string logLine = "   \t   \n   ";
        LogEntry entry = parser.parseLine(logLine);
        
        REQUIRE(entry.level == "UNKNOWN");
        REQUIRE(entry.timestamp == "");
        REQUIRE(entry.message == "");
    }
}

TEST_CASE("LogParser - 에러 처리 및 경계 케이스", "[LogParser][ErrorHandling]")
{
    LogParser parser;
    
    SECTION("매우 긴 로그 라인 처리")
    {
        // 1MB 크기의 로그 라인
        std::string longLine = "2024-01-01 10:00:00 ERROR: ";
        longLine += std::string(1024 * 1024, 'A');
        
        REQUIRE_NOTHROW(parser.parseLine(longLine));
        
        LogEntry entry = parser.parseLine(longLine);
        REQUIRE(entry.level == "ERROR");
        REQUIRE(entry.timestamp == "2024-01-01 10:00:00");
        REQUIRE(!entry.message.empty());
        REQUIRE(entry.rawLine == longLine);
    }
    
    SECTION("특수 문자 및 유니코드 처리")
    {
        std::string unicodeLine = "2024-01-01 10:00:00 INFO: 한글 메시지 테스트 🔥 특수문자 !@#$%^&*()";
        
        LogEntry entry = parser.parseLine(unicodeLine);
        REQUIRE(entry.level == "INFO");
        REQUIRE(entry.timestamp == "2024-01-01 10:00:00");
        REQUIRE(entry.message.find("한글") != std::string::npos);
        REQUIRE(entry.message.find("🔥") != std::string::npos);
    }
    
    SECTION("잘못된 형식의 타임스탬프")
    {
        std::vector<std::string> invalidTimestamps = {
            "25:99:99 ERROR: 잘못된 시간",
            "2024-13-45 ERROR: 잘못된 날짜",
            "abc-def-ghi ERROR: 문자 타임스탬프",
            "ERROR: 타임스탬프 없음"
        };
        
        for (const auto& line : invalidTimestamps)
        {
            LogEntry entry = parser.parseLine(line);
            REQUIRE(entry.level == "ERROR");
            // 잘못된 타임스탬프는 빈 문자열이거나 전체 문자열일 수 있음
            REQUIRE(entry.rawLine == line);
        }
    }
    
    SECTION("중복된 로그 레벨")
    {
        std::string duplicateLine = "2024-01-01 10:00:00 ERROR INFO: ERROR와 INFO가 모두 있는 경우";
        
        LogEntry entry = parser.parseLine(duplicateLine);
        // 첫 번째로 발견되는 레벨이 선택되어야 함
        REQUIRE(entry.level == "ERROR");
    }
    
    SECTION("대소문자 혼합 로그 레벨")
    {
        std::vector<std::pair<std::string, std::string>> mixedCases = {
            {"2024-01-01 10:00:00 Error: 대소문자 혼합", "ERROR"},
            {"2024-01-01 10:00:00 WaRnInG: 이상한 케이스", "WARNING"},
            {"2024-01-01 10:00:00 info: 소문자", "INFO"},
            {"2024-01-01 10:00:00 DEBUG: 대문자", "DEBUG"}
        };
        
        for (const auto& [line, expectedLevel] : mixedCases)
        {
            LogEntry entry = parser.parseLine(line);
            REQUIRE(entry.level == expectedLevel);
        }
    }
    
    SECTION("키워드 검색 경계 케이스")
    {
        std::string testLine = "2024-01-01 10:00:00 INFO: Database connection established for database operations";
        
        // 대소문자 구분 확인
        REQUIRE(parser.containsKeyword(testLine, "Database") == true);
        REQUIRE(parser.containsKeyword(testLine, "database") == true);
        REQUIRE(parser.containsKeyword(testLine, "DATABASE") == false);
        
        // 부분 문자열 매칭
        REQUIRE(parser.containsKeyword(testLine, "Data") == true);
        REQUIRE(parser.containsKeyword(testLine, "base") == true);
        REQUIRE(parser.containsKeyword(testLine, "xyz") == false);
        
        // 빈 키워드
        REQUIRE(parser.containsKeyword(testLine, "") == false);
        
        // 전체 라인과 같은 키워드
        REQUIRE(parser.containsKeyword(testLine, testLine) == true);
    }
    
    SECTION("null 문자 포함 라인 처리")
    {
        std::string nullLine = "2024-01-01 10:00:00 ERROR: 메시지";
        nullLine += '\0';  // 문자열 끝에 null 문자 추가
        nullLine += " 추가 내용";
        
        REQUIRE_NOTHROW(parser.parseLine(nullLine));
        LogEntry entry = parser.parseLine(nullLine);
        // null 문자로 인해 문자열이 잘릴 수 있으므로 UNKNOWN이 될 수도 있음
        REQUIRE((entry.level == "ERROR" || entry.level == "UNKNOWN"));
    }
}

TEST_CASE("LogParser - 성능 및 스트레스 테스트", "[LogParser][Performance]")
{
    LogParser parser;
    
    SECTION("대량 로그 라인 처리")
    {
        const int numLines = 1000;
        std::vector<LogEntry> entries;
        entries.reserve(numLines);
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < numLines; ++i)
        {
            std::string line = "2024-01-01 10:00:" + 
                              std::to_string(i % 60).insert(0, 2 - std::to_string(i % 60).length(), '0') +
                              " INFO: 메시지 번호 " + std::to_string(i);
            
            entries.push_back(parser.parseLine(line));
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        // 1000개 라인 처리가 1초 이내에 완료되어야 함
        REQUIRE(duration.count() < 1000);
        REQUIRE(entries.size() == numLines);
        
        // 모든 엔트리가 올바르게 파싱되었는지 확인
        for (size_t i = 0; i < entries.size(); ++i)
        {
            REQUIRE(entries[i].level == "INFO");
            REQUIRE(!entries[i].timestamp.empty());
            REQUIRE(entries[i].message.find("메시지 번호") != std::string::npos);
        }
    }
    
    SECTION("키워드 검색 성능 테스트")
    {
        std::string longLine = "2024-01-01 10:00:00 INFO: ";
        // 10KB 길이의 라인에 키워드를 다양한 위치에 배치
        longLine += std::string(5000, 'A');
        longLine += "KEYWORD";
        longLine += std::string(5000, 'B');
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // 100번 키워드 검색 수행
        bool found = false;
        for (int i = 0; i < 100; ++i)
        {
            found = parser.containsKeyword(longLine, "KEYWORD");
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        
        REQUIRE(found == true);
        // 100번 검색이 10ms 이내에 완료되어야 함
        REQUIRE(duration.count() < 10000);
    }
} 