#include "LogFileReader.hpp"
#include "LogParser.hpp"
#include "LogStats.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    // 명령줄 인자 확인
    if (argc < 2)
    {
        std::cout << "사용법: " << argv[0] << " <로그파일경로> [검색키워드]" << std::endl;
        return 1;
    }
    
    const std::string filePath = argv[1];
    
    try
    {
        // LogFileReader 객체 생성
        LogFileReader reader(filePath);
        
        // 파일 유효성 검사
        if (!reader.isValid())
        {
            std::cerr << "오류: 파일이 존재하지 않거나 읽을 수 없습니다: " << filePath << std::endl;
            return 1;
        }
        
        std::cout << "로그 파일 분석 시작: " << filePath << std::endl;
        
        // 파일 정보 출력
        if (auto fileSize = reader.getFileSize())
        {
            std::cout << "파일 크기: " << *fileSize << " 바이트" << std::endl;
        }
        
        // 파일 열기
        if (!reader.open())
        {
            std::cerr << "오류: 파일을 열 수 없습니다: " << filePath << std::endl;
            return 1;
        }
        
        std::cout << "\n=== 로그 파싱 및 분석 시작 ===" << std::endl;
        
        // 모든 라인 읽기
        auto lines = reader.readAllLines();
        std::cout << "총 " << lines.size() << "줄을 읽었습니다." << std::endl;
        
        // 로그 파서 생성 및 파싱
        LogParser parser;
        auto entries = parser.parseLines(lines);
        std::cout << "파싱 완료: " << entries.size() << "개의 유효한 로그 엔트리" << std::endl;
        std::cout << "파싱 성공률: " << std::fixed << std::setprecision(1) 
                  << (parser.getParseSuccessRate() * 100.0) << "%" << std::endl;
        
        // 통계 계산
        LogStats stats;
        stats.calculateStatistics(entries);
        
        // 통계 출력
        stats.printStatistics(true); // 상세 정보 포함
        
        // ERROR 레벨 로그들만 출력
        auto errorEntries = parser.filterByLevel(entries, LogLevel::ERROR);
        if (!errorEntries.empty())
        {
            std::cout << "\n=== ERROR 레벨 로그들 ===" << std::endl;
            for (const auto& entry : errorEntries)
            {
                std::cout << "[라인 " << entry.lineNumber << "] " << entry << std::endl;
            }
        }
        
        // 키워드가 제공된 경우 키워드 검색
        if (argc >= 3)
        {
            std::string keyword = argv[2];
            auto keywordEntries = parser.filterByKeyword(entries, keyword);
            
            std::cout << "\n=== 키워드 검색 결과: '" << keyword << "' ===" << std::endl;
            std::cout << "발견된 항목: " << keywordEntries.size() << "개" << std::endl;
            
            for (const auto& entry : keywordEntries)
            {
                std::cout << "[라인 " << entry.lineNumber << "] " << entry << std::endl;
            }
        }
        
        std::cout << "\n=== 분석 완료 ===" << std::endl;
        
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "예외 발생: " << e.what() << std::endl;
        return 1;
    }
} 