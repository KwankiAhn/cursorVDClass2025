#include <catch2/catch_test_macros.hpp>
#include "../LogStats.hpp"
#include "../LogParser.hpp"
#include <iostream>
#include <sstream>

using namespace LogAnalyzer;

TEST_CASE("LogStats 통계 계산 테스트", "[LogStats]") {
    LogStats stats;
    
    SECTION("기본 통계 계산") {
        std::vector<LogEntry> entries = {
            LogEntry("Error message 1", LogLevel::ERROR),
            LogEntry("Error message 2", LogLevel::ERROR),
            LogEntry("Warning message", LogLevel::WARNING),
            LogEntry("Info message 1", LogLevel::INFO),
            LogEntry("Info message 2", LogLevel::INFO),
            LogEntry("Info message 3", LogLevel::INFO),
            LogEntry("Unknown message", LogLevel::UNKNOWN)
        };
        
        auto statistics = stats.calculateStats(entries, "/test/path.log", 1024);
        
        REQUIRE(statistics.totalLines == 7);
        REQUIRE(statistics.filePath == "/test/path.log");
        REQUIRE(statistics.fileSize == 1024);
        REQUIRE(statistics.levelCounts[LogLevel::ERROR] == 2);
        REQUIRE(statistics.levelCounts[LogLevel::WARNING] == 1);
        REQUIRE(statistics.levelCounts[LogLevel::INFO] == 3);
        REQUIRE(statistics.levelCounts[LogLevel::UNKNOWN] == 1);
    }
    
    SECTION("빈 엔트리 처리") {
        std::vector<LogEntry> entries;
        auto statistics = stats.calculateStats(entries);
        
        REQUIRE(statistics.totalLines == 0);
        REQUIRE(statistics.levelCounts.empty());
    }
}

TEST_CASE("LogStats JSON 출력 테스트", "[LogStats]") {
    LogStats stats;
    
    std::vector<LogEntry> entries = {
        LogEntry("Error message", LogLevel::ERROR),
        LogEntry("Warning message", LogLevel::WARNING),
        LogEntry("Info message", LogLevel::INFO)
    };
    
    auto statistics = stats.calculateStats(entries, "/test/log.txt", 512);
    std::string json = stats.statsToJson(statistics);
    
    // JSON 형식 기본 검증
    REQUIRE(json.find("\"filePath\"") != std::string::npos);
    REQUIRE(json.find("\"fileSize\"") != std::string::npos);
    REQUIRE(json.find("\"totalLines\"") != std::string::npos);
    REQUIRE(json.find("\"levelCounts\"") != std::string::npos);
    REQUIRE(json.find("/test/log.txt") != std::string::npos);
    REQUIRE(json.find("512") != std::string::npos);
    REQUIRE(json.find("3") != std::string::npos); // totalLines
}

TEST_CASE("LogStats 통계 출력 테스트", "[LogStats]") {
    LogStats stats;
    
    std::vector<LogEntry> entries = {
        LogEntry("Error message", LogLevel::ERROR),
        LogEntry("Info message", LogLevel::INFO)
    };
    
    auto statistics = stats.calculateStats(entries, "/test/log.txt", 256);
    
    SECTION("기본 통계 출력") {
        // 출력 캡처를 위한 스트림 리다이렉션
        std::ostringstream buffer;
        std::streambuf* orig = std::cout.rdbuf(buffer.rdbuf());
        
        stats.printStats(statistics);
        
        std::cout.rdbuf(orig);
        std::string output = buffer.str();
        
        REQUIRE(output.find("로그 분석 결과") != std::string::npos);
        REQUIRE(output.find("/test/log.txt") != std::string::npos);
        REQUIRE(output.find("전체 라인 수: 2") != std::string::npos);
    }
}

TEST_CASE("LogStats 레벨별 엔트리 출력 테스트", "[LogStats]") {
    LogStats stats;
    
    std::vector<LogEntry> entries = {
        LogEntry("First error", LogLevel::ERROR),
        LogEntry("Second error", LogLevel::ERROR),
        LogEntry("Warning message", LogLevel::WARNING),
        LogEntry("Info message", LogLevel::INFO)
    };
    
    SECTION("ERROR 레벨 엔트리 출력") {
        std::ostringstream buffer;
        std::streambuf* orig = std::cout.rdbuf(buffer.rdbuf());
        
        stats.printEntriesByLevel(entries, LogLevel::ERROR);
        
        std::cout.rdbuf(orig);
        std::string output = buffer.str();
        
        REQUIRE(output.find("ERROR 로그 엔트리") != std::string::npos);
        REQUIRE(output.find("First error") != std::string::npos);
        REQUIRE(output.find("Second error") != std::string::npos);
        REQUIRE(output.find("총 2개의") != std::string::npos);
    }
    
    SECTION("존재하지 않는 레벨 출력") {
        std::ostringstream buffer;
        std::streambuf* orig = std::cout.rdbuf(buffer.rdbuf());
        
        stats.printEntriesByLevel(entries, LogLevel::DEBUG);
        
        std::cout.rdbuf(orig);
        std::string output = buffer.str();
        
        REQUIRE(output.find("해당 레벨의 로그가 없습니다") != std::string::npos);
    }
}

TEST_CASE("LogStats 키워드 매칭 출력 테스트", "[LogStats]") {
    LogStats stats;
    
    std::vector<LogEntry> entries = {
        LogEntry("Database connection failed", LogLevel::ERROR),
        LogEntry("Database query executed", LogLevel::INFO),
        LogEntry("Memory usage high", LogLevel::WARNING),
        LogEntry("Database backup completed", LogLevel::INFO)
    };
    
    SECTION("키워드 매칭 결과 출력") {
        std::ostringstream buffer;
        std::streambuf* orig = std::cout.rdbuf(buffer.rdbuf());
        
        stats.printKeywordMatches(entries, "Database");
        
        std::cout.rdbuf(orig);
        std::string output = buffer.str();
        
        REQUIRE(output.find("키워드 'Database' 검색 결과") != std::string::npos);
        REQUIRE(output.find("Database connection failed") != std::string::npos);
        REQUIRE(output.find("Database query executed") != std::string::npos);
        REQUIRE(output.find("Database backup completed") != std::string::npos);
        REQUIRE(output.find("총 3개의") != std::string::npos);
    }
    
    SECTION("매칭되지 않는 키워드") {
        std::ostringstream buffer;
        std::streambuf* orig = std::cout.rdbuf(buffer.rdbuf());
        
        stats.printKeywordMatches(entries, "NonExistent");
        
        std::cout.rdbuf(orig);
        std::string output = buffer.str();
        
        REQUIRE(output.find("키워드를 포함한 로그가 없습니다") != std::string::npos);
    }
} 