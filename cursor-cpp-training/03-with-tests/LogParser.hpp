#pragma once

#include <string>
#include <vector>
#include <regex>
#include <unordered_map>

namespace LogAnalyzer {

enum class LogLevel {
    UNKNOWN = 0,
    ERROR = 1,
    WARNING = 2,
    INFO = 3,
    DEBUG = 4
};

struct LogEntry {
    std::string originalLine;
    LogLevel level;
    std::string timestamp;
    std::string message;
    
    LogEntry(const std::string& line, LogLevel lvl, 
             const std::string& ts = "", const std::string& msg = "")
        : originalLine(line), level(lvl), timestamp(ts), message(msg) {}
};

class LogParser {
public:
    LogParser();
    ~LogParser() = default;

    // 단일 라인 파싱
    LogEntry parseLine(const std::string& line) const;
    
    // 여러 라인 파싱
    std::vector<LogEntry> parseLines(const std::vector<std::string>& lines) const;
    
    // 키워드 검색
    std::vector<LogEntry> filterByKeyword(const std::vector<LogEntry>& entries, 
                                         const std::string& keyword) const;
    
    // 로그 레벨별 필터링
    std::vector<LogEntry> filterByLevel(const std::vector<LogEntry>& entries, 
                                       LogLevel level) const;
    
    // 로그 레벨 문자열 변환
    static std::string logLevelToString(LogLevel level);
    static LogLevel stringToLogLevel(const std::string& levelStr);

private:
    std::unordered_map<std::string, LogLevel> levelMap_;
    std::regex timestampRegex_;
    
    void initializeLevelMap();
    LogLevel detectLogLevel(const std::string& line) const;
    std::string extractTimestamp(const std::string& line) const;
    std::string extractMessage(const std::string& line) const;
};

} // namespace LogAnalyzer 