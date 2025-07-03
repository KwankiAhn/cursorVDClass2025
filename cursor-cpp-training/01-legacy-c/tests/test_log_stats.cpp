#include <catch2/catch_test_macros.hpp>
#include "../LogStats.hpp"
#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>

TEST_CASE("LogStats - 기본 기능", "[LogStats]")
{
    LogStats stats;
    
    SECTION("초기 상태")
    {
        const auto& statistics = stats.getStatistics();
        REQUIRE(statistics.totalLines == 0);
        REQUIRE(statistics.levelCounts.at("ERROR") == 0);
        REQUIRE(statistics.levelCounts.at("WARNING") == 0);
        REQUIRE(statistics.levelCounts.at("INFO") == 0);
        REQUIRE(statistics.levelCounts.at("DEBUG") == 0);
        REQUIRE(statistics.levelCounts.at("UNKNOWN") == 0);
    }
    
    SECTION("단일 엔트리 추가")
    {
        LogEntry entry("2024-01-01 10:00:00", "ERROR", "Test error message", "Full log line");
        stats.addEntry(entry);
        
        const auto& statistics = stats.getStatistics();
        REQUIRE(statistics.totalLines == 1);
        REQUIRE(statistics.levelCounts.at("ERROR") == 1);
        REQUIRE(statistics.errorEntries.size() == 1);
        REQUIRE(statistics.errorEntries[0].level == "ERROR");
    }
}

TEST_CASE("LogStats - 로그 레벨 통계", "[LogStats]")
{
    LogStats stats;
    
    // 다양한 레벨의 로그 엔트리 추가
    stats.addEntry(LogEntry("", "ERROR", "Error 1", ""));
    stats.addEntry(LogEntry("", "ERROR", "Error 2", ""));
    stats.addEntry(LogEntry("", "WARNING", "Warning 1", ""));
    stats.addEntry(LogEntry("", "INFO", "Info 1", ""));
    stats.addEntry(LogEntry("", "INFO", "Info 2", ""));
    stats.addEntry(LogEntry("", "INFO", "Info 3", ""));
    stats.addEntry(LogEntry("", "DEBUG", "Debug 1", ""));
    stats.addEntry(LogEntry("", "UNKNOWN", "Unknown 1", ""));
    
    stats.finalize();
    
    SECTION("전체 개수 확인")
    {
        const auto& statistics = stats.getStatistics();
        REQUIRE(statistics.totalLines == 8);
    }
    
    SECTION("레벨별 개수 확인")
    {
        REQUIRE(stats.getCountForLevel("ERROR") == 2);
        REQUIRE(stats.getCountForLevel("WARNING") == 1);
        REQUIRE(stats.getCountForLevel("INFO") == 3);
        REQUIRE(stats.getCountForLevel("DEBUG") == 1);
        REQUIRE(stats.getCountForLevel("UNKNOWN") == 1);
    }
    
    SECTION("비율 계산 확인")
    {
        REQUIRE(stats.getPercentageForLevel("ERROR") == 25.0);        // 2/8 * 100
        REQUIRE(stats.getPercentageForLevel("WARNING") == 12.5);      // 1/8 * 100
        REQUIRE(stats.getPercentageForLevel("INFO") == 37.5);         // 3/8 * 100
        REQUIRE(stats.getPercentageForLevel("DEBUG") == 12.5);        // 1/8 * 100
        REQUIRE(stats.getPercentageForLevel("UNKNOWN") == 12.5);      // 1/8 * 100
    }
    
    SECTION("가장 많은 레벨 확인")
    {
        REQUIRE(stats.getMostCommonLevel() == "INFO");
    }
    
    SECTION("에러 로그 엔트리 확인")
    {
        const auto& errorEntries = stats.getErrorEntries();
        REQUIRE(errorEntries.size() == 2);
        REQUIRE(errorEntries[0].level == "ERROR");
        REQUIRE(errorEntries[1].level == "ERROR");
    }
}

