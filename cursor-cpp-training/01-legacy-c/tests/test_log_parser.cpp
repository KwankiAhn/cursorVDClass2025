#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "LogParser.hpp"
#include "LogEntry.hpp"

TEST_CASE("LogParser 기본 파싱 테스트", "[LogParser]")
{
    LogParser parser;
    
    SECTION("ERROR 레벨 감지")
    {
        std::string errorLine = "2023-12-01 10:30:15 ERROR Database connection failed";
        auto entry = parser.parseLine(errorLine, 1);
        
        REQUIRE(entry.has_value());
        REQUIRE(entry->level == LogLevel::ERROR);
        REQUIRE(entry->timestamp == "2023-12-01 10:30:15");
        REQUIRE(entry->message == "Database connection failed");
        REQUIRE(entry->lineNumber == 1);
    }
    
    SECTION("INFO 레벨 감지")
    {
        std::string infoLine = "2023-12-01 10:30:15 INFO Application started successfully";
        auto entry = parser.parseLine(infoLine, 2);
        
        REQUIRE(entry.has_value());
        REQUIRE(entry->level == LogLevel::INFO);
        REQUIRE(entry->timestamp == "2023-12-01 10:30:15");
        REQUIRE(entry->message == "Application started successfully");
    }
    
    SECTION("WARN 레벨 감지")
    {
        std::string warnLine = "2023-12-01 10:30:15 WARN Configuration file permission is too open";
        auto entry = parser.parseLine(warnLine, 3);
        
        REQUIRE(entry.has_value());
        REQUIRE(entry->level == LogLevel::WARN);
        REQUIRE(entry->message == "Configuration file permission is too open");
    }
    
    SECTION("WARNING 키워드 처리 (WARN과 동일)")
    {
        std::string warningLine = "2023-12-01 10:30:15 WARNING Memory usage is high";
        auto entry = parser.parseLine(warningLine, 4);
        
        REQUIRE(entry.has_value());
        REQUIRE(entry->level == LogLevel::WARN);
        REQUIRE(entry->message == "Memory usage is high");
    }
}

TEST_CASE("LogParser 잘못된 입력 처리", "[LogParser][error]")
{
    LogParser parser;
    
    SECTION("빈 라인 처리")
    {
        std::string emptyLine = "";
        auto entry = parser.parseLine(emptyLine);
        REQUIRE_FALSE(entry.has_value());
    }
    
    SECTION("너무 짧은 라인 처리")
    {
        std::string shortLine = "short";
        auto entry = parser.parseLine(shortLine);
        REQUIRE_FALSE(entry.has_value());
    }
    
    SECTION("타임스탬프가 없는 라인")
    {
        std::string noTimestamp = "ERROR Some error message";
        auto entry = parser.parseLine(noTimestamp);
        REQUIRE_FALSE(entry.has_value());
    }
    
    SECTION("로그 레벨이 없는 라인")
    {
        std::string noLevel = "2023-12-01 10:30:15 Some message without level";
        auto entry = parser.parseLine(noLevel);
        REQUIRE_FALSE(entry.has_value());
    }
}

TEST_CASE("LogParser 여러 라인 파싱", "[LogParser][multiline]")
{
    LogParser parser;
    
    std::vector<std::string> testLines = {
        "2023-12-01 09:00:00 INFO Application started successfully",
        "2023-12-01 09:00:01 INFO Loading configuration",
        "2023-12-01 09:00:02 ERROR Database connection failed",
        "invalid line without proper format",
        "2023-12-01 09:00:03 WARN Configuration file permission is too open"
    };
    
    auto entries = parser.parseLines(testLines);
    
    REQUIRE(entries.size() == 4); // 4개의 유효한 라인
    REQUIRE(entries[0].level == LogLevel::INFO);
    REQUIRE(entries[1].level == LogLevel::INFO);
    REQUIRE(entries[2].level == LogLevel::ERROR);
    REQUIRE(entries[3].level == LogLevel::WARN);
    
    // 파싱 성공률 확인 (4/5 = 0.8)
    REQUIRE(parser.getParseSuccessRate() == Catch::Approx(0.8).epsilon(0.01));
}

