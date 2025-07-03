#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "LogStats.hpp"
#include "LogEntry.hpp"

TEST_CASE("LogStats 기본 통계 계산", "[LogStats]")
{
    LogStats stats;
    
    std::vector<LogEntry> testEntries = {
        LogEntry("2023-12-01 09:00:00", LogLevel::INFO, "Application started", 1),
        LogEntry("2023-12-01 09:00:01", LogLevel::INFO, "Loading configuration", 2),
        LogEntry("2023-12-01 09:00:02", LogLevel::ERROR, "Database connection failed", 3),
        LogEntry("2023-12-01 09:00:03", LogLevel::WARN, "Memory usage high", 4),
        LogEntry("2023-12-01 09:00:04", LogLevel::ERROR, "Disk space low", 5),
        LogEntry("2023-12-01 09:00:05", LogLevel::INFO, "User login successful", 6),
        LogEntry("2023-12-01 09:00:06", LogLevel::DEBUG, "Debug information", 7),
        LogEntry("2023-12-01 09:00:07", LogLevel::WARN, "Configuration warning", 8),
        LogEntry("2023-12-01 09:00:08", LogLevel::INFO, "Processing request", 9),
        LogEntry("2023-12-01 09:00:09", LogLevel::ERROR, "Network timeout", 10)
    };
    
    stats.calculateStatistics(testEntries);
    const auto& result = stats.getStatistics();
    
    SECTION("기본 카운팅")
    {
        REQUIRE(result.totalEntries == 10);
        REQUIRE(result.errorCount == 3);
        REQUIRE(result.warnCount == 2);
        REQUIRE(result.infoCount == 4);
        REQUIRE(result.debugCount == 1);
        REQUIRE(result.unknownCount == 0);
    }
    
    SECTION("비율 계산")
    {
        REQUIRE(result.errorPercentage == Catch::Approx(30.0).epsilon(0.1));
        REQUIRE(result.warnPercentage == Catch::Approx(20.0).epsilon(0.1));
        REQUIRE(result.infoPercentage == Catch::Approx(40.0).epsilon(0.1));
        REQUIRE(result.debugPercentage == Catch::Approx(10.0).epsilon(0.1));
        REQUIRE(result.unknownPercentage == Catch::Approx(0.0).epsilon(0.1));
    }
    
    SECTION("시간 정보")
    {
        REQUIRE(result.analysisTimeSec >= 0.0);
        REQUIRE_FALSE(result.analysisStartTime.empty());
        REQUIRE_FALSE(result.analysisEndTime.empty());
    }
}

TEST_CASE("LogStats 개별 조회 함수", "[LogStats][query]")
{
    LogStats stats;
    
    std::vector<LogEntry> testEntries = {
        LogEntry("2023-12-01 09:00:00", LogLevel::ERROR, "Error 1", 1),
        LogEntry("2023-12-01 09:00:01", LogLevel::ERROR, "Error 2", 2),
        LogEntry("2023-12-01 09:00:02", LogLevel::WARN, "Warning 1", 3),
        LogEntry("2023-12-01 09:00:03", LogLevel::INFO, "Info 1", 4),
        LogEntry("2023-12-01 09:00:04", LogLevel::INFO, "Info 2", 5)
    };
    
    stats.calculateStatistics(testEntries);
    
    SECTION("레벨별 개수 조회")
    {
        REQUIRE(stats.getCountByLevel(LogLevel::ERROR) == 2);
        REQUIRE(stats.getCountByLevel(LogLevel::WARN) == 1);
        REQUIRE(stats.getCountByLevel(LogLevel::INFO) == 2);
        REQUIRE(stats.getCountByLevel(LogLevel::DEBUG) == 0);
        REQUIRE(stats.getCountByLevel(LogLevel::UNKNOWN) == 0);
    }
    
    SECTION("레벨별 비율 조회")
    {
        REQUIRE(stats.getPercentageByLevel(LogLevel::ERROR) == Catch::Approx(40.0).epsilon(0.1));
        REQUIRE(stats.getPercentageByLevel(LogLevel::WARN) == Catch::Approx(20.0).epsilon(0.1));
        REQUIRE(stats.getPercentageByLevel(LogLevel::INFO) == Catch::Approx(40.0).epsilon(0.1));
        REQUIRE(stats.getPercentageByLevel(LogLevel::DEBUG) == Catch::Approx(0.0).epsilon(0.1));
    }
    
    SECTION("총 개수 및 문제 개수")
    {
        REQUIRE(stats.getTotalCount() == 5);
        REQUIRE(stats.getIssueCount() == 3); // ERROR(2) + WARN(1)
    }
}

TEST_CASE("LogStats 심각한 문제 감지", "[LogStats][critical]")
{
    LogStats stats;
    
    SECTION("ERROR 비율이 낮은 경우 (정상)")
    {
        std::vector<LogEntry> normalEntries = {
            LogEntry("2023-12-01 09:00:00", LogLevel::INFO, "Info 1", 1),
            LogEntry("2023-12-01 09:00:01", LogLevel::INFO, "Info 2", 2),
            LogEntry("2023-12-01 09:00:02", LogLevel::INFO, "Info 3", 3),
            LogEntry("2023-12-01 09:00:03", LogLevel::WARN, "Warning 1", 4),
            LogEntry("2023-12-01 09:00:04", LogLevel::ERROR, "Error 1", 5) // 20% ERROR
        };
        
        stats.calculateStatistics(normalEntries);
        REQUIRE(stats.hasCriticalIssues()); // 20%이므로 기본값 10% 초과
        REQUIRE_FALSE(stats.hasCriticalIssues(25.0)); // 25% 임계값으로 테스트
    }
    
    SECTION("ERROR 비율이 높은 경우 (심각)")
    {
        std::vector<LogEntry> criticalEntries = {
            LogEntry("2023-12-01 09:00:00", LogLevel::ERROR, "Error 1", 1),
            LogEntry("2023-12-01 09:00:01", LogLevel::ERROR, "Error 2", 2),
            LogEntry("2023-12-01 09:00:02", LogLevel::ERROR, "Error 3", 3),
            LogEntry("2023-12-01 09:00:03", LogLevel::INFO, "Info 1", 4),
            LogEntry("2023-12-01 09:00:04", LogLevel::INFO, "Info 2", 5) // 60% ERROR
        };
        
        stats.calculateStatistics(criticalEntries);
        REQUIRE(stats.hasCriticalIssues()); // 기본값 10% 초과
        REQUIRE(stats.hasCriticalIssues(50.0)); // 50% 임계값도 초과
        REQUIRE_FALSE(stats.hasCriticalIssues(70.0)); // 70% 임계값은 초과하지 않음
    }
}

