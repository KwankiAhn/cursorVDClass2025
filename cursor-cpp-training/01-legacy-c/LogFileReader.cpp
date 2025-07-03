#include "LogFileReader.hpp"
#include <stdexcept>
#include <system_error>

LogFileReader::LogFileReader(const std::filesystem::path& path)
    : filePath_(path), isValid_(false), fileSize_(0), currentLine_(0)
{
    validateFile();
    
    if (isValid_)
    {
        calculateFileSize();
        fileStream_.open(filePath_, std::ios::in);
        isValid_ = fileStream_.is_open();
    }
}

LogFileReader::LogFileReader(LogFileReader&& other) noexcept
    : filePath_(std::move(other.filePath_))
    , fileStream_(std::move(other.fileStream_))
    , isValid_(other.isValid_)
    , fileSize_(other.fileSize_)
    , currentLine_(other.currentLine_)
{
    // 이동된 객체의 상태를 초기화
    other.isValid_ = false;
    other.fileSize_ = 0;
    other.currentLine_ = 0;
}

LogFileReader& LogFileReader::operator=(LogFileReader&& other) noexcept
{
    if (this != &other)
    {
        // 현재 파일 스트림을 닫고 이동
        fileStream_.close();
        
        filePath_ = std::move(other.filePath_);
        fileStream_ = std::move(other.fileStream_);
        isValid_ = other.isValid_;
        fileSize_ = other.fileSize_;
        currentLine_ = other.currentLine_;
        
        // 이동된 객체의 상태를 초기화
        other.isValid_ = false;
        other.fileSize_ = 0;
        other.currentLine_ = 0;
    }
    return *this;
}

bool LogFileReader::isValid() const noexcept
{
    return isValid_;
}

std::optional<std::string> LogFileReader::readLine()
{
    if (!isValid_ || !fileStream_.is_open())
    {
        return std::nullopt;
    }
    
    std::string line;
    if (std::getline(fileStream_, line))
    {
        ++currentLine_;
        return line;
    }
    
    return std::nullopt;
}

size_t LogFileReader::getFileSize() const noexcept
{
    return fileSize_;
}

size_t LogFileReader::getCurrentLine() const noexcept
{
    return currentLine_;
}

void LogFileReader::reset()
{
    if (isValid_ && fileStream_.is_open())
    {
        fileStream_.clear();
        fileStream_.seekg(0, std::ios::beg);
        currentLine_ = 0;
    }
}

const std::filesystem::path& LogFileReader::getFilePath() const noexcept
{
    return filePath_;
}

void LogFileReader::calculateFileSize()
{
    try
    {
        fileSize_ = std::filesystem::file_size(filePath_);
    }
    catch (const std::filesystem::filesystem_error&)
    {
        fileSize_ = 0;
        isValid_ = false;
    }
}

void LogFileReader::validateFile()
{
    try
    {
        // 파일 경로가 비어있는지 확인
        if (filePath_.empty())
        {
            isValid_ = false;
            return;
        }
        
        // 파일이 존재하는지 확인
        if (!std::filesystem::exists(filePath_))
        {
            isValid_ = false;
            return;
        }
        
        // 파일이 일반 파일인지 확인 (디렉토리가 아닌)
        if (!std::filesystem::is_regular_file(filePath_))
        {
            isValid_ = false;
            return;
        }
        
        // 파일 읽기 권한이 있는지 확인
        std::error_code ec;
        auto perms = std::filesystem::status(filePath_, ec).permissions();
        if (ec || (perms & std::filesystem::perms::owner_read) == std::filesystem::perms::none)
        {
            isValid_ = false;
            return;
        }
        
        isValid_ = true;
    }
    catch (const std::filesystem::filesystem_error&)
    {
        isValid_ = false;
    }
    catch (const std::exception&)
    {
        isValid_ = false;
    }
} 