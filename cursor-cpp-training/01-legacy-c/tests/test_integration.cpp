#include <catch2/catch_test_macros.hpp>
#include "../LogFileReader.hpp"
#include "../LogParser.hpp"
#include "../LogStats.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>

TEST_CASE("통합 테스트 - End-to-End 워크플로우", "[Integration]")
{
    SECTION("전체 로그 분석 프로세스")
    {
        // 테스트 로그 파일 생성
        std::string testLogFile = "integration_test.log";
        std::ofstream file(testLogFile);
        file << "2024-01-01 10:00:00 INFO: 서버 시작\n";
        file << "2024-01-01 10:00:05 ERROR: 데이터베이스 연결 실패\n";
        file << "2024-01-01 10:00:10 WARNING: 메모리 사용량 높음\n";
        file << "2024-01-01 10:00:15 INFO: 데이터베이스 연결 복구\n";
        file << "2024-01-01 10:00:20 DEBUG: 사용자 로그인 시도\n";
        file << "2024-01-01 10:00:25 ERROR: 권한 없음\n";
        file << "2024-01-01 10:00:30 INFO: 시스템 정상 운영\n";
        file.close();
        
        // 1. LogFileReader로 파일 읽기
        LogFileReader reader(testLogFile);
        REQUIRE(reader.isValid() == true);
        REQUIRE(reader.getFileSize() > 0);
        
        // 2. LogParser로 파싱 및 LogStats로 통계 수집
        LogParser parser;
        LogStats stats;
        
        int lineCount = 0;
        while (auto line = reader.readLine())
        {
            LogEntry entry = parser.parseLine(*line);
            stats.addEntry(entry);
            lineCount++;
        }
        
        stats.finalize();
        
        // 3. 결과 검증
        REQUIRE(lineCount == 7);
        
        const auto& statistics = stats.getStatistics();
        REQUIRE(statistics.totalLines == 7);
        REQUIRE(statistics.levelCounts.at("ERROR") == 2);
        REQUIRE(statistics.levelCounts.at("WARNING") == 1);
        REQUIRE(statistics.levelCounts.at("INFO") == 3);
        REQUIRE(statistics.levelCounts.at("DEBUG") == 1);
        
        // 가장 많은 레벨 확인
        REQUIRE(stats.getMostCommonLevel() == "INFO");
        
        // 에러 엔트리 확인
        const auto& errorEntries = stats.getErrorEntries();
        REQUIRE(errorEntries.size() == 2);
        REQUIRE(errorEntries[0].message.find("데이터베이스") != std::string::npos);
        REQUIRE(errorEntries[1].message.find("권한") != std::string::npos);
        
        // 4. JSON 출력 테스트
        std::string jsonOutputFile = "integration_output.json";
        bool saveResult = stats.saveToJsonFile(jsonOutputFile);
        REQUIRE(saveResult == true);
        REQUIRE(std::filesystem::exists(jsonOutputFile));
        
        // JSON 파일 내용 검증
        std::ifstream jsonFile(jsonOutputFile);
        std::ostringstream buffer;
        buffer << jsonFile.rdbuf();
        jsonFile.close();
        
        auto json = nlohmann::json::parse(buffer.str());
        REQUIRE(json["summary"]["totalLines"] == 7);
        REQUIRE(json["levelCounts"]["ERROR"]["count"] == 2);
        REQUIRE(json["levelCounts"]["INFO"]["count"] == 3);
        REQUIRE(json["errorEntries"].size() == 2);
        
        // 정리
        std::filesystem::remove(testLogFile);
        std::filesystem::remove(jsonOutputFile);
    }
    
    SECTION("키워드 검색 통합 테스트")
    {
        // 키워드가 포함된 테스트 로그 파일 생성
        std::string testLogFile = "keyword_test.log";
        std::ofstream file(testLogFile);
        file << "2024-01-01 10:00:00 INFO: Database connection established\n";
        file << "2024-01-01 10:00:05 ERROR: Database query failed\n";
        file << "2024-01-01 10:00:10 WARNING: User authentication required\n";
        file << "2024-01-01 10:00:15 INFO: Database backup completed\n";
        file << "2024-01-01 10:00:20 DEBUG: Connection pool status\n";
        file << "2024-01-01 10:00:25 ERROR: Database timeout occurred\n";
        file.close();
        
        LogFileReader reader(testLogFile);
        LogParser parser;
        LogStats stats;
        
        std::string keyword = "Database";
        int keywordMatches = 0;
        
        while (auto line = reader.readLine())
        {
            LogEntry entry = parser.parseLine(*line);
            
            if (parser.containsKeyword(entry.rawLine, keyword))
            {
                stats.addEntryWithKeyword(entry, keyword);
                keywordMatches++;
            }
            else
            {
                stats.addEntry(entry);
            }
        }
        
        stats.finalize();
        
        // 키워드 매칭 결과 검증
        REQUIRE(keywordMatches == 4);  // Database가 포함된 라인 4개
        
        const auto& statistics = stats.getStatistics();
        REQUIRE(statistics.keywordCounts.at(keyword) == 4);
        REQUIRE(statistics.keywordMatches.size() == 4);
        
        // JSON 출력에서 키워드 정보 확인
        std::string jsonStr = stats.toJson();
        auto json = nlohmann::json::parse(jsonStr);
        REQUIRE(json["keywordCounts"][keyword] == 4);
        REQUIRE(json["keywordMatches"].size() == 4);
        
        std::filesystem::remove(testLogFile);
    }
}

