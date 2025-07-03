#include <catch2/catch_test_macros.hpp>
#include "LogFileReader.hpp"
#include <filesystem>
#include <fstream>

TEST_CASE("LogFileReader 기본 기능 테스트", "[LogFileReader]")
{
    SECTION("존재하지 않는 파일 처리")
    {
        LogFileReader reader{std::string("nonexistent_file.log")};
        REQUIRE_FALSE(reader.isValid());
        REQUIRE_FALSE(reader.open());
    }
    
    SECTION("빈 파일 처리")
    {
        // 임시 빈 파일 생성
        std::string tempFile = "temp_empty.log";
        std::ofstream ofs(tempFile);
        ofs.close();
        
        LogFileReader reader{tempFile};
        REQUIRE(reader.isValid());
        REQUIRE(reader.open());
        
        auto lines = reader.readAllLines();
        REQUIRE(lines.empty());
        
        // 정리
        std::filesystem::remove(tempFile);
    }
    
    SECTION("파일 크기 확인")
    {
        // 임시 파일 생성
        std::string tempFile = "temp_size_test.log";
        std::ofstream ofs(tempFile);
        ofs << "Hello World\n";
        ofs << "Second Line\n";
        ofs.close();
        
        LogFileReader reader{tempFile};
        REQUIRE(reader.isValid());
        
        auto size = reader.getFileSize();
        REQUIRE(size.has_value());
        REQUIRE(*size > 0);
        
        // 정리
        std::filesystem::remove(tempFile);
    }
}

TEST_CASE("LogFileReader 라인 읽기 테스트", "[LogFileReader][readline]")
{
    // 테스트용 임시 파일 생성
    std::string tempFile = "temp_line_test.log";
    std::ofstream ofs(tempFile);
    ofs << "First line\n";
    ofs << "Second line\n";
    ofs << "Third line\n";
    ofs.close();
    
    SECTION("한 줄씩 읽기")
    {
        LogFileReader reader{tempFile};
        REQUIRE(reader.open());
        
        auto line1 = reader.readLine();
        REQUIRE(line1.has_value());
        REQUIRE(*line1 == "First line");
        REQUIRE(reader.getCurrentLineNumber() == 1);
        
        auto line2 = reader.readLine();
        REQUIRE(line2.has_value());
        REQUIRE(*line2 == "Second line");
        REQUIRE(reader.getCurrentLineNumber() == 2);
        
        auto line3 = reader.readLine();
        REQUIRE(line3.has_value());
        REQUIRE(*line3 == "Third line");
        REQUIRE(reader.getCurrentLineNumber() == 3);
        
        auto line4 = reader.readLine();
        REQUIRE_FALSE(line4.has_value()); // EOF
    }
    
    SECTION("전체 라인 읽기")
    {
        LogFileReader reader{tempFile};
        auto lines = reader.readAllLines();
        
        REQUIRE(lines.size() == 3);
        REQUIRE(lines[0] == "First line");
        REQUIRE(lines[1] == "Second line");
        REQUIRE(lines[2] == "Third line");
        REQUIRE(reader.getCurrentLineNumber() == 3);
    }
    
    // 정리
    std::filesystem::remove(tempFile);
}

TEST_CASE("LogFileReader 오류 처리 테스트", "[LogFileReader][error]")
{
    SECTION("디렉토리를 파일로 처리")
    {
        // 임시 디렉토리 생성
        std::string tempDir = "temp_dir";
        std::filesystem::create_directory(tempDir);
        
        LogFileReader reader{tempDir};
        REQUIRE_FALSE(reader.isValid());
        
        // 정리
        std::filesystem::remove(tempDir);
    }
    
    SECTION("파일을 열지 않고 읽기 시도")
    {
        std::string tempFile = "temp_no_open.log";
        std::ofstream ofs(tempFile);
        ofs << "Test content\n";
        ofs.close();
        
        LogFileReader reader{tempFile};
        // open()을 호출하지 않고 readLine() 시도
        auto line = reader.readLine();
        REQUIRE_FALSE(line.has_value());
        
        // 정리
        std::filesystem::remove(tempFile);
    }
}

TEST_CASE("LogFileReader 파일 경로 테스트", "[LogFileReader][path]")
{
    SECTION("std::string 생성자")
    {
        std::string path = "test.log";
        LogFileReader reader{path};
        REQUIRE(reader.getFilePath() == path);
    }
    
    SECTION("std::filesystem::path 생성자")
    {
        std::filesystem::path path = "test.log";
        LogFileReader reader{path};
        REQUIRE(reader.getFilePath() == path);
    }
} 