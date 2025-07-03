#pragma once

#include "LogStats.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>

/**
 * @brief HTML 리포트 생성기 클래스
 * LogStats 데이터를 받아서 HTML 형식의 리포트를 생성
 */
class HTMLReporter
{
public:
    /**
     * @brief 생성자
     * @param outputPath HTML 파일 출력 경로
     */
    explicit HTMLReporter(const std::string& outputPath);
    
    /**
     * @brief HTML 리포트 생성
     * @param stats 로그 통계 객체
     * @param logFilePath 원본 로그 파일 경로
     * @return bool 성공 여부
     */
    bool generateReport(const LogStats& stats, const std::string& logFilePath);
    
private:
    std::string outputPath_;
    
    /**
     * @brief HTML 템플릿 생성
     * @param stats 로그 통계 객체
     * @param logFilePath 원본 로그 파일 경로
     * @return std::string HTML 콘텐츠
     */
    std::string generateHTMLContent(const LogStats& stats, const std::string& logFilePath);
    
    /**
     * @brief 현재 시간을 문자열로 반환
     * @return std::string 현재 시간
     */
    std::string getCurrentTime();
    
    /**
     * @brief 에러 로그 목록을 HTML로 변환
     * @param errorEntries 에러 로그 엔트리들
     * @return std::string HTML 형식의 에러 목록
     */
    std::string generateErrorList(const std::vector<LogEntry>& errorEntries);
    
    /**
     * @brief 통계 카드 HTML 생성
     * @param stats 로그 통계 객체
     * @return std::string HTML 형식의 통계 카드
     */
    std::string generateStatsCards(const LogStats& stats);
    
    /**
     * @brief CSS 스타일 생성
     * @return std::string CSS 콘텐츠
     */
    std::string generateCSS();
}; 