#include "LogFileReader.hpp"
#include "LogParser.hpp"
#include "LogStats.hpp"
#include "HTMLReporter.hpp"
#include <iostream>
#include <iomanip>
#include <map>
#include <sstream>
#include <string_view>
#include <chrono>

/**
 * @brief 로그 레벨을 감지하는 함수
 * @param line 로그 라인
 * @return std::string_view 감지된 로그 레벨
 */
[[nodiscard]] constexpr std::string_view detectLogLevel(std::string_view line) noexcept
{
    if (line.find("ERROR") != std::string_view::npos)
    {
        return "ERROR";
    }
    else if (line.find("WARN") != std::string_view::npos)
    {
        return "WARNING";
    }
    else if (line.find("INFO") != std::string_view::npos)
    {
        return "INFO";
    }
    else if (line.find("DEBUG") != std::string_view::npos)
    {
        return "DEBUG";
    }
    
    return "UNKNOWN";
}

/**
 * @brief 파일 크기를 읽기 쉬운 형태로 포맷하는 함수
 * @param bytes 바이트 크기
 * @return std::string 포맷된 크기 문자열
 */
[[nodiscard]] std::string formatFileSize(size_t bytes)
{
    constexpr double KB = 1024.0;
    constexpr double MB = KB * 1024.0;
    constexpr double GB = MB * 1024.0;
    
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    
    if (bytes >= GB)
    {
        oss << (bytes / GB) << " GB";
    }
    else if (bytes >= MB)
    {
        oss << (bytes / MB) << " MB";
    }
    else if (bytes >= KB)
    {
        oss << (bytes / KB) << " KB";
    }
    else
    {
        oss << bytes << " bytes";
    }
    
    return oss.str();
}

/**
 * @brief 키워드 검색 함수
 * @param line 로그 라인
 * @param keyword 검색할 키워드
 * @return bool 키워드가 발견되면 true
 */
[[nodiscard]] bool containsKeyword(std::string_view line, std::string_view keyword) noexcept
{
    if (keyword.empty())
    {
        return false;
    }
    
    return line.find(keyword) != std::string_view::npos;
}

int main(int argc, char* argv[])
{
    try
    {
        // 인자 검사
        if (argc < 2)
        {
            std::cerr << "사용법: " << argv[0] << " <로그파일> [검색키워드] [--output-json <파일경로>] [--html-output <파일경로>]\n";
            std::cerr << "예시: " << argv[0] << " sample.log database --output-json results.json --html-output report.html\n";
            return 1;
        }
        
        const std::filesystem::path logFilePath{argv[1]};
        std::string keyword;
        std::string jsonOutputFile;
        std::string htmlOutputFile;
        
        // 명령행 인자 파싱
        for (int i = 2; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (arg == "--output-json" && i + 1 < argc)
            {
                jsonOutputFile = argv[i + 1];
                ++i;  // 다음 인자는 파일명이므로 건너뛰기
            }
            else if (arg == "--html-output" && i + 1 < argc)
            {
                htmlOutputFile = argv[i + 1];
                ++i;  // 다음 인자는 파일명이므로 건너뛰기
            }
            else if (arg != "--output-json" && arg != "--html-output" && 
                     jsonOutputFile.empty() && htmlOutputFile.empty())
            {
                // 첫 번째 비옵션 인자는 키워드로 처리
                if (keyword.empty())
                {
                    keyword = arg;
                }
            }
        }
        
        // 시작 시간 측정
        auto startTime = std::chrono::steady_clock::now();
        
        // 객체 생성
        LogFileReader reader{logFilePath};
        LogParser parser;
        LogStats stats;
        
        if (!reader.isValid())
        {
            std::cerr << "파일을 열 수 없습니다: " << logFilePath << '\n';
            return 1;
        }
        
        // 파일 정보 출력
        std::cout << "🔍 로그 파일 분석 시작: " << reader.getFilePath() << '\n';
        std::cout << "📁 파일 크기: " << formatFileSize(reader.getFileSize()) << '\n';
        
        if (!keyword.empty())
        {
            std::cout << "🔎 검색 키워드: " << keyword << '\n';
        }
        
        if (!jsonOutputFile.empty())
        {
            std::cout << "📄 JSON 출력 파일: " << jsonOutputFile << '\n';
        }
        
        if (!htmlOutputFile.empty())
        {
            std::cout << "🌐 HTML 출력 파일: " << htmlOutputFile << '\n';
        }
        
        std::cout << '\n';
        
        // 파일 읽기 및 분석
        while (auto line = reader.readLine())
        {
            LogEntry entry = parser.parseLine(*line);
            
            if (!keyword.empty())
            {
                stats.addEntryWithKeyword(entry, keyword);
                
                // 키워드 매칭된 라인 출력
                if (parser.containsKeyword(entry.rawLine, keyword))
                {
                    std::cout << "[키워드 발견] " << entry.rawLine << '\n';
                }
            }
            else
            {
                stats.addEntry(entry);
            }
            
            // ERROR 라인 출력
            if (entry.level == "ERROR")
            {
                std::cout << "[ERROR] " << entry.rawLine << '\n';
            }
        }
        
        // 처리 시간 계산
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        stats.setProcessingTime(duration.count());
        
        // 통계 계산 완료
        stats.finalize();
        
        // 통계 출력
        stats.printStatistics();
        
        // JSON 파일 저장
        if (!jsonOutputFile.empty())
        {
            if (!stats.saveToJsonFile(jsonOutputFile))
            {
                std::cerr << "❌ JSON 파일 저장에 실패했습니다.\n";
                return 1;
            }
        }
        
        // HTML 리포트 생성
        if (!htmlOutputFile.empty())
        {
            HTMLReporter htmlReporter(htmlOutputFile);
            if (!htmlReporter.generateReport(stats, logFilePath.string()))
            {
                std::cerr << "❌ HTML 리포트 생성에 실패했습니다.\n";
                return 1;
            }
        }
        
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "❌ 오류 발생: " << e.what() << '\n';
        return 1;
    }
    catch (...)
    {
        std::cerr << "❌ 알 수 없는 오류가 발생했습니다.\n";
        return 1;
    }
} 