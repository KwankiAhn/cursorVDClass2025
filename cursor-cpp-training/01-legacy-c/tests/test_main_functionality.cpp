#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <nlohmann/json.hpp>

// 테스트를 위한 헬퍼 함수
std::string executeCommand(const std::string& command)
{
    std::array<char, 128> buffer;
    std::string result;
    
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        return "";
    }
    
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }
    
    pclose(pipe);
    return result;
}

TEST_CASE("Main 기능 테스트 - 커맨드라인 인자 처리", "[Main][CommandLine]")
{
    // 테스트 로그 파일 생성
    std::string testLogFile = "main_test.log";
    std::ofstream file(testLogFile);
    file << "2024-01-01 10:00:00 INFO: 서버 시작\n";
    file << "2024-01-01 10:00:05 ERROR: 데이터베이스 연결 실패\n";
    file << "2024-01-01 10:00:10 WARNING: 메모리 사용량 높음\n";
    file << "2024-01-01 10:00:15 INFO: Database 연결 복구\n";
    file << "2024-01-01 10:00:20 DEBUG: 사용자 로그인 시도\n";
    file << "2024-01-01 10:00:25 ERROR: Database 타임아웃\n";
    file.close();
    
    SECTION("기본 로그 분석 (파일만 지정)")
    {
        std::string command = "./log_analyzer " + testLogFile;
        std::string output = executeCommand(command);
        
        // 출력에 기본 정보가 포함되어야 함
        REQUIRE(output.find("로그 분석 결과") != std::string::npos);
        REQUIRE(output.find("전체 라인 수: 6") != std::string::npos);
        REQUIRE(output.find("ERROR") != std::string::npos);
        REQUIRE(output.find("WARNING") != std::string::npos);
        REQUIRE(output.find("INFO") != std::string::npos);
        REQUIRE(output.find("DEBUG") != std::string::npos);
    }
    
    SECTION("키워드 검색 기능")
    {
        std::string command = "./log_analyzer " + testLogFile + " Database";
        std::string output = executeCommand(command);
        
        // 키워드 검색 결과가 출력에 포함되어야 함
        REQUIRE(output.find("🔎 검색 키워드: Database") != std::string::npos);
        REQUIRE(output.find("[키워드 발견]") != std::string::npos);
    }
    
    SECTION("JSON 출력 기능")
    {
        std::string jsonOutputFile = "main_test_output.json";
        std::string command = "./log_analyzer " + testLogFile + " --output-json " + jsonOutputFile;
        
        // 명령 실행
        std::string output = executeCommand(command);
        
        // JSON 파일이 생성되었는지 확인
        REQUIRE(std::filesystem::exists(jsonOutputFile));
        
        // JSON 파일 내용 검증
        std::ifstream jsonFile(jsonOutputFile);
        std::ostringstream buffer;
        buffer << jsonFile.rdbuf();
        jsonFile.close();
        
        // JSON 파싱 가능한지 확인
        REQUIRE_NOTHROW(nlohmann::json::parse(buffer.str()));
        
        auto json = nlohmann::json::parse(buffer.str());
        REQUIRE(json["summary"]["totalLines"] == 6);
        REQUIRE(json.contains("levelCounts"));
        REQUIRE(json.contains("errorEntries"));
        
        std::filesystem::remove(jsonOutputFile);
    }
    
    SECTION("키워드 검색과 JSON 출력 조합")
    {
        std::string jsonOutputFile = "main_test_keyword_output.json";
        std::string command = "./log_analyzer " + testLogFile + " Database --output-json " + jsonOutputFile;
        
        // 명령 실행
        std::string output = executeCommand(command);
        
        // JSON 파일이 생성되었는지 확인
        REQUIRE(std::filesystem::exists(jsonOutputFile));
        
        // JSON 파일 내용에서 키워드 정보 확인
        std::ifstream jsonFile(jsonOutputFile);
        std::ostringstream buffer;
        buffer << jsonFile.rdbuf();
        jsonFile.close();
        
        auto json = nlohmann::json::parse(buffer.str());
        REQUIRE(json["keywordCounts"]["Database"] > 0);
        REQUIRE(json["keywordMatches"].size() > 0);
        
        std::filesystem::remove(jsonOutputFile);
    }
    
    // 정리
    std::filesystem::remove(testLogFile);
}

