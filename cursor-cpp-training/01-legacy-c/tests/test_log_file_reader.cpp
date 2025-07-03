#include <catch2/catch_test_macros.hpp>
#include "../LogFileReader.hpp"
#include <filesystem>
#include <fstream>
#include <string>

TEST_CASE("LogFileReader - 파일 존재 여부 확인", "[LogFileReader]")
{
    SECTION("존재하는 파일")
    {
        // 테스트용 임시 파일 생성
        std::string tempFile = "test_temp.log";
        std::ofstream file(tempFile);
        file << "Test log line 1\nTest log line 2\n";
        file.close();
        
        LogFileReader reader(tempFile);
        REQUIRE(reader.isValid() == true);
        
        // 임시 파일 삭제
        std::filesystem::remove(tempFile);
    }
    
    SECTION("존재하지 않는 파일")
    {
        LogFileReader reader("nonexistent_file.log");
        REQUIRE(reader.isValid() == false);
    }
}

TEST_CASE("LogFileReader - 파일 정보 읽기", "[LogFileReader]")
{
    // 테스트용 임시 파일 생성
    std::string tempFile = "test_temp.log";
    std::string testContent = "Test log line 1\nTest log line 2\nTest log line 3\n";
    std::ofstream file(tempFile);
    file << testContent;
    file.close();
    
    LogFileReader reader(tempFile);
    
    SECTION("파일 경로 확인")
    {
        REQUIRE(reader.getFilePath() == tempFile);
    }
    
    SECTION("파일 크기 확인")
    {
        REQUIRE(reader.getFileSize() > 0);
        REQUIRE(reader.getFileSize() == testContent.size());
    }
    
    // 임시 파일 삭제
    std::filesystem::remove(tempFile);
}

TEST_CASE("LogFileReader - 라인 읽기", "[LogFileReader]")
{
    // 테스트용 임시 파일 생성
    std::string tempFile = "test_temp.log";
    std::ofstream file(tempFile);
    file << "첫 번째 라인\n";
    file << "두 번째 라인\n";
    file << "세 번째 라인\n";
    file.close();
    
    LogFileReader reader(tempFile);
    REQUIRE(reader.isValid() == true);
    
    SECTION("라인 순차 읽기")
    {
        auto line1 = reader.readLine();
        REQUIRE(line1.has_value());
        REQUIRE(*line1 == "첫 번째 라인");
        
        auto line2 = reader.readLine();
        REQUIRE(line2.has_value());
        REQUIRE(*line2 == "두 번째 라인");
        
        auto line3 = reader.readLine();
        REQUIRE(line3.has_value());
        REQUIRE(*line3 == "세 번째 라인");
        
        auto line4 = reader.readLine();
        REQUIRE(line4.has_value() == false);
    }
    
    // 임시 파일 삭제
    std::filesystem::remove(tempFile);
}

TEST_CASE("LogFileReader - 빈 파일 처리", "[LogFileReader]")
{
    // 빈 테스트 파일 생성
    std::string tempFile = "test_empty.log";
    std::ofstream file(tempFile);
    file.close();
    
    LogFileReader reader(tempFile);
    REQUIRE(reader.isValid() == true);
    REQUIRE(reader.getFileSize() == 0);
    
    auto line = reader.readLine();
    REQUIRE(line.has_value() == false);
    
    // 임시 파일 삭제
    std::filesystem::remove(tempFile);
}

TEST_CASE("LogFileReader - 대용량 파일 처리", "[LogFileReader]")
{
    // 대용량 테스트 파일 생성 (1000 라인)
    std::string tempFile = "test_large.log";
    std::ofstream file(tempFile);
    for (int i = 0; i < 1000; ++i)
    {
        file << "Test line " << i << "\n";
    }
    file.close();
    
    LogFileReader reader(tempFile);
    REQUIRE(reader.isValid() == true);
    REQUIRE(reader.getFileSize() > 0);
    
    // 모든 라인 읽기
    int lineCount = 0;
    while (auto line = reader.readLine())
    {
        ++lineCount;
        REQUIRE(line->find("Test line") != std::string::npos);
    }
    
    REQUIRE(lineCount == 1000);
    
    // 임시 파일 삭제
    std::filesystem::remove(tempFile);
}

TEST_CASE("LogFileReader - 라인 번호 추적", "[LogFileReader]")
{
    // 테스트용 임시 파일 생성
    std::string tempFile = "test_line_tracking.log";
    std::ofstream file(tempFile);
    file << "첫 번째 라인\n";
    file << "두 번째 라인\n";
    file << "세 번째 라인\n";
    file.close();
    
    LogFileReader reader(tempFile);
    REQUIRE(reader.isValid() == true);
    
    SECTION("초기 라인 번호")
    {
        REQUIRE(reader.getCurrentLine() == 0);
    }
    
    SECTION("라인 읽기 후 번호 증가 확인")
    {
        auto line1 = reader.readLine();
        REQUIRE(line1.has_value());
        REQUIRE(reader.getCurrentLine() == 1);
        
        auto line2 = reader.readLine();
        REQUIRE(line2.has_value());
        REQUIRE(reader.getCurrentLine() == 2);
        
        auto line3 = reader.readLine();
        REQUIRE(line3.has_value());
        REQUIRE(reader.getCurrentLine() == 3);
        
        // 파일 끝에 도달해도 라인 번호 유지
        auto line4 = reader.readLine();
        REQUIRE(line4.has_value() == false);
        REQUIRE(reader.getCurrentLine() == 3);
    }
    
    // 임시 파일 삭제
    std::filesystem::remove(tempFile);
}