TEST_CASE("LogStats - 키워드 검색", "[LogStats]")
{
    LogStats stats;
    
    SECTION("키워드 매칭")
    {
        LogEntry entry1("", "ERROR", "Database connection failed", "ERROR: Database connection failed");
        LogEntry entry2("", "INFO", "User logged in", "INFO: User logged in");
        LogEntry entry3("", "WARNING", "Database timeout", "WARNING: Database timeout");
        
        stats.addEntryWithKeyword(entry1, "Database");
        stats.addEntryWithKeyword(entry2, "Database");
        stats.addEntryWithKeyword(entry3, "Database");
        
        const auto& keywordMatches = stats.getKeywordMatches();
        REQUIRE(keywordMatches.size() == 2);  // entry1과 entry3만 매칭
        
        const auto& statistics = stats.getStatistics();
        REQUIRE(statistics.keywordCounts.at("Database") == 2);
    }
    
    SECTION("빈 키워드")
    {
        LogEntry entry("", "INFO", "Test message", "INFO: Test message");
        stats.addEntryWithKeyword(entry, "");
        
        const auto& keywordMatches = stats.getKeywordMatches();
        REQUIRE(keywordMatches.size() == 0);
    }
}

TEST_CASE("LogStats - 통계 초기화", "[LogStats]")
{
    LogStats stats;
    
    // 데이터 추가
    stats.addEntry(LogEntry("", "ERROR", "Test error", ""));
    stats.addEntry(LogEntry("", "INFO", "Test info", ""));
    
    REQUIRE(stats.getStatistics().totalLines == 2);
    
    // 초기화
    stats.reset();
    
    SECTION("초기화 후 상태 확인")
    {
        const auto& statistics = stats.getStatistics();
        REQUIRE(statistics.totalLines == 0);
        REQUIRE(statistics.levelCounts.at("ERROR") == 0);
        REQUIRE(statistics.levelCounts.at("INFO") == 0);
        REQUIRE(statistics.errorEntries.size() == 0);
        REQUIRE(statistics.keywordMatches.size() == 0);
    }
}

TEST_CASE("LogStats - 처리 시간 설정", "[LogStats]")
{
    LogStats stats;
    
    SECTION("처리 시간 설정")
    {
        stats.setProcessingTime(123.45);
        
        const auto& statistics = stats.getStatistics();
        REQUIRE(statistics.processingTimeMs == 123.45);
    }
}

TEST_CASE("LogStats - 빈 통계", "[LogStats]")
{
    LogStats stats;
    
    SECTION("빈 상태에서 비율 계산")
    {
        REQUIRE(stats.getPercentageForLevel("ERROR") == 0.0);
        REQUIRE(stats.getPercentageForLevel("INFO") == 0.0);
    }
    
    SECTION("존재하지 않는 레벨")
    {
        REQUIRE(stats.getCountForLevel("NONEXISTENT") == 0);
        REQUIRE(stats.getPercentageForLevel("NONEXISTENT") == 0.0);
    }
}

