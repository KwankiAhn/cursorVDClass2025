#include "LogParser.hpp"
#include <algorithm>
#include <cctype>
#include <regex>

LogEntry LogParser::parseLine(std::string_view line) const
{
    LogEntry entry;
    entry.rawLine = line;
    
    // 로그 레벨 감지
    entry.level = detectLogLevel(line);
    
    // 타임스탬프 추출
    entry.timestamp = extractTimestamp(line);
    
    // 메시지 추출
    entry.message = extractMessage(line, entry.level);
    
    return entry;
}

std::string LogParser::detectLogLevel(std::string_view line) const
{
    // 대소문자 구분 없이 로그 레벨 감지
    std::string lowerLine;
    lowerLine.reserve(line.size());
    std::transform(line.begin(), line.end(), std::back_inserter(lowerLine), 
                   [](unsigned char c) { return std::tolower(c); });
    
    if (lowerLine.find("error") != std::string::npos)
    {
        return "ERROR";
    }
    else if (lowerLine.find("warn") != std::string::npos)
    {
        return "WARNING";
    }
    else if (lowerLine.find("info") != std::string::npos)
    {
        return "INFO";
    }
    else if (lowerLine.find("debug") != std::string::npos)
    {
        return "DEBUG";
    }
    
    return "UNKNOWN";
}

std::string LogParser::extractTimestamp(std::string_view line) const
{
    // 일반적인 타임스탬프 패턴들을 매칭
    static const std::regex timestampPatterns[] = {
        // ISO 8601 형식: 2024-01-01T12:00:00
        std::regex(R"(\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}(?:\.\d+)?(?:Z|[+-]\d{2}:\d{2})?)", std::regex::icase),
        // 일반 형식: 2024-01-01 12:00:00
        std::regex(R"(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}(?:\.\d+)?)", std::regex::icase),
        // 짧은 형식: 01-01 12:00:00
        std::regex(R"(\d{2}-\d{2} \d{2}:\d{2}:\d{2})", std::regex::icase),
        // 시간만: 12:00:00
        std::regex(R"(\d{2}:\d{2}:\d{2}(?:\.\d+)?)", std::regex::icase)
    };
    
    std::string lineStr{line};
    std::smatch match;
    
    for (const auto& pattern : timestampPatterns)
    {
        if (std::regex_search(lineStr, match, pattern))
        {
            return match.str();
        }
    }
    
    return "";
}

std::string LogParser::extractMessage(std::string_view line, std::string_view level) const
{
    std::string lineStr{line};
    
    // 타임스탬프 제거
    std::string timestamp = extractTimestamp(line);
    if (!timestamp.empty())
    {
        size_t pos = lineStr.find(timestamp);
        if (pos != std::string::npos)
        {
            lineStr.erase(pos, timestamp.length());
        }
    }
    
    // 로그 레벨 제거 (다양한 형태 지원)
    if (level != "UNKNOWN")
    {
        // 정확한 레벨명 제거 시도
        size_t pos = lineStr.find(level);
        if (pos != std::string::npos)
        {
            lineStr.erase(pos, level.length());
        }
        else
        {
            // WARNING의 경우 WARN도 찾아서 제거
            if (level == "WARNING")
            {
                pos = lineStr.find("WARN");
                if (pos != std::string::npos)
                {
                    lineStr.erase(pos, 4); // "WARN" 길이
                }
            }
        }
    }
    
    // 공통 구분자 제거 (대괄호, 콜론, 하이픈 등)
    static const std::regex cleanupPatterns[] = {
        std::regex(R"(^\s*[-\[\]:|]+\s*)", std::regex::icase),
        std::regex(R"(\s*[-\[\]:|]+\s*$)", std::regex::icase)
    };
    
    for (const auto& pattern : cleanupPatterns)
    {
        lineStr = std::regex_replace(lineStr, pattern, "");
    }
    
    return trim(lineStr);
}

bool LogParser::containsKeyword(std::string_view line, std::string_view keyword) const noexcept
{
    if (keyword.empty())
    {
        return false;
    }
    
    return line.find(keyword) != std::string_view::npos;
}

std::string LogParser::trim(std::string_view str) const
{
    const auto start = str.find_first_not_of(" \t\n\r");
    if (start == std::string_view::npos)
    {
        return "";
    }
    
    const auto end = str.find_last_not_of(" \t\n\r");
    return std::string{str.substr(start, end - start + 1)};
} 