TEST_CASE("Main 기능 테스트 - 에러 상황 처리", "[Main][ErrorHandling]")
{
    SECTION("존재하지 않는 파일")
    {
        std::string command = "./log_analyzer nonexistent_file.log 2>&1";
        std::string output = executeCommand(command);
        
        // 에러 메시지가 출력되어야 함
        bool hasErrorMessage = (output.find("파일을 열 수 없습니다") != std::string::npos || 
                               output.find("오류") != std::string::npos ||
                               output.find("error") != std::string::npos);
        REQUIRE(hasErrorMessage);
    }
    
    SECTION("잘못된 커맨드라인 인자")
    {
        // 인자 없이 실행
        std::string command = "./log_analyzer 2>&1";
        std::string output = executeCommand(command);
        
        // 사용법 안내나 에러 메시지가 출력되어야 함
        REQUIRE(!output.empty());
    }
    
    SECTION("잘못된 JSON 출력 경로")
    {
        // 테스트 로그 파일 생성
        std::string testLogFile = "error_test.log";
        std::ofstream file(testLogFile);
        file << "2024-01-01 10:00:00 INFO: 테스트 메시지\n";
        file.close();
        
        // 존재하지 않는 디렉토리에 JSON 출력 시도
        std::string command = "./log_analyzer " + testLogFile + " --output-json /nonexistent_dir/output.json 2>&1";
        std::string output = executeCommand(command);
        
        // 프로그램이 종료되지 않고 에러를 적절히 처리해야 함
        REQUIRE(!output.empty());
        
        std::filesystem::remove(testLogFile);
    }
}

TEST_CASE("Main 기능 테스트 - 성능 벤치마크", "[Main][Performance]")
{
    SECTION("대용량 파일 처리 성능")
    {
        // 대용량 테스트 로그 파일 생성 (5000 라인)
        std::string largeLogFile = "large_performance_test.log";
        std::ofstream file(largeLogFile);
        
        for (int i = 0; i < 5000; ++i)
        {
            std::string level = (i % 4 == 0) ? "ERROR" : 
                               (i % 4 == 1) ? "WARNING" :
                               (i % 4 == 2) ? "INFO" : "DEBUG";
            
            file << "2024-01-01 " 
                 << std::setfill('0') << std::setw(2) << (10 + i / 3600) << ":"
                 << std::setfill('0') << std::setw(2) << ((i % 3600) / 60) << ":"
                 << std::setfill('0') << std::setw(2) << (i % 60) << " "
                 << level << ": 메시지 번호 " << i << "\n";
        }
        file.close();
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // 전체 파일 처리
        std::string command = "./log_analyzer " + largeLogFile + " Database";
        std::string output = executeCommand(command);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        // 5000라인 처리가 2초 이내에 완료되어야 함
        REQUIRE(duration.count() < 2000);
        
        // 결과가 올바르게 출력되었는지 확인
        REQUIRE(output.find("전체 라인 수: 5000") != std::string::npos);
        
        std::filesystem::remove(largeLogFile);
    }
    
    SECTION("JSON 출력 성능")
    {
        // 중간 크기 테스트 파일 생성 (1000 라인)
        std::string testLogFile = "json_performance_test.log";
        std::ofstream file(testLogFile);
        
        for (int i = 0; i < 1000; ++i)
        {
            file << "2024-01-01 10:00:00 ERROR: Error message number " << i << "\n";
        }
        file.close();
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // JSON 출력 포함 처리
        std::string jsonOutputFile = "performance_output.json";
        std::string command = "./log_analyzer " + testLogFile + " --output-json " + jsonOutputFile;
        std::string output = executeCommand(command);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        // JSON 포함 1000라인 처리가 1초 이내에 완료되어야 함
        REQUIRE(duration.count() < 1000);
        
        // JSON 파일이 올바르게 생성되었는지 확인
        REQUIRE(std::filesystem::exists(jsonOutputFile));
        
        std::ifstream jsonFile(jsonOutputFile);
        std::ostringstream buffer;
        buffer << jsonFile.rdbuf();
        jsonFile.close();
        
        auto json = nlohmann::json::parse(buffer.str());
        REQUIRE(json["summary"]["totalLines"] == 1000);
        REQUIRE(json["errorEntries"].size() == 1000);
        
        std::filesystem::remove(testLogFile);
        std::filesystem::remove(jsonOutputFile);
    }
}

