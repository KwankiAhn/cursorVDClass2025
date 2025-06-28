#include <catch2/catch_test_macros.hpp>
#include "../LogFileReader.hpp"
#include <fstream>
#include <filesystem>

using namespace LogAnalyzer;

// 테스트용 임시 파일 생성 헬퍼 함수
class TestFileHelper {
public:
    static std::string createTempFile(const std::string& content) {
        std::string tempPath = std::filesystem::temp_directory_path() / "test_log.txt";
        std::ofstream file(tempPath);
        file << content;
        file.close();
        return tempPath;
    }
    
    static void deleteTempFile(const std::string& path) {
        if (std::filesystem::exists(path)) {
            std::filesystem::remove(path);
        }
    }
};

TEST_CASE("LogFileReader 기본 기능 테스트", "[LogFileReader]") {
    SECTION("유효한 파일 읽기") {
        std::string testContent = "Line 1\nLine 2\nLine 3\n";
        std::string tempFile = TestFileHelper::createTempFile(testContent);
        
        LogFileReader reader(tempFile);
        REQUIRE(reader.isValid());
        REQUIRE(reader.getFilePath() == tempFile);
        REQUIRE(reader.getFileSize() > 0);
        
        auto lines = reader.readAllLines();
        REQUIRE(lines.size() == 3);
        REQUIRE(lines[0] == "Line 1");
        REQUIRE(lines[1] == "Line 2");
        REQUIRE(lines[2] == "Line 3");
        
        TestFileHelper::deleteTempFile(tempFile);
    }
    
    SECTION("존재하지 않는 파일 처리") {
        LogFileReader reader("/non/existent/file.txt");
        REQUIRE_FALSE(reader.isValid());
        
        auto lines = reader.readAllLines();
        REQUIRE(lines.empty());
    }
    
    SECTION("빈 경로 처리") {
        LogFileReader reader("");
        REQUIRE_FALSE(reader.isValid());
    }
}

TEST_CASE("LogFileReader 순차 읽기 테스트", "[LogFileReader]") {
    std::string testContent = "First line\nSecond line\nThird line\n";
    std::string tempFile = TestFileHelper::createTempFile(testContent);
    
    LogFileReader reader(tempFile);
    REQUIRE(reader.isValid());
    
    SECTION("순차적 라인 읽기") {
        auto line1 = reader.readNextLine();
        REQUIRE(line1.has_value());
        REQUIRE(line1.value() == "First line");
        
        auto line2 = reader.readNextLine();
        REQUIRE(line2.has_value());
        REQUIRE(line2.value() == "Second line");
        
        auto line3 = reader.readNextLine();
        REQUIRE(line3.has_value());
        REQUIRE(line3.value() == "Third line");
        
        // 더 이상 읽을 라인이 없음
        auto line4 = reader.readNextLine();
        REQUIRE_FALSE(line4.has_value());
    }
    
    TestFileHelper::deleteTempFile(tempFile);
}

TEST_CASE("LogFileReader 빈 파일 처리", "[LogFileReader]") {
    std::string tempFile = TestFileHelper::createTempFile("");
    
    LogFileReader reader(tempFile);
    REQUIRE(reader.isValid());
    REQUIRE(reader.getFileSize() == 0);
    
    auto lines = reader.readAllLines();
    REQUIRE(lines.empty());
    
    auto nextLine = reader.readNextLine();
    REQUIRE_FALSE(nextLine.has_value());
    
    TestFileHelper::deleteTempFile(tempFile);
}

TEST_CASE("LogFileReader 대용량 파일 시뮬레이션", "[LogFileReader]") {
    // 여러 라인을 가진 파일 생성
    std::string testContent;
    const int lineCount = 1000;
    
    for (int i = 1; i <= lineCount; ++i) {
        testContent += "Log line " + std::to_string(i) + "\n";
    }
    
    std::string tempFile = TestFileHelper::createTempFile(testContent);
    
    LogFileReader reader(tempFile);
    REQUIRE(reader.isValid());
    
    SECTION("전체 라인 읽기") {
        auto lines = reader.readAllLines();
        REQUIRE(lines.size() == lineCount);
        REQUIRE(lines[0] == "Log line 1");
        REQUIRE(lines[lineCount - 1] == "Log line " + std::to_string(lineCount));
    }
    
    SECTION("파일 크기 확인") {
        auto fileSize = reader.getFileSize();
        REQUIRE(fileSize > 0);
        REQUIRE(fileSize == testContent.size());
    }
    
    TestFileHelper::deleteTempFile(tempFile);
} 