TEST_CASE("LogStats 빈 데이터 처리", "[LogStats][empty]")
{
    LogStats stats;
    
    SECTION("빈 엔트리 벡터")
    {
        std::vector<LogEntry> emptyEntries;
        stats.calculateStatistics(emptyEntries);
        
        const auto& result = stats.getStatistics();
        REQUIRE(result.totalEntries == 0);
        REQUIRE(result.errorCount == 0);
        REQUIRE(result.warnCount == 0);
        REQUIRE(result.infoCount == 0);
        REQUIRE(result.debugCount == 0);
        REQUIRE(result.unknownCount == 0);
        
        // 비율은 모두 0이어야 함
        REQUIRE(result.errorPercentage == 0.0);
        REQUIRE(result.warnPercentage == 0.0);
        REQUIRE(result.infoPercentage == 0.0);
        REQUIRE(result.debugPercentage == 0.0);
        REQUIRE(result.unknownPercentage == 0.0);
        
        REQUIRE(stats.getTotalCount() == 0);
        REQUIRE(stats.getIssueCount() == 0);
        REQUIRE_FALSE(stats.hasCriticalIssues());
    }
}

TEST_CASE("LogStats 출력 기능", "[LogStats][output]")
{
    LogStats stats;
    
    std::vector<LogEntry> testEntries = {
        LogEntry("2023-12-01 09:00:00", LogLevel::ERROR, "Database connection failed", 1),
        LogEntry("2023-12-01 09:00:01", LogLevel::WARN, "Memory usage high", 2),
        LogEntry("2023-12-01 09:00:02", LogLevel::INFO, "User login successful", 3)
    };
    
    stats.calculateStatistics(testEntries);
    
    SECTION("기본 통계 문자열")
    {
        std::string basicStats = stats.getStatisticsString(false);
        REQUIRE_FALSE(basicStats.empty());
        REQUIRE(basicStats.find("전체 라인 수: 3") != std::string::npos);
        REQUIRE(basicStats.find("ERROR 개수: 1") != std::string::npos);
        REQUIRE(basicStats.find("WARNING 개수: 1") != std::string::npos);
        REQUIRE(basicStats.find("INFO 개수: 1") != std::string::npos);
    }
    
    SECTION("상세 통계 문자열")
    {
        std::string detailedStats = stats.getStatisticsString(true);
        REQUIRE_FALSE(detailedStats.empty());
        REQUIRE(detailedStats.find("DEBUG 개수: 0") != std::string::npos);
        REQUIRE(detailedStats.find("UNKNOWN 개수: 0") != std::string::npos);
        REQUIRE(detailedStats.find("분석 소요 시간") != std::string::npos);
        REQUIRE(detailedStats.find("총 문제 개수") != std::string::npos);
    }
    
    SECTION("JSON 출력")
    {
        std::string jsonStats = stats.toJson();
        REQUIRE_FALSE(jsonStats.empty());
        REQUIRE(jsonStats.find("\"totalEntries\": 3") != std::string::npos);
        REQUIRE(jsonStats.find("\"error\": 1") != std::string::npos);
        REQUIRE(jsonStats.find("\"warn\": 1") != std::string::npos);
        REQUIRE(jsonStats.find("\"info\": 1") != std::string::npos);
        REQUIRE(jsonStats.find("\"analysisTime\"") != std::string::npos);
        REQUIRE(jsonStats.find("\"hasCriticalIssues\"") != std::string::npos);
    }
}

TEST_CASE("LogStats 초기화 기능", "[LogStats][reset]")
{
    LogStats stats;
    
    // 데이터로 통계 계산
    std::vector<LogEntry> testEntries = {
        LogEntry("2023-12-01 09:00:00", LogLevel::ERROR, "Error message", 1),
        LogEntry("2023-12-01 09:00:01", LogLevel::INFO, "Info message", 2)
    };
    
    stats.calculateStatistics(testEntries);
    
    // 통계가 계산되었는지 확인
    REQUIRE(stats.getTotalCount() == 2);
    REQUIRE(stats.getCountByLevel(LogLevel::ERROR) == 1);
    
    // 초기화
    stats.reset();
    
    // 초기화 후 모든 값이 0인지 확인
    const auto& result = stats.getStatistics();
    REQUIRE(result.totalEntries == 0);
    REQUIRE(result.errorCount == 0);
    REQUIRE(result.warnCount == 0);
    REQUIRE(result.infoCount == 0);
    REQUIRE(result.debugCount == 0);
    REQUIRE(result.unknownCount == 0);
    REQUIRE(stats.getTotalCount() == 0);
    REQUIRE(stats.getIssueCount() == 0);
} 