TEST_CASE("통합 테스트 - 대용량 파일 처리", "[Integration][Performance]")
{
    SECTION("1000라인 로그 파일 전체 처리")
    {
        std::string largeLogFile = "large_integration_test.log";
        
        // 대용량 로그 파일 생성
        std::ofstream file(largeLogFile);
        for (int i = 0; i < 1000; ++i)
        {
            std::string level = (i % 4 == 0) ? "ERROR" : 
                               (i % 4 == 1) ? "WARNING" :
                               (i % 4 == 2) ? "INFO" : "DEBUG";
            
            file << "2024-01-01 10:" 
                 << std::setfill('0') << std::setw(2) << (i % 60) << ":"
                 << std::setfill('0') << std::setw(2) << (i % 60) << " "
                 << level << ": 메시지 번호 " << i << "\n";
        }
        file.close();
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // 전체 처리 과정
        LogFileReader reader(largeLogFile);
        LogParser parser;
        LogStats stats;
        
        REQUIRE(reader.isValid() == true);
        
        int processedLines = 0;
        while (auto line = reader.readLine())
        {
            LogEntry entry = parser.parseLine(*line);
            stats.addEntry(entry);
            processedLines++;
        }
        
        stats.finalize();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        // 성능 검증 (1000라인 처리가 500ms 이내)
        REQUIRE(duration.count() < 500);
        
        // 결과 검증
        REQUIRE(processedLines == 1000);
        const auto& statistics = stats.getStatistics();
        REQUIRE(statistics.totalLines == 1000);
        REQUIRE(statistics.levelCounts.at("ERROR") == 250);
        REQUIRE(statistics.levelCounts.at("WARNING") == 250);
        REQUIRE(statistics.levelCounts.at("INFO") == 250);
        REQUIRE(statistics.levelCounts.at("DEBUG") == 250);
        
        // JSON 변환 성능 테스트
        auto jsonStartTime = std::chrono::high_resolution_clock::now();
        std::string jsonStr = stats.toJson();
        auto jsonEndTime = std::chrono::high_resolution_clock::now();
        auto jsonDuration = std::chrono::duration_cast<std::chrono::milliseconds>(jsonEndTime - jsonStartTime);
        
        REQUIRE(jsonDuration.count() < 100);  // JSON 변환 100ms 이내
        REQUIRE(!jsonStr.empty());
        
        std::filesystem::remove(largeLogFile);
    }
}

