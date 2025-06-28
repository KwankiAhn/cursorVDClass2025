#include "LogFileReader.hpp"
#include "LogParser.hpp"
#include "LogStats.hpp"
#include <iostream>
#include <string>
#include <exception>

using namespace LogAnalyzer;

void printUsage(const std::string& programName) {
    std::cout << "사용법: " << programName << " <로그파일> [옵션]\n";
    std::cout << "옵션:\n";
    std::cout << "  --keyword <키워드>    특정 키워드를 포함한 로그만 출력\n";
    std::cout << "  --level <레벨>        특정 레벨의 로그만 출력 (ERROR, WARNING, INFO, DEBUG)\n";
    std::cout << "  --json               결과를 JSON 형태로 출력\n";
    std::cout << "  --detailed           상세 통계 출력\n";
    std::cout << "  --help               도움말 출력\n";
}

int main(int argc, char* argv[]) {
    try {
        // 인자 검사
        if (argc < 2) {
            printUsage(argv[0]);
            return 1;
        }
        
        std::string filePath = argv[1];
        std::string keyword;
        std::string levelFilter;
        bool jsonOutput = false;
        bool detailedOutput = false;
        
        // 옵션 파싱
        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help") {
                printUsage(argv[0]);
                return 0;
            } else if (arg == "--keyword" && i + 1 < argc) {
                keyword = argv[++i];
            } else if (arg == "--level" && i + 1 < argc) {
                levelFilter = argv[++i];
            } else if (arg == "--json") {
                jsonOutput = true;
            } else if (arg == "--detailed") {
                detailedOutput = true;
            }
        }
        
        // 1. 파일 읽기
        std::cout << "로그 파일 분석 시작: " << filePath << std::endl;
        
        LogFileReader reader(filePath);
        if (!reader.isValid()) {
            std::cerr << "파일을 읽을 수 없습니다: " << filePath << std::endl;
            return 1;
        }
        
        auto lines = reader.readAllLines();
        std::cout << "파일 크기: " << reader.getFileSize() << " bytes" << std::endl;
        std::cout << "읽은 라인 수: " << lines.size() << std::endl;
        
        // 2. 로그 파싱
        LogParser parser;
        auto entries = parser.parseLines(lines);
        
        // 3. 필터링 (키워드)
        if (!keyword.empty()) {
            entries = parser.filterByKeyword(entries, keyword);
            std::cout << "키워드 '" << keyword << "' 필터링 후: " << entries.size() << " 라인" << std::endl;
        }
        
        // 4. 필터링 (로그 레벨)
        if (!levelFilter.empty()) {
            LogLevel level = LogParser::stringToLogLevel(levelFilter);
            if (level != LogLevel::UNKNOWN) {
                entries = parser.filterByLevel(entries, level);
                std::cout << "로그 레벨 '" << levelFilter << "' 필터링 후: " << entries.size() << " 라인" << std::endl;
            } else {
                std::cerr << "알 수 없는 로그 레벨: " << levelFilter << std::endl;
            }
        }
        
        // 5. 통계 계산 및 출력
        LogStats stats;
        auto statistics = stats.calculateStats(entries, filePath, reader.getFileSize());
        
        if (jsonOutput) {
            std::cout << stats.statsToJson(statistics) << std::endl;
        } else if (detailedOutput) {
            stats.printDetailedStats(statistics);
        } else {
            stats.printStats(statistics);
        }
        
        // 6. 특별한 출력 요청 처리
        if (!keyword.empty()) {
            stats.printKeywordMatches(parser.parseLines(reader.readAllLines()), keyword);
        }
        
        if (!levelFilter.empty()) {
            LogLevel level = LogParser::stringToLogLevel(levelFilter);
            if (level != LogLevel::UNKNOWN) {
                stats.printEntriesByLevel(parser.parseLines(reader.readAllLines()), level);
            }
        }
        
        // ERROR 로그가 있으면 항상 출력
        auto allEntries = parser.parseLines(reader.readAllLines());
        auto errorEntries = parser.filterByLevel(allEntries, LogLevel::ERROR);
        if (!errorEntries.empty() && keyword.empty() && levelFilter.empty()) {
            stats.printEntriesByLevel(allEntries, LogLevel::ERROR);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "오류 발생: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "알 수 없는 오류가 발생했습니다." << std::endl;
        return 1;
    }
    
    return 0;
} 