TEST_CASE("LogStats - JSON 변환 테스트", "[LogStats]")
{
    LogStats stats;
    
    SECTION("빈 통계 JSON 변환")
    {
        stats.finalize();
        std::string jsonStr = stats.toJson();
        
        // JSON 파싱 가능한지 확인
        REQUIRE_NOTHROW(nlohmann::json::parse(jsonStr));
        
        auto json = nlohmann::json::parse(jsonStr);
        
        // 필수 섹션들이 존재하는지 확인
        REQUIRE(json.contains("summary"));
        REQUIRE(json.contains("levelCounts"));
        REQUIRE(json.contains("keywordCounts"));
        REQUIRE(json.contains("errorEntries"));
        REQUIRE(json.contains("keywordMatches"));
        
        // 기본 값 확인
        REQUIRE(json["summary"]["totalLines"] == 0);
        REQUIRE(json["errorEntries"].is_array());
        REQUIRE(json["keywordMatches"].is_array());
    }
    
    SECTION("데이터가 있는 통계 JSON 변환")
    {
        // 테스트 데이터 추가
        stats.addEntry(LogEntry("2024-01-01 10:00:00", "ERROR", "Database error", "ERROR: Database error"));
        stats.addEntry(LogEntry("2024-01-01 10:01:00", "INFO", "Server started", "INFO: Server started"));
        stats.addEntry(LogEntry("2024-01-01 10:02:00", "WARNING", "High memory usage", "WARNING: High memory usage"));
        stats.setProcessingTime(123.45);
        stats.finalize();
        
        std::string jsonStr = stats.toJson();
        auto json = nlohmann::json::parse(jsonStr);
        
        // 요약 정보 확인
        REQUIRE(json["summary"]["totalLines"] == 3);
        REQUIRE(json["summary"]["processingTimeMs"] == 123.45);
        REQUIRE(json["summary"]["mostCommonLevel"].is_string());
        
        // 레벨별 개수 확인
        REQUIRE(json["levelCounts"]["ERROR"]["count"] == 1);
        REQUIRE(json["levelCounts"]["INFO"]["count"] == 1);
        REQUIRE(json["levelCounts"]["WARNING"]["count"] == 1);
        
        // 백분율 확인
        REQUIRE(json["levelCounts"]["ERROR"]["percentage"].is_number());
        REQUIRE(json["levelCounts"]["INFO"]["percentage"].is_number());
        REQUIRE(json["levelCounts"]["WARNING"]["percentage"].is_number());
        
        // 에러 엔트리 확인
        REQUIRE(json["errorEntries"].size() == 1);
        REQUIRE(json["errorEntries"][0]["level"] == "ERROR");
        REQUIRE(json["errorEntries"][0]["message"] == "Database error");
    }
    
    SECTION("키워드 검색 결과 JSON 변환")
    {
        LogEntry entry1("2024-01-01 10:00:00", "ERROR", "Database connection failed", "ERROR: Database connection failed");
        LogEntry entry2("2024-01-01 10:01:00", "INFO", "User login successful", "INFO: User login successful");
        LogEntry entry3("2024-01-01 10:02:00", "WARNING", "Database timeout", "WARNING: Database timeout");
        
        stats.addEntryWithKeyword(entry1, "Database");
        stats.addEntryWithKeyword(entry2, "Database");
        stats.addEntryWithKeyword(entry3, "Database");
        stats.finalize();
        
        std::string jsonStr = stats.toJson();
        auto json = nlohmann::json::parse(jsonStr);
        
        // 키워드 카운트 확인
        REQUIRE(json["keywordCounts"]["Database"] == 2);
        
        // 키워드 매칭 엔트리 확인
        REQUIRE(json["keywordMatches"].size() == 2);
        REQUIRE(json["keywordMatches"][0]["level"] == "ERROR");
        REQUIRE(json["keywordMatches"][1]["level"] == "WARNING");
    }
    
    SECTION("특수 문자 이스케이프 처리")
    {
        LogEntry entry("2024-01-01 10:00:00", "ERROR", "Message with \"quotes\" and \\backslashes", "ERROR: Message with \"quotes\" and \\backslashes");
        stats.addEntry(entry);
        stats.finalize();
        
        std::string jsonStr = stats.toJson();
        
        // JSON 파싱이 성공해야 함 (특수 문자가 올바르게 이스케이프됨)
        REQUIRE_NOTHROW(nlohmann::json::parse(jsonStr));
        
        auto json = nlohmann::json::parse(jsonStr);
        REQUIRE(json["errorEntries"][0]["message"] == "Message with \"quotes\" and \\backslashes");
    }
}

