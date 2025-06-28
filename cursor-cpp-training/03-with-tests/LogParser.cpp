#include "LogParser.hpp"
#include <algorithm>
#include <sstream>

namespace LogAnalyzer {

LogParser::LogParser() 
    : timestampRegex_(R"(\d{4}-\d{2}-\d{2}\s\d{2}:\d{2}:\d{2})") {
    initializeLevelMap();
}

void LogParser::initializeLevelMap() {
    levelMap_["ERROR"] = LogLevel::ERROR;
    levelMap_["ERR"] = LogLevel::ERROR;
    levelMap_["WARN"] = LogLevel::WARNING;
    levelMap_["WARNING"] = LogLevel::WARNING;
    levelMap_["INFO"] = LogLevel::INFO;
    levelMap_["DEBUG"] = LogLevel::DEBUG;
    levelMap_["DBG"] = LogLevel::DEBUG;
}

LogEntry LogParser::parseLine(const std::string& line) const {
    if (line.empty()) {
        return LogEntry(line, LogLevel::UNKNOWN);
    }
    
    LogLevel level = detectLogLevel(line);
    std::string timestamp = extractTimestamp(line);
    std::string message = extractMessage(line);
    
    return LogEntry(line, level, timestamp, message);
}

std::vector<LogEntry> LogParser::parseLines(const std::vector<std::string>& lines) const {
    std::vector<LogEntry> entries;
    entries.reserve(lines.size());
    
    for (const auto& line : lines) {
        entries.emplace_back(parseLine(line));
    }
    
    return entries;
}

std::vector<LogEntry> LogParser::filterByKeyword(const std::vector<LogEntry>& entries, 
                                                const std::string& keyword) const {
    if (keyword.empty()) {
        return entries;
    }
    
    std::vector<LogEntry> filtered;
    
    std::copy_if(entries.begin(), entries.end(), std::back_inserter(filtered),
                [&keyword](const LogEntry& entry) {
                    return entry.originalLine.find(keyword) != std::string::npos;
                });
    
    return filtered;
}

std::vector<LogEntry> LogParser::filterByLevel(const std::vector<LogEntry>& entries, 
                                              LogLevel level) const {
    std::vector<LogEntry> filtered;
    
    std::copy_if(entries.begin(), entries.end(), std::back_inserter(filtered),
                [level](const LogEntry& entry) {
                    return entry.level == level;
                });
    
    return filtered;
}

LogLevel LogParser::detectLogLevel(const std::string& line) const {
    // 대소문자 구분없이 검색하기 위해 대문자로 변환
    std::string upperLine = line;
    std::transform(upperLine.begin(), upperLine.end(), upperLine.begin(), ::toupper);
    
    for (const auto& [levelStr, level] : levelMap_) {
        if (upperLine.find(levelStr) != std::string::npos) {
            return level;
        }
    }
    
    return LogLevel::UNKNOWN;
}

std::string LogParser::extractTimestamp(const std::string& line) const {
    std::smatch match;
    if (std::regex_search(line, match, timestampRegex_)) {
        return match.str();
    }
    return "";
}

std::string LogParser::extractMessage(const std::string& line) const {
    // 간단한 메시지 추출 (타임스탬프와 로그 레벨 이후의 내용)
    std::istringstream iss(line);
    std::string token;
    std::vector<std::string> tokens;
    
    while (iss >> token) {
        tokens.push_back(token);
    }
    
    // 최소 3개 토큰이 있으면 (날짜, 시간, 레벨) 나머지를 메시지로 간주
    if (tokens.size() > 3) {
        std::ostringstream message;
        for (size_t i = 3; i < tokens.size(); ++i) {
            if (i > 3) message << " ";
            message << tokens[i];
        }
        return message.str();
    }
    
    return line; // 파싱 실패시 원본 반환
}

std::string LogParser::logLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::ERROR:   return "ERROR";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::INFO:    return "INFO";
        case LogLevel::DEBUG:   return "DEBUG";
        default:                return "UNKNOWN";
    }
}

LogLevel LogParser::stringToLogLevel(const std::string& levelStr) {
    std::string upper = levelStr;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    
    if (upper == "ERROR" || upper == "ERR") return LogLevel::ERROR;
    if (upper == "WARNING" || upper == "WARN") return LogLevel::WARNING;
    if (upper == "INFO") return LogLevel::INFO;
    if (upper == "DEBUG" || upper == "DBG") return LogLevel::DEBUG;
    
    return LogLevel::UNKNOWN;
}

} // namespace LogAnalyzer 