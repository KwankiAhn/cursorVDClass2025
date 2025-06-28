#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <optional>
#include <filesystem>

namespace LogAnalyzer {

class LogFileReader {
public:
    explicit LogFileReader(const std::string& filePath);
    ~LogFileReader() = default;

    // 복사 생성자와 대입 연산자 삭제 (RAII 패턴)
    LogFileReader(const LogFileReader&) = delete;
    LogFileReader& operator=(const LogFileReader&) = delete;

    // 이동 생성자와 이동 대입 연산자
    LogFileReader(LogFileReader&&) = default;
    LogFileReader& operator=(LogFileReader&&) = default;

    // 파일 유효성 검사
    bool isValid() const noexcept;
    
    // 전체 로그 라인 읽기
    std::vector<std::string> readAllLines();
    
    // 라인별 순차 읽기 (메모리 효율적)
    std::optional<std::string> readNextLine();
    
    // 파일 정보
    std::uintmax_t getFileSize() const;
    std::string getFilePath() const noexcept;

private:
    std::string filePath_;
    std::ifstream fileStream_;
    bool isValid_;
    
    void validateFile();
};

} // namespace LogAnalyzer 