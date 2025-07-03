#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <optional>
#include <cstdint>

/**
 * @brief 로그 파일 읽기 및 검증을 담당하는 클래스
 * 
 * C++17 표준을 사용하여 안전하고 효율적인 파일 처리를 제공합니다.
 * RAII 패턴을 적용하여 자동 리소스 관리를 수행합니다.
 */
class LogFileReader
{
public:
    /**
     * @brief LogFileReader 생성자
     * @param filePath 읽을 로그 파일의 경로
     */
    explicit LogFileReader(const std::string& filePath);
    
    /**
     * @brief LogFileReader 생성자 (filesystem::path 버전)
     * @param filePath 읽을 로그 파일의 경로
     */
    explicit LogFileReader(const std::filesystem::path& filePath);
    
    /**
     * @brief 소멸자
     */
    ~LogFileReader() = default;
    
    // 복사 생성자와 대입 연산자 삭제 (파일 핸들 때문에)
    LogFileReader(const LogFileReader&) = delete;
    LogFileReader& operator=(const LogFileReader&) = delete;
    
    // 이동 생성자와 이동 대입 연산자 허용
    LogFileReader(LogFileReader&&) = default;
    LogFileReader& operator=(LogFileReader&&) = default;
    
    /**
     * @brief 파일이 유효한지 확인
     * @return 파일이 존재하고 읽을 수 있으면 true
     */
    bool isValid() const noexcept;
    
    /**
     * @brief 파일을 열고 읽기 준비
     * @return 성공하면 true, 실패하면 false
     */
    bool open();
    
    /**
     * @brief 파일을 닫기
     */
    void close() noexcept;
    
    /**
     * @brief 다음 라인을 읽기
     * @return 라인 내용 (EOF에 도달하면 std::nullopt)
     */
    std::optional<std::string> readLine();
    
    /**
     * @brief 전체 파일 내용을 한 번에 읽기
     * @return 모든 라인을 담은 벡터
     */
    std::vector<std::string> readAllLines();
    
    /**
     * @brief 파일 경로 반환
     * @return 현재 파일 경로
     */
    const std::filesystem::path& getFilePath() const noexcept;
    
    /**
     * @brief 파일 크기 반환
     * @return 파일 크기 (바이트 단위), 오류 시 std::nullopt
     */
    std::optional<std::uintmax_t> getFileSize() const noexcept;
    
    /**
     * @brief 현재 라인 번호 반환
     * @return 현재 읽고 있는 라인 번호 (1부터 시작)
     */
    std::size_t getCurrentLineNumber() const noexcept;

private:
    std::filesystem::path filePath_;    ///< 파일 경로
    std::ifstream fileStream_;          ///< 파일 스트림
    std::size_t currentLineNumber_;     ///< 현재 라인 번호
    bool isOpen_;                       ///< 파일 열림 상태
    
    /**
     * @brief 파일 존재 여부와 읽기 권한 확인
     * @return 파일이 유효하면 true
     */
    bool validateFile() const noexcept;
}; 