#include "LogParser.hpp"
#include <algorithm>
#include <sstream>

// 생성자
LogParser::LogParser()
    : totalParsed_(0)
    , successfulParsed_(0)
{
    initializePatterns();
}

// 한 라인 파싱
std::optional<LogEntry> LogParser::parseLine(const std::string& line, std::size_t lineNumber) const
{
    totalParsed_++;
    
    // 빈 라인이나 너무 짧은 라인 건너뛰기
    if (line.empty() || line.length() < 10)
    {
        return std::nullopt;
    }
    
    try
    {
        // 타임스탬프 추출
        std::string timestamp = extractTimestamp(line);
        if (timestamp.empty())
        {
            return std::nullopt;
        }
        
        // 로그 레벨 추출
        LogLevel level = extractLogLevel(line);
        if (level == LogLevel::UNKNOWN)
        {
            return std::nullopt;
        }
        
        // 메시지 추출 (타임스탬프와 레벨 이후의 텍스트)
        std::string message = extractMessage(line, timestamp.length(), 0);
        if (message.empty())
        {
            return std::nullopt;
        }
        
        successfulParsed_++;
        return LogEntry(timestamp, level, message, lineNumber);
    }
    catch (const std::exception&)
    {
        // 파싱 오류 시 nullopt 반환
        return std::nullopt;
    }
}

// 여러 라인 파싱
std::vector<LogEntry> LogParser::parseLines(const std::vector<std::string>& lines) const
{
    std::vector<LogEntry> entries;
    entries.reserve(lines.size());
    
    for (std::size_t i = 0; i < lines.size(); ++i)
    {
        auto entry = parseLine(lines[i], i + 1);
        if (entry)
        {
            entries.push_back(std::move(*entry));
        }
    }
    
    return entries;
}

// 키워드 필터링
std::vector<LogEntry> LogParser::filterByKeyword(const std::vector<LogEntry>& entries, 
                                                 const std::string& keyword) const
{
    if (keyword.empty())
    {
        return entries;
    }
    
    std::vector<LogEntry> filtered;
    
    std::copy_if(entries.begin(), entries.end(), std::back_inserter(filtered),
                [&keyword](const LogEntry& entry) 
                {
                    return entry.containsKeyword(keyword);
                });
    
    return filtered;
}

// 레벨 필터링
std::vector<LogEntry> LogParser::filterByLevel(const std::vector<LogEntry>& entries, 
                                              LogLevel level) const
{
    std::vector<LogEntry> filtered;
    
    std::copy_if(entries.begin(), entries.end(), std::back_inserter(filtered),
                [level](const LogEntry& entry) 
                {
                    return entry.level == level;
                });
    
    return filtered;
}

// 최소 레벨 필터링
std::vector<LogEntry> LogParser::filterByMinLevel(const std::vector<LogEntry>& entries, 
                                                  LogLevel minLevel) const
{
    std::vector<LogEntry> filtered;
    
    std::copy_if(entries.begin(), entries.end(), std::back_inserter(filtered),
                [minLevel](const LogEntry& entry) 
                {
                    return static_cast<int>(entry.level) >= static_cast<int>(minLevel);
                });
    
    return filtered;
}

// 파싱 성공률
double LogParser::getParseSuccessRate() const noexcept
{
    if (totalParsed_ == 0)
    {
        return 0.0;
    }
    
    return static_cast<double>(successfulParsed_) / static_cast<double>(totalParsed_);
}

// 통계 초기화
void LogParser::resetStatistics() noexcept
{
    totalParsed_ = 0;
    successfulParsed_ = 0;
}

// private: 타임스탬프 추출
std::string LogParser::extractTimestamp(const std::string& line) const
{
    std::smatch match;
    
    if (std::regex_search(line, match, timestampPattern_))
    {
        return match[0].str();
    }
    
    return "";
}

// private: 로그 레벨 추출
LogLevel LogParser::extractLogLevel(const std::string& line) const
{
    std::smatch match;
    
    if (std::regex_search(line, match, logLevelPattern_))
    {
        std::string levelStr = match[1].str();
        return stringToLogLevel(levelStr);
    }
    
    return LogLevel::UNKNOWN;
}

// private: 메시지 추출
std::string LogParser::extractMessage(const std::string& line, 
                                     std::size_t timestampEnd, 
                                     std::size_t levelEnd) const
{
    // 정규식을 사용하여 더 정확한 메시지 추출
    std::smatch timestampMatch;
    std::smatch levelMatch;
    
    // 타임스탬프 매치 위치 찾기
    if (!std::regex_search(line, timestampMatch, timestampPattern_))
    {
        return "";
    }
    
    // 로그 레벨 매치 위치 찾기
    if (!std::regex_search(line, levelMatch, logLevelPattern_))
    {
        return "";
    }
    
    // 레벨 이후의 위치부터 메시지 시작
    std::size_t messageStart = levelMatch.position() + levelMatch.length();
    
    // 메시지 부분 추출
    if (messageStart >= line.length())
    {
        return "";
    }
    
    std::string message = line.substr(messageStart);
    
    // 앞뒤 공백 제거
    auto start = message.find_first_not_of(" \t");
    if (start == std::string::npos)
    {
        return "";
    }
    
    auto end = message.find_last_not_of(" \t\r\n");
    if (end == std::string::npos)
    {
        return message.substr(start);
    }
    
    return message.substr(start, end - start + 1);
}

// private: 정규식 패턴 초기화
void LogParser::initializePatterns()
{
    // 타임스탬프 패턴: YYYY-MM-DD HH:MM:SS 형식
    timestampPattern_ = std::regex(R"(\d{4}-\d{2}-\d{2}\s+\d{2}:\d{2}:\d{2})");
    
    // 로그 레벨 패턴: ERROR, WARN, INFO, DEBUG 등
    logLevelPattern_ = std::regex(R"(\b(ERROR|WARN|WARNING|INFO|DEBUG)\b)");
} 