TEST_CASE("LogStats - JSON 파일 저장", "[LogStats]")
{
    LogStats stats;
    
    SECTION("기본 JSON 파일 저장")
    {
        std::string testFile = "test_stats.json";
        
        // 테스트 데이터 추가
        stats.addEntry(LogEntry("2024-01-01 10:00:00", "ERROR", "Test error", "ERROR: Test error"));
        stats.addEntry(LogEntry("2024-01-01 10:01:00", "INFO", "Test info", "INFO: Test info"));
        stats.setProcessingTime(50.25);
        stats.finalize();
        
        // JSON 파일 저장
        bool saveResult = stats.saveToJsonFile(testFile);
        REQUIRE(saveResult == true);
        
        // 파일 존재 확인
        REQUIRE(std::filesystem::exists(testFile));
        
        // 파일 내용 확인
        std::ifstream file(testFile);
        REQUIRE(file.is_open());
        
        std::ostringstream buffer;
        buffer << file.rdbuf();
        std::string fileContent = buffer.str();
        file.close();
        
        // JSON 파싱 가능한지 확인
        REQUIRE_NOTHROW(nlohmann::json::parse(fileContent));
        
        auto json = nlohmann::json::parse(fileContent);
        REQUIRE(json["summary"]["totalLines"] == 2);
        REQUIRE(json["summary"]["processingTimeMs"] == 50.25);
        
        // 임시 파일 삭제
        std::filesystem::remove(testFile);
    }
    
    SECTION("빈 통계 JSON 파일 저장")
    {
        std::string testFile = "test_empty_stats.json";
        
        stats.finalize();
        bool saveResult = stats.saveToJsonFile(testFile);
        REQUIRE(saveResult == true);
        
        // 파일 내용 확인
        std::ifstream file(testFile);
        std::ostringstream buffer;
        buffer << file.rdbuf();
        std::string fileContent = buffer.str();
        file.close();
        
        auto json = nlohmann::json::parse(fileContent);
        REQUIRE(json["summary"]["totalLines"] == 0);
        REQUIRE(json["errorEntries"].is_array());
        REQUIRE(json["errorEntries"].size() == 0);
        
        // 임시 파일 삭제
        std::filesystem::remove(testFile);
    }
    
    SECTION("파일 저장 실패 처리")
    {
        // 존재하지 않는 디렉토리에 저장 시도
        std::string invalidPath = "/nonexistent_directory/test.json";
        
        stats.finalize();
        bool saveResult = stats.saveToJsonFile(invalidPath);
        REQUIRE(saveResult == false);
    }
}

TEST_CASE("LogStats - 콘솔 출력 테스트", "[LogStats]")
{
    LogStats stats;
    
    SECTION("printStatistics 함수 호출 테스트")
    {
        // 테스트 데이터 추가
        stats.addEntry(LogEntry("2024-01-01 10:00:00", "ERROR", "Test error", "ERROR: Test error"));
        stats.addEntry(LogEntry("2024-01-01 10:01:00", "INFO", "Test info", "INFO: Test info"));
        stats.addEntry(LogEntry("2024-01-01 10:02:00", "WARNING", "Test warning", "WARNING: Test warning"));
        stats.setProcessingTime(75.5);
        stats.finalize();
        
        // printStatistics 함수가 예외 없이 실행되는지 확인
        REQUIRE_NOTHROW(stats.printStatistics());
    }
    
    SECTION("빈 통계 출력 테스트")
    {
        stats.finalize();
        
        // 빈 통계에서도 예외 없이 실행되는지 확인
        REQUIRE_NOTHROW(stats.printStatistics());
    }
}