TEST_CASE("LogFileReader - 스트림 초기화", "[LogFileReader]")
{
    // 테스트용 임시 파일 생성
    std::string tempFile = "test_reset.log";
    std::ofstream file(tempFile);
    file << "라인 1\n";
    file << "라인 2\n";
    file << "라인 3\n";
    file.close();
    
    LogFileReader reader(tempFile);
    REQUIRE(reader.isValid() == true);
    
    SECTION("reset() 후 라인 번호 초기화 확인")
    {
        // 몇 줄 읽기
        auto line1 = reader.readLine();
        auto line2 = reader.readLine();
        REQUIRE(reader.getCurrentLine() == 2);
        
        // 리셋 후 확인
        reader.reset();
        REQUIRE(reader.getCurrentLine() == 0);
        
        // 다시 처음부터 읽기 가능한지 확인
        auto firstLine = reader.readLine();
        REQUIRE(firstLine.has_value());
        REQUIRE(*firstLine == "라인 1");
        REQUIRE(reader.getCurrentLine() == 1);
    }
    
    SECTION("reset() 후 재읽기 가능 여부 확인")
    {
        // 모든 라인 읽기
        while (reader.readLine().has_value()) {
            // 모든 라인 읽기
        }
        REQUIRE(reader.getCurrentLine() == 3);
        
        // 더 이상 읽을 수 없음 확인
        auto endLine = reader.readLine();
        REQUIRE(endLine.has_value() == false);
        
        // 리셋 후 다시 읽기 가능
        reader.reset();
        auto resetLine = reader.readLine();
        REQUIRE(resetLine.has_value());
        REQUIRE(*resetLine == "라인 1");
    }
    
    // 임시 파일 삭제
    std::filesystem::remove(tempFile);
}

TEST_CASE("LogFileReader - 이동 의미론", "[LogFileReader]")
{
    // 테스트용 임시 파일 생성
    std::string tempFile = "test_move.log";
    std::ofstream file(tempFile);
    file << "이동 테스트 라인 1\n";
    file << "이동 테스트 라인 2\n";
    file.close();
    
    SECTION("이동 생성자 테스트")
    {
        LogFileReader reader1(tempFile);
        REQUIRE(reader1.isValid() == true);
        
        // 한 줄 읽기
        auto line1 = reader1.readLine();
        REQUIRE(line1.has_value());
        REQUIRE(reader1.getCurrentLine() == 1);
        
        // 이동 생성자 호출
        LogFileReader reader2 = std::move(reader1);
        
        // 이동된 객체 확인
        REQUIRE(reader2.isValid() == true);
        REQUIRE(reader2.getCurrentLine() == 1);
        REQUIRE(reader2.getFilePath() == tempFile);
        
        // 원본 객체는 유효하지 않음
        REQUIRE(reader1.isValid() == false);
        
        // 이동된 객체로 계속 읽기 가능
        auto line2 = reader2.readLine();
        REQUIRE(line2.has_value());
        REQUIRE(*line2 == "이동 테스트 라인 2");
    }
    
    SECTION("이동 할당 연산자 테스트")
    {
        LogFileReader reader1(tempFile);
        LogFileReader reader2("nonexistent.log");
        
        REQUIRE(reader1.isValid() == true);
        REQUIRE(reader2.isValid() == false);
        
        // 한 줄 읽기
        auto line1 = reader1.readLine();
        REQUIRE(reader1.getCurrentLine() == 1);
        
        // 이동 할당
        reader2 = std::move(reader1);
        
        // 이동된 객체 확인
        REQUIRE(reader2.isValid() == true);
        REQUIRE(reader2.getCurrentLine() == 1);
        REQUIRE(reader2.getFilePath() == tempFile);
        
        // 원본 객체는 유효하지 않음
        REQUIRE(reader1.isValid() == false);
    }
    
    // 임시 파일 삭제
    std::filesystem::remove(tempFile);
}