TEST_CASE("LogParser 필터링 기능", "[LogParser][filter]")
{
    LogParser parser;
    
    std::vector<LogEntry> testEntries = {
        LogEntry("2023-12-01 09:00:00", LogLevel::INFO, "Application started", 1),
        LogEntry("2023-12-01 09:00:01", LogLevel::ERROR, "Database connection failed", 2),
        LogEntry("2023-12-01 09:00:02", LogLevel::WARN, "Memory usage high", 3),
        LogEntry("2023-12-01 09:00:03", LogLevel::INFO, "User login successful", 4),
        LogEntry("2023-12-01 09:00:04", LogLevel::ERROR, "Disk space low", 5)
    };
    
    SECTION("레벨별 필터링")
    {
        auto errorEntries = parser.filterByLevel(testEntries, LogLevel::ERROR);
        REQUIRE(errorEntries.size() == 2);
        REQUIRE(errorEntries[0].message == "Database connection failed");
        REQUIRE(errorEntries[1].message == "Disk space low");
        
        auto infoEntries = parser.filterByLevel(testEntries, LogLevel::INFO);
        REQUIRE(infoEntries.size() == 2);
    }
    
    SECTION("최소 레벨 필터링")
    {
        auto criticalEntries = parser.filterByMinLevel(testEntries, LogLevel::WARN);
        REQUIRE(criticalEntries.size() == 3); // WARN + ERROR (2개)
        
        auto errorOnlyEntries = parser.filterByMinLevel(testEntries, LogLevel::ERROR);
        REQUIRE(errorOnlyEntries.size() == 2); // ERROR만
    }
    
    SECTION("키워드 필터링")
    {
        auto databaseEntries = parser.filterByKeyword(testEntries, "Database");
        REQUIRE(databaseEntries.size() == 1);
        REQUIRE(databaseEntries[0].message == "Database connection failed");
        
        auto userEntries = parser.filterByKeyword(testEntries, "User");
        REQUIRE(userEntries.size() == 1);
        REQUIRE(userEntries[0].message == "User login successful");
        
        auto emptyResult = parser.filterByKeyword(testEntries, "nonexistent");
        REQUIRE(emptyResult.empty());
        
        auto emptyKeyword = parser.filterByKeyword(testEntries, "");
        REQUIRE(emptyKeyword.size() == testEntries.size()); // 빈 키워드는 모든 항목 반환
    }
}

TEST_CASE("LogParser 통계 기능", "[LogParser][stats]")
{
    LogParser parser;
    
    SECTION("초기 상태")
    {
        REQUIRE(parser.getParseSuccessRate() == 0.0);
    }
    
    SECTION("통계 초기화")
    {
        // 몇 개 파싱 후
        parser.parseLine("2023-12-01 09:00:00 INFO Test message");
        parser.parseLine("invalid line");
        
        REQUIRE(parser.getParseSuccessRate() == Catch::Approx(0.5).epsilon(0.01));
        
        // 초기화
        parser.resetStatistics();
        REQUIRE(parser.getParseSuccessRate() == 0.0);
    }
}

TEST_CASE("LogEntry 유틸리티 함수 테스트", "[LogEntry]")
{
    SECTION("로그 레벨 문자열 변환")
    {
        REQUIRE(logLevelToString(LogLevel::ERROR) == "ERROR");
        REQUIRE(logLevelToString(LogLevel::WARN) == "WARN");
        REQUIRE(logLevelToString(LogLevel::INFO) == "INFO");
        REQUIRE(logLevelToString(LogLevel::DEBUG) == "DEBUG");
        REQUIRE(logLevelToString(LogLevel::UNKNOWN) == "UNKNOWN");
    }
    
    SECTION("문자열을 로그 레벨로 변환")
    {
        REQUIRE(stringToLogLevel("ERROR") == LogLevel::ERROR);
        REQUIRE(stringToLogLevel("WARN") == LogLevel::WARN);
        REQUIRE(stringToLogLevel("WARNING") == LogLevel::WARN);
        REQUIRE(stringToLogLevel("INFO") == LogLevel::INFO);
        REQUIRE(stringToLogLevel("DEBUG") == LogLevel::DEBUG);
        REQUIRE(stringToLogLevel("INVALID") == LogLevel::UNKNOWN);
    }
    
    SECTION("LogEntry 유효성 검사")
    {
        LogEntry validEntry("2023-12-01 09:00:00", LogLevel::INFO, "Test message");
        REQUIRE(validEntry.isValid());
        
        LogEntry invalidEntry1("", LogLevel::UNKNOWN, "");
        REQUIRE_FALSE(invalidEntry1.isValid());
        
        LogEntry invalidEntry2("2023-12-01 09:00:00", LogLevel::INFO, "");
        REQUIRE_FALSE(invalidEntry2.isValid());
    }
    
    SECTION("키워드 포함 검사")
    {
        LogEntry entry("2023-12-01 09:00:00", LogLevel::INFO, "Database connection successful");
        
        REQUIRE(entry.containsKeyword("Database"));
        REQUIRE(entry.containsKeyword("connection"));
        REQUIRE(entry.containsKeyword("2023-12-01")); // 타임스탬프에서도 검색
        REQUIRE_FALSE(entry.containsKeyword("Error"));
        REQUIRE_FALSE(entry.containsKeyword(""));
    }
} 