TEST_CASE("통합 테스트 - 다양한 로그 형식 처리", "[Integration][Compatibility]")
{
    SECTION("혼합된 로그 형식 처리")
    {
        std::string mixedLogFile = "mixed_format_test.log";
        std::ofstream file(mixedLogFile);
        
        // 다양한 타임스탬프 형식
        file << "2024-01-01 10:00:00 INFO: 표준 형식\n";
        file << "2024-01-01T10:00:05Z ERROR: ISO 8601 형식\n";
        file << "10:00:10 WARNING: 시간만 있는 형식\n";
        file << "DEBUG: 타임스탬프 없는 형식\n";
        
        // 다양한 로그 레벨 케이스
        file << "2024-01-01 10:00:15 error: 소문자 레벨\n";
        file << "2024-01-01 10:00:20 WARN: 대문자 WARNING\n";
        file << "2024-01-01 10:00:25 Info: 혼합 케이스\n";
        
        // 특수 문자 포함
        file << "2024-01-01 10:00:30 ERROR: 메시지에 \"따옴표\" 포함\n";
        file << "2024-01-01 10:00:35 INFO: 한글 메시지와 🚀 이모지\n";
        
        file.close();
        
        LogFileReader reader(mixedLogFile);
        LogParser parser;
        LogStats stats;
        
        std::vector<LogEntry> allEntries;
        
        while (auto line = reader.readLine())
        {
            LogEntry entry = parser.parseLine(*line);
            allEntries.push_back(entry);
            stats.addEntry(entry);
        }
        
        stats.finalize();
        
        // 모든 라인이 처리되었는지 확인
        REQUIRE(allEntries.size() == 9);
        
        // 각 엔트리의 로그 레벨이 올바르게 감지되었는지 확인
        REQUIRE(allEntries[0].level == "INFO");
        REQUIRE(allEntries[1].level == "ERROR");
        REQUIRE(allEntries[2].level == "WARNING");
        REQUIRE(allEntries[3].level == "DEBUG");
        REQUIRE(allEntries[4].level == "ERROR");  // 소문자 error
        REQUIRE(allEntries[5].level == "WARNING"); // WARN
        REQUIRE(allEntries[6].level == "INFO");   // 혼합 케이스
        REQUIRE(allEntries[7].level == "ERROR");  // 특수 문자
        REQUIRE(allEntries[8].level == "INFO");   // 한글 + 이모지
        
        // 타임스탬프 추출 확인
        REQUIRE(!allEntries[0].timestamp.empty());
        REQUIRE(!allEntries[1].timestamp.empty());
        REQUIRE(!allEntries[2].timestamp.empty());
        REQUIRE(allEntries[3].timestamp.empty());  // 타임스탬프 없음
        
        // 메시지 추출 확인 (특수 문자 포함)
        REQUIRE(allEntries[7].message.find("따옴표") != std::string::npos);
        REQUIRE(allEntries[8].message.find("한글") != std::string::npos);
        REQUIRE(allEntries[8].message.find("🚀") != std::string::npos);
        
        // JSON 변환에서 특수 문자가 올바르게 처리되는지 확인
        std::string jsonStr = stats.toJson();
        REQUIRE_NOTHROW(nlohmann::json::parse(jsonStr));
        
        auto json = nlohmann::json::parse(jsonStr);
        REQUIRE(json["summary"]["totalLines"] == 9);
        
        std::filesystem::remove(mixedLogFile);
    }
}

TEST_CASE("통합 테스트 - 에러 복구 및 견고성", "[Integration][Resilience]")
{
    SECTION("손상된 로그 라인 처리")
    {
        std::string corruptedLogFile = "corrupted_test.log";
        std::ofstream file(corruptedLogFile);
        
        // 정상 라인
        file << "2024-01-01 10:00:00 INFO: 정상 메시지\n";
        
        // 비정상 라인들
        file << "\n";  // 빈 라인
        file << "     \t  \n";  // 공백만 있는 라인
        file << "완전히 잘못된 형식의 라인\n";
        file << "25:99:99 INVALID: 잘못된 시간\n";
        file << "2024-13-99 ERROR: 잘못된 날짜\n";
        
        // 다시 정상 라인
        file << "2024-01-01 10:00:10 ERROR: 복구된 메시지\n";
        
        // 매우 긴 라인
        std::string longLine = "2024-01-01 10:00:15 WARNING: ";
        longLine += std::string(10000, 'A');
        file << longLine << "\n";
        
        file.close();
        
        LogFileReader reader(corruptedLogFile);
        LogParser parser;
        LogStats stats;
        
        int totalLines = 0;
        int processedLines = 0;
        
        while (auto line = reader.readLine())
        {
            totalLines++;
            
            // 모든 라인이 예외 없이 처리되어야 함
            REQUIRE_NOTHROW(parser.parseLine(*line));
            
            LogEntry entry = parser.parseLine(*line);
            stats.addEntry(entry);
            processedLines++;
        }
        
        stats.finalize();
        
        // 모든 라인이 처리되었는지 확인
        REQUIRE(totalLines == processedLines);
        REQUIRE(totalLines == 8);
        
        const auto& statistics = stats.getStatistics();
        REQUIRE(statistics.totalLines == 8);
        
        // 일부 라인은 UNKNOWN으로 분류될 수 있음
        size_t identifiedLines = 0;
        for (const auto& [level, count] : statistics.levelCounts)
        {
            identifiedLines += count;
        }
        REQUIRE(identifiedLines == 8);
        
        // JSON 변환도 문제없이 수행되어야 함
        REQUIRE_NOTHROW(stats.toJson());
        
        std::filesystem::remove(corruptedLogFile);
    }
} 