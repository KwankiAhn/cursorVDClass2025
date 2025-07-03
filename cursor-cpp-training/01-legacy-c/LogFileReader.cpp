#include "LogFileReader.hpp"
#include <iostream>
#include <stdexcept>
#include <locale>

// 생성자 (std::string 버전)
LogFileReader::LogFileReader(const std::string& filePath)
    : filePath_(filePath)
    , currentLineNumber_(0)
    , isOpen_(false)
{
    // 생성자에서는 파일 검증만 수행, 실제 열기는 open()에서
}

// 생성자 (std::filesystem::path 버전)
LogFileReader::LogFileReader(const std::filesystem::path& filePath)
    : filePath_(filePath)
    , currentLineNumber_(0)
    , isOpen_(false)
{
    // 생성자에서는 파일 검증만 수행, 실제 열기는 open()에서
}

// 파일 유효성 검사
bool LogFileReader::isValid() const noexcept
{
    return validateFile();
}

// 파일 열기
bool LogFileReader::open()
{
    // 이미 열린 경우 닫기
    if (isOpen_)
    {
        close();
    }
    
    // 파일 유효성 확인
    if (!validateFile())
    {
        return false;
    }
    
    // 파일 스트림 열기
    fileStream_.open(filePath_, std::ios::in);
    
    if (!fileStream_.is_open())
    {
        return false;
    }
    
    // UTF-8 BOM 처리 (선택적)
    fileStream_.imbue(std::locale(""));
    
    isOpen_ = true;
    currentLineNumber_ = 0;
    
    return true;
}

// 파일 닫기
void LogFileReader::close() noexcept
{
    if (fileStream_.is_open())
    {
        fileStream_.close();
    }
    
    isOpen_ = false;
    currentLineNumber_ = 0;
}

// 한 라인 읽기
std::optional<std::string> LogFileReader::readLine()
{
    if (!isOpen_ || !fileStream_.is_open())
    {
        return std::nullopt;
    }
    
    std::string line;
    
    if (std::getline(fileStream_, line))
    {
        currentLineNumber_++;
        return line;
    }
    
    // EOF에 도달하거나 오류 발생
    return std::nullopt;
}

// 전체 파일 읽기
std::vector<std::string> LogFileReader::readAllLines()
{
    std::vector<std::string> lines;
    
    if (!isOpen_ && !open())
    {
        return lines; // 빈 벡터 반환
    }
    
    // 파일 크기 추정으로 벡터 용량 미리 할당 (성능 최적화)
    if (auto fileSize = getFileSize())
    {
        // 평균 라인 길이를 80자로 가정하여 대략적인 라인 수 계산
        lines.reserve(*fileSize / 80 + 100);
    }
    
    std::string line;
    while (std::getline(fileStream_, line))
    {
        currentLineNumber_++;
        lines.push_back(std::move(line));
    }
    
    return lines;
}

// 파일 경로 반환
const std::filesystem::path& LogFileReader::getFilePath() const noexcept
{
    return filePath_;
}

// 파일 크기 반환
std::optional<std::uintmax_t> LogFileReader::getFileSize() const noexcept
{
    std::error_code ec;
    auto size = std::filesystem::file_size(filePath_, ec);
    
    if (ec)
    {
        return std::nullopt;
    }
    
    return size;
}

// 현재 라인 번호 반환
std::size_t LogFileReader::getCurrentLineNumber() const noexcept
{
    return currentLineNumber_;
}

// private: 파일 검증
bool LogFileReader::validateFile() const noexcept
{
    std::error_code ec;
    
    // 파일 존재 확인
    if (!std::filesystem::exists(filePath_, ec) || ec)
    {
        return false;
    }
    
    // 일반 파일인지 확인 (디렉토리나 특수 파일 제외)
    if (!std::filesystem::is_regular_file(filePath_, ec) || ec)
    {
        return false;
    }
    
    // 읽기 권한 확인 (간접적으로)
    auto perms = std::filesystem::status(filePath_, ec).permissions();
    if (ec)
    {
        return false;
    }
    
    // 소유자, 그룹, 기타 사용자 중 하나라도 읽기 권한이 있으면 OK
    using std::filesystem::perms;
    return (perms & (perms::owner_read | perms::group_read | perms::others_read)) != perms::none;
} 