TEST_CASE("LogStats - 에러 처리 및 경계 케이스", "[LogStats][ErrorHandling]")
{
    LogStats stats;
    
    SECTION("매우 많은 엔트리 추가")
    {
        const int numEntries = 10000;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < numEntries; ++i)
        {
            std::string level = (i % 4 == 0) ? "ERROR" : 
                               (i % 4 == 1) ? "WARNING" :
                               (i % 4 == 2) ? "INFO" : "DEBUG";
            
            LogEntry entry("2024-01-01 10:00:00", level, "메시지 " + std::to_string(i), "로그 라인 " + std::to_string(i));
            stats.addEntry(entry);
        }
        
        stats.finalize();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        // 10000개 엔트리 처리가 1초 이내에 완료되어야 함
        REQUIRE(duration.count() < 1000);
        
        const auto& statistics = stats.getStatistics();
        REQUIRE(statistics.totalLines == numEntries);
        REQUIRE(statistics.levelCounts.at("ERROR") == 2500);
        REQUIRE(statistics.levelCounts.at("WARNING") == 2500);
        REQUIRE(statistics.levelCounts.at("INFO") == 2500);
        REQUIRE(statistics.levelCounts.at("DEBUG") == 2500);
    }
    
    SECTION("극단적으로 긴 메시지 처리")
    {
        std::string longMessage(100000, 'A');  // 100KB 메시지
        LogEntry entry("2024-01-01 10:00:00", "ERROR", longMessage, "ERROR: " + longMessage);
        
        REQUIRE_NOTHROW(stats.addEntry(entry));
        stats.finalize();
        
        const auto& errorEntries = stats.getErrorEntries();
        REQUIRE(errorEntries.size() == 1);
        REQUIRE(errorEntries[0].message.size() == 100000);
        
        // JSON 변환도 문제없이 수행되어야 함
        REQUIRE_NOTHROW(stats.toJson());
    }
    
    SECTION("특수 문자가 포함된 메시지")
    {
        std::vector<LogEntry> specialEntries = {
            LogEntry("2024-01-01 10:00:00", "ERROR", "메시지에 \"따옴표\" 포함", "ERROR: 메시지에 \"따옴표\" 포함"),
            LogEntry("2024-01-01 10:00:01", "WARNING", "메시지에 \\백슬래시\\ 포함", "WARNING: 메시지에 \\백슬래시\\ 포함"),
            LogEntry("2024-01-01 10:00:02", "INFO", "메시지에 \n개행\n문자 포함", "INFO: 메시지에 \n개행\n문자 포함"),
            LogEntry("2024-01-01 10:00:03", "DEBUG", "메시지에 \t탭\t문자 포함", "DEBUG: 메시지에 \t탭\t문자 포함"),
            LogEntry("2024-01-01 10:00:04", "ERROR", "메시지에 🔥 이모지 포함", "ERROR: 메시지에 🔥 이모지 포함")
        };
        
        for (const auto& entry : specialEntries)
        {
            REQUIRE_NOTHROW(stats.addEntry(entry));
        }
        
        stats.finalize();
        
        // JSON 변환이 문제없이 수행되어야 함
        std::string jsonStr;
        REQUIRE_NOTHROW(jsonStr = stats.toJson());
        
        // JSON 파싱이 성공해야 함
        REQUIRE_NOTHROW(nlohmann::json::parse(jsonStr));
        
        auto json = nlohmann::json::parse(jsonStr);
        REQUIRE(json["summary"]["totalLines"] == 5);
        REQUIRE(json["errorEntries"].size() == 2);
    }
    
    SECTION("null 문자 포함 메시지 처리")
    {
        std::string messageWithNull = "메시지 앞부분";
        messageWithNull += '\0';
        messageWithNull += "메시지 뒷부분";
        
        LogEntry entry("2024-01-01 10:00:00", "ERROR", messageWithNull, "ERROR: " + messageWithNull);
        
        REQUIRE_NOTHROW(stats.addEntry(entry));
        stats.finalize();
        
        // JSON 변환이 문제없이 수행되어야 함
        REQUIRE_NOTHROW(stats.toJson());
    }
    
    SECTION("동일한 키워드 대량 매칭")
    {
        const int numEntries = 1000;
        
        for (int i = 0; i < numEntries; ++i)
        {
            LogEntry entry("2024-01-01 10:00:00", "INFO", "Database operation " + std::to_string(i), "INFO: Database operation " + std::to_string(i));
            stats.addEntryWithKeyword(entry, "Database");
        }
        
        stats.finalize();
        
        const auto& statistics = stats.getStatistics();
        REQUIRE(statistics.keywordCounts.at("Database") == numEntries);
        REQUIRE(statistics.keywordMatches.size() == numEntries);
    }
    
    SECTION("빈 또는 유효하지 않은 파일 경로로 JSON 저장")
    {
        stats.addEntry(LogEntry("2024-01-01 10:00:00", "INFO", "테스트", "INFO: 테스트"));
        stats.finalize();
        
        // 빈 파일명
        REQUIRE(stats.saveToJsonFile("") == false);
        
        // 유효하지 않은 문자가 포함된 파일명
        #ifdef _WIN32
        REQUIRE(stats.saveToJsonFile("test<>:\"|?*.json") == false);
        #else
        REQUIRE(stats.saveToJsonFile("/dev/null/invalid.json") == false);
        #endif
        
        // 읽기 전용 디렉토리 (권한이 있는 경우에만 테스트)
        #ifndef _WIN32
        std::string readonlyDir = "/tmp/readonly_test_dir";
        if (std::filesystem::create_directory(readonlyDir))
        {
            std::filesystem::permissions(readonlyDir, std::filesystem::perms::owner_read | std::filesystem::perms::owner_exec);
            
            bool saveResult = stats.saveToJsonFile(readonlyDir + "/test.json");
            REQUIRE(saveResult == false);
            
            // 권한 복원 후 삭제
            std::filesystem::permissions(readonlyDir, std::filesystem::perms::owner_all);
            std::filesystem::remove_all(readonlyDir);
        }
        #endif
    }
}