TEST_CASE("LogFileReader - 파일 시스템 에러 처리", "[LogFileReader][ErrorHandling]")
{
    SECTION("읽기 권한 없는 파일 처리")
    {
        std::string tempFile = "test_readonly.log";
        
        // 파일 생성
        std::ofstream file(tempFile);
        file << "테스트 내용\n";
        file.close();
        
        // 읽기 권한 제거 시도 (Unix 시스템)
        #ifndef _WIN32
        std::filesystem::permissions(tempFile, std::filesystem::perms::none);
        
        LogFileReader reader(tempFile);
        // 권한이 없는 파일은 유효하지 않아야 함
        REQUIRE(reader.isValid() == false);
        
        // 권한 복원 후 삭제
        std::filesystem::permissions(tempFile, std::filesystem::perms::owner_all);
        #endif
        
        std::filesystem::remove(tempFile);
    }
    
    SECTION("존재하지 않는 디렉토리의 파일")
    {
        std::string invalidPath = "/nonexistent_directory_12345/test.log";
        
        LogFileReader reader(invalidPath);
        REQUIRE(reader.isValid() == false);
        REQUIRE(reader.getFileSize() == 0);
        
        auto line = reader.readLine();
        REQUIRE(line.has_value() == false);
    }
    
    SECTION("심볼릭 링크 파일 처리")
    {
        #ifndef _WIN32
        std::string originalFile = "test_original.log";
        std::string linkFile = "test_link.log";
        
        // 원본 파일 생성
        std::ofstream file(originalFile);
        file << "원본 파일 내용\n";
        file.close();
        
        // 심볼릭 링크 생성
        std::filesystem::create_symlink(originalFile, linkFile);
        
        LogFileReader reader(linkFile);
        REQUIRE(reader.isValid() == true);
        
        auto line = reader.readLine();
        REQUIRE(line.has_value());
        REQUIRE(*line == "원본 파일 내용");
        
        // 정리
        std::filesystem::remove(linkFile);
        std::filesystem::remove(originalFile);
        #endif
    }
    
    SECTION("파일 경로에 특수 문자 포함")
    {
        std::string specialFile = "test file with spaces & symbols!@#.log";
        
        std::ofstream file(specialFile);
        file << "특수 문자 파일명 테스트\n";
        file.close();
        
        LogFileReader reader(specialFile);
        REQUIRE(reader.isValid() == true);
        
        auto line = reader.readLine();
        REQUIRE(line.has_value());
        REQUIRE(*line == "특수 문자 파일명 테스트");
        
        std::filesystem::remove(specialFile);
    }
}

TEST_CASE("LogFileReader - 메모리 및 성능 테스트", "[LogFileReader][Performance]")
{
    SECTION("매우 긴 라인 처리")
    {
        std::string tempFile = "test_long_line.log";
        
        // 10KB 길이의 라인 생성
        std::string longLine(10240, 'A');
        longLine += "\n";
        
        std::ofstream file(tempFile);
        file << longLine;
        file << "짧은 라인\n";
        file.close();
        
        LogFileReader reader(tempFile);
        REQUIRE(reader.isValid() == true);
        
        auto line1 = reader.readLine();
        REQUIRE(line1.has_value());
        REQUIRE(line1->size() == 10240);
        REQUIRE(line1->front() == 'A');
        REQUIRE(line1->back() == 'A');
        
        auto line2 = reader.readLine();
        REQUIRE(line2.has_value());
        REQUIRE(*line2 == "짧은 라인");
        
        std::filesystem::remove(tempFile);
    }
    
    SECTION("빈 라인과 공백만 있는 라인들")
    {
        std::string tempFile = "test_empty_lines.log";
        
        std::ofstream file(tempFile);
        file << "\n";                  // 빈 라인
        file << "   \n";              // 공백만 있는 라인
        file << "\t\t\n";             // 탭만 있는 라인
        file << "실제 내용\n";         // 실제 내용
        file << "    \t  \r  \n";     // 다양한 공백 문자
        file.close();
        
        LogFileReader reader(tempFile);
        REQUIRE(reader.isValid() == true);
        
        auto line1 = reader.readLine();
        REQUIRE(line1.has_value());
        REQUIRE(line1->empty());
        
        auto line2 = reader.readLine();
        REQUIRE(line2.has_value());
        REQUIRE(*line2 == "   ");
        
        auto line3 = reader.readLine();
        REQUIRE(line3.has_value());
        REQUIRE(*line3 == "\t\t");
        
        auto line4 = reader.readLine();
        REQUIRE(line4.has_value());
        REQUIRE(*line4 == "실제 내용");
        
        auto line5 = reader.readLine();
        REQUIRE(line5.has_value());
        REQUIRE(!line5->empty());
        
        std::filesystem::remove(tempFile);
    }
    
    SECTION("연속적인 파일 읽기 작업")
    {
        std::string tempFile = "test_continuous.log";
        
        // 여러 번의 읽기 작업 수행
        for (int iteration = 0; iteration < 3; ++iteration)
        {
            std::ofstream file(tempFile);
            file << "반복 " << iteration << " 내용\n";
            file.close();
            
            LogFileReader reader(tempFile);
            REQUIRE(reader.isValid() == true);
            
            auto line = reader.readLine();
            REQUIRE(line.has_value());
            REQUIRE(line->find("반복 " + std::to_string(iteration)) != std::string::npos);
        }
        
        std::filesystem::remove(tempFile);
    }
} 