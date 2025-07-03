#pragma once

#include <filesystem>
#include <fstream>
#include <optional>
#include <string>

/**
 * @brief 로그 파일을 읽고 관리하는 클래스
 * 
 * C++17 RAII 패턴을 사용하여 안전한 파일 처리를 제공합니다.
 * std::filesystem과 std::ifstream을 활용하여 현대적인 파일 I/O를 구현합니다.
 */
class LogFileReader
{
private:
    std::filesystem::path filePath_;
    std::ifstream fileStream_;
    bool isValid_;
    size_t fileSize_;
    size_t currentLine_;

public:
    /**
     * @brief 파일 경로를 받아 LogFileReader 객체를 생성합니다
     * @param path 읽을 로그 파일의 경로
     */
    explicit LogFileReader(const std::filesystem::path& path);

    /**
     * @brief 복사 생성자 (삭제됨)
     * 파일 스트림은 복사할 수 없으므로 복사 생성자를 삭제합니다
     */
    LogFileReader(const LogFileReader&) = delete;

    /**
     * @brief 복사 할당 연산자 (삭제됨)
     * 파일 스트림은 복사할 수 없으므로 복사 할당 연산자를 삭제합니다
     */
    LogFileReader& operator=(const LogFileReader&) = delete;

    /**
     * @brief 이동 생성자
     * @param other 이동할 LogFileReader 객체
     */
    LogFileReader(LogFileReader&& other) noexcept;

    /**
     * @brief 이동 할당 연산자
     * @param other 이동할 LogFileReader 객체
     * @return LogFileReader& 참조
     */
    LogFileReader& operator=(LogFileReader&& other) noexcept;

    /**
     * @brief 소멸자 - RAII 패턴으로 자동 파일 닫기
     */
    ~LogFileReader() = default;

    /**
     * @brief 파일이 유효한지 확인합니다
     * @return bool 파일이 유효하면 true, 그렇지 않으면 false
     */
    [[nodiscard]] bool isValid() const noexcept;

    /**
     * @brief 파일에서 한 줄을 읽습니다
     * @return std::optional<std::string> 읽은 줄의 내용, 파일 끝이면 std::nullopt
     */
    [[nodiscard]] std::optional<std::string> readLine();

    /**
     * @brief 파일 크기를 바이트 단위로 반환합니다
     * @return size_t 파일 크기
     */
    [[nodiscard]] size_t getFileSize() const noexcept;

    /**
     * @brief 현재 읽은 줄 번호를 반환합니다
     * @return size_t 현재 줄 번호 (1부터 시작)
     */
    [[nodiscard]] size_t getCurrentLine() const noexcept;

    /**
     * @brief 파일 스트림을 처음으로 되돌립니다
     */
    void reset();

    /**
     * @brief 파일 경로를 반환합니다
     * @return const std::filesystem::path& 파일 경로
     */
    [[nodiscard]] const std::filesystem::path& getFilePath() const noexcept;

private:
    /**
     * @brief 파일 크기를 계산하는 내부 메서드
     */
    void calculateFileSize();

    /**
     * @brief 파일 유효성을 검사하는 내부 메서드
     */
    void validateFile();
}; 