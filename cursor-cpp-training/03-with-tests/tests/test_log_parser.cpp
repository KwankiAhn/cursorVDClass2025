#include <catch2/catch_test_macros.hpp>
#include "../LogParser.hpp"

using namespace LogAnalyzer;

TEST_CASE("LogParser 기본 기능 테스트", "[LogParser]") {
    LogParser parser;
    
    SECTION("로그 레벨 감지 테스트") {
        SECTION("ERROR 레벨 감지") {
            std::string errorLine = "2023-12-01 10:30:15 ERROR Database connection failed";
            auto entry = parser.parseLine(errorLine);
            REQUIRE(entry.level == LogLevel::ERROR);
        }
        
        SECTION("WARNING 레벨 감지") {
            std::string warnLine = "2023-12-01 10:30:15 WARN Memory usage high";
            auto entry = parser.parseLine(warnLine);
            REQUIRE(entry.level == LogLevel::WARNING);
        }
        
        SECTION("INFO 레벨 감지") {
            std::string infoLine = "2023-12-01 10:30:15 INFO Application started";
            auto entry = parser.parseLine(infoLine);
            REQUIRE(entry.level == LogLevel::INFO);
        }
        
        SECTION("UNKNOWN 레벨 처리") {
            std::string unknownLine = "2023-12-01 10:30:15 Some random log";
            auto entry = parser.parseLine(unknownLine);
            REQUIRE(entry.level == LogLevel::UNKNOWN);
        }
    }
    
    SECTION("빈 라인 처리") {
        std::string emptyLine = "";
        auto entry = parser.parseLine(emptyLine);
        REQUIRE(entry.level == LogLevel::UNKNOWN);
        REQUIRE(entry.originalLine.empty());
    }
}

TEST_CASE("LogParser 여러 라인 파싱 테스트", "[LogParser]") {
    LogParser parser;
    
    std::vector<std::string> testLines = {
        "2023-12-01 10:30:15 ERROR Database connection failed",
        "2023-12-01 10:30:16 WARN Memory usage high",
        "2023-12-01 10:30:17 INFO Application started",
        "2023-12-01 10:30:18 Some random log"
    };
    
    auto entries = parser.parseLines(testLines);
    
    REQUIRE(entries.size() == 4);
    REQUIRE(entries[0].level == LogLevel::ERROR);
    REQUIRE(entries[1].level == LogLevel::WARNING);
    REQUIRE(entries[2].level == LogLevel::INFO);
    REQUIRE(entries[3].level == LogLevel::UNKNOWN);
}

TEST_CASE("LogParser 키워드 필터링 테스트", "[LogParser]") {
    LogParser parser;
    
    std::vector<LogEntry> entries = {
        LogEntry("Database connection failed", LogLevel::ERROR),
        LogEntry("Memory usage high", LogLevel::WARNING),
        LogEntry("Database query executed", LogLevel::INFO),
        LogEntry("Application started", LogLevel::INFO)
    };
    
    SECTION("키워드 매칭 테스트") {
        auto filtered = parser.filterByKeyword(entries, "Database");
        REQUIRE(filtered.size() == 2);
        REQUIRE(filtered[0].originalLine.find("Database") != std::string::npos);
        REQUIRE(filtered[1].originalLine.find("Database") != std::string::npos);
    }
    
    SECTION("빈 키워드 처리") {
        auto filtered = parser.filterByKeyword(entries, "");
        REQUIRE(filtered.size() == entries.size());
    }
    
    SECTION("매칭되지 않는 키워드") {
        auto filtered = parser.filterByKeyword(entries, "NonExistent");
        REQUIRE(filtered.empty());
    }
}

TEST_CASE("LogParser 레벨 필터링 테스트", "[LogParser]") {
    LogParser parser;
    
    std::vector<LogEntry> entries = {
        LogEntry("Error message", LogLevel::ERROR),
        LogEntry("Warning message", LogLevel::WARNING),
        LogEntry("Info message 1", LogLevel::INFO),
        LogEntry("Info message 2", LogLevel::INFO)
    };
    
    SECTION("ERROR 레벨 필터링") {
        auto filtered = parser.filterByLevel(entries, LogLevel::ERROR);
        REQUIRE(filtered.size() == 1);
        REQUIRE(filtered[0].level == LogLevel::ERROR);
    }
    
    SECTION("INFO 레벨 필터링") {
        auto filtered = parser.filterByLevel(entries, LogLevel::INFO);
        REQUIRE(filtered.size() == 2);
        for (const auto& entry : filtered) {
            REQUIRE(entry.level == LogLevel::INFO);
        }
    }
    
    SECTION("존재하지 않는 레벨 필터링") {
        auto filtered = parser.filterByLevel(entries, LogLevel::DEBUG);
        REQUIRE(filtered.empty());
    }
}

TEST_CASE("LogParser 문자열 변환 테스트", "[LogParser]") {
    SECTION("LogLevel to String") {
        REQUIRE(LogParser::logLevelToString(LogLevel::ERROR) == "ERROR");
        REQUIRE(LogParser::logLevelToString(LogLevel::WARNING) == "WARNING");
        REQUIRE(LogParser::logLevelToString(LogLevel::INFO) == "INFO");
        REQUIRE(LogParser::logLevelToString(LogLevel::DEBUG) == "DEBUG");
        REQUIRE(LogParser::logLevelToString(LogLevel::UNKNOWN) == "UNKNOWN");
    }
    
    SECTION("String to LogLevel") {
        REQUIRE(LogParser::stringToLogLevel("ERROR") == LogLevel::ERROR);
        REQUIRE(LogParser::stringToLogLevel("error") == LogLevel::ERROR);
        REQUIRE(LogParser::stringToLogLevel("ERR") == LogLevel::ERROR);
        REQUIRE(LogParser::stringToLogLevel("WARNING") == LogLevel::WARNING);
        REQUIRE(LogParser::stringToLogLevel("WARN") == LogLevel::WARNING);
        REQUIRE(LogParser::stringToLogLevel("INFO") == LogLevel::INFO);
        REQUIRE(LogParser::stringToLogLevel("DEBUG") == LogLevel::DEBUG);
        REQUIRE(LogParser::stringToLogLevel("INVALID") == LogLevel::UNKNOWN);
    }
} 