TEST_CASE("Main 기능 테스트 - 실제 사용 시나리오", "[Main][RealWorld]")
{
    SECTION("웹 서버 로그 분석 시뮬레이션")
    {
        // 실제 웹 서버 로그와 유사한 형식으로 테스트 파일 생성
        std::string webLogFile = "web_server_test.log";
        std::ofstream file(webLogFile);
        
        file << "2024-01-01 10:00:00 INFO: Server started on port 8080\n";
        file << "2024-01-01 10:00:05 INFO: Database connection pool initialized\n";
        file << "2024-01-01 10:00:10 INFO: User login: user123\n";
        file << "2024-01-01 10:00:15 WARNING: Database connection slow (2.5s)\n";
        file << "2024-01-01 10:00:20 ERROR: Database query timeout\n";
        file << "2024-01-01 10:00:25 INFO: Database connection restored\n";
        file << "2024-01-01 10:00:30 ERROR: User authentication failed: user456\n";
        file << "2024-01-01 10:00:35 INFO: API request: /api/users\n";
        file << "2024-01-01 10:00:40 DEBUG: Cache hit for user data\n";
        file << "2024-01-01 10:00:45 WARNING: High memory usage: 85%\n";
        file.close();
        
        // Database 관련 문제 분석
        std::string jsonOutputFile = "web_analysis.json";
        std::string command = "./log_analyzer " + webLogFile + " Database --output-json " + jsonOutputFile;
        std::string output = executeCommand(command);
        
        // 분석 결과 확인
        REQUIRE(output.find("🔎 검색 키워드: Database") != std::string::npos);
        REQUIRE(std::filesystem::exists(jsonOutputFile));
        
        // JSON 결과 상세 검증
        std::ifstream jsonFile(jsonOutputFile);
        std::ostringstream buffer;
        buffer << jsonFile.rdbuf();
        jsonFile.close();
        
        auto json = nlohmann::json::parse(buffer.str());
        
        // Database 관련 엔트리가 올바르게 식별되었는지 확인
        REQUIRE(json["keywordCounts"]["Database"] >= 3);  // Database 관련 로그 최소 3개
        REQUIRE(json["summary"]["totalLines"] == 10);
        REQUIRE(json["levelCounts"]["ERROR"]["count"] == 2);
        REQUIRE(json["levelCounts"]["WARNING"]["count"] == 2);
        
        std::filesystem::remove(webLogFile);
        std::filesystem::remove(jsonOutputFile);
    }
    
    SECTION("보안 이벤트 로그 분석 시뮬레이션")
    {
        // 보안 관련 로그 시뮬레이션
        std::string securityLogFile = "security_test.log";
        std::ofstream file(securityLogFile);
        
        file << "2024-01-01 10:00:00 INFO: Security system initialized\n";
        file << "2024-01-01 10:00:05 WARNING: Multiple failed login attempts from IP 192.168.1.100\n";
        file << "2024-01-01 10:00:10 ERROR: Brute force attack detected from IP 192.168.1.100\n";
        file << "2024-01-01 10:00:15 INFO: IP 192.168.1.100 blocked\n";
        file << "2024-01-01 10:00:20 ERROR: Unauthorized access attempt to admin panel\n";
        file << "2024-01-01 10:00:25 WARNING: Suspicious SQL query detected\n";
        file << "2024-01-01 10:00:30 INFO: Security scan completed\n";
        file.close();
        
        // 보안 위협 분석
        std::string command = "./log_analyzer " + securityLogFile + " attack";
        std::string output = executeCommand(command);
        
        // 보안 이벤트가 올바르게 식별되었는지 확인
        REQUIRE(output.find("🔎 검색 키워드: attack") != std::string::npos);
        REQUIRE(output.find("[키워드 발견]") != std::string::npos);
        
        std::filesystem::remove(securityLogFile);
    }
} 