TEST_CASE("LogStats - 메모리 및 성능 스트레스 테스트", "[LogStats][Performance]")
{
    LogStats stats;
    
    SECTION("대용량 키워드 검색 성능")
    {
        const int numKeywords = 100;
        const int entriesPerKeyword = 100;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // 다양한 키워드로 대량 엔트리 추가
        for (int i = 0; i < numKeywords; ++i)
        {
            std::string keyword = "Keyword" + std::to_string(i);
            
            for (int j = 0; j < entriesPerKeyword; ++j)
            {
                LogEntry entry("2024-01-01 10:00:00", "INFO", 
                              keyword + " message " + std::to_string(j),
                              "INFO: " + keyword + " message " + std::to_string(j));
                stats.addEntryWithKeyword(entry, keyword);
            }
        }
        
        stats.finalize();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        // 10000개 엔트리 처리가 2초 이내에 완료되어야 함
        REQUIRE(duration.count() < 2000);
        
        const auto& statistics = stats.getStatistics();
        REQUIRE(statistics.totalLines == numKeywords * entriesPerKeyword);
        REQUIRE(statistics.keywordCounts.size() == numKeywords);
    }
    
    SECTION("JSON 변환 성능 테스트")
    {
        // 대량 데이터 준비
        for (int i = 0; i < 1000; ++i)
        {
            LogEntry entry("2024-01-01 10:00:00", "ERROR", 
                          "Error message " + std::to_string(i),
                          "ERROR: Error message " + std::to_string(i));
            stats.addEntry(entry);
        }
        
        stats.finalize();
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // JSON 변환 10번 수행
        for (int i = 0; i < 10; ++i)
        {
            std::string jsonStr = stats.toJson();
            REQUIRE(!jsonStr.empty());
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        // 10번 JSON 변환이 1초 이내에 완료되어야 함
        REQUIRE(duration.count() < 1000);
    }
    
    SECTION("메모리 효율성 테스트")
    {
        // 동일한 메시지를 가진 엔트리들을 대량 추가
        const std::string commonMessage = "동일한 메시지 내용";
        const int numEntries = 5000;
        
        for (int i = 0; i < numEntries; ++i)
        {
            LogEntry entry("2024-01-01 10:00:00", "INFO", commonMessage, "INFO: " + commonMessage);
            stats.addEntry(entry);
        }
        
        stats.finalize();
        
        // 통계가 올바르게 계산되었는지 확인
        const auto& statistics = stats.getStatistics();
        REQUIRE(statistics.totalLines == numEntries);
        REQUIRE(statistics.levelCounts.at("INFO") == numEntries);
        
        // JSON 변환이 문제없이 수행되어야 함
        REQUIRE_NOTHROW(stats.toJson());
    }
} 