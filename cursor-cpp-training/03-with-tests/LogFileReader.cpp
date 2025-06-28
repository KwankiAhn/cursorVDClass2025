#include "LogFileReader.hpp"
#include <iostream>
#include <stdexcept>

namespace LogAnalyzer {

LogFileReader::LogFileReader(const std::string& filePath) 
    : filePath_(filePath), isValid_(false) {
    validateFile();
    if (isValid_) {
        fileStream_.open(filePath_);
        isValid_ = fileStream_.is_open();
    }
}

void LogFileReader::validateFile() {
    try {
        if (filePath_.empty()) {
            throw std::invalid_argument("파일 경로가 비어있습니다");
        }
        
        if (!std::filesystem::exists(filePath_)) {
            throw std::runtime_error("파일이 존재하지 않습니다: " + filePath_);
        }
        
        if (!std::filesystem::is_regular_file(filePath_)) {
            throw std::runtime_error("일반 파일이 아닙니다: " + filePath_);
        }
        
        isValid_ = true;
    } catch (const std::exception& e) {
        std::cerr << "파일 검증 실패: " << e.what() << std::endl;
        isValid_ = false;
    }
}

bool LogFileReader::isValid() const noexcept {
    return isValid_;
}

std::vector<std::string> LogFileReader::readAllLines() {
    std::vector<std::string> lines;
    
    if (!isValid_) {
        return lines;
    }
    
    // 파일 스트림 재설정
    fileStream_.clear();
    fileStream_.seekg(0, std::ios::beg);
    
    std::string line;
    while (std::getline(fileStream_, line)) {
        lines.emplace_back(std::move(line));
    }
    
    return lines;
}

std::optional<std::string> LogFileReader::readNextLine() {
    if (!isValid_ || !fileStream_.is_open()) {
        return std::nullopt;
    }
    
    std::string line;
    if (std::getline(fileStream_, line)) {
        return line;
    }
    
    return std::nullopt;
}

std::uintmax_t LogFileReader::getFileSize() const {
    if (!isValid_) {
        return 0;
    }
    
    try {
        return std::filesystem::file_size(filePath_);
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "파일 크기 조회 실패: " << e.what() << std::endl;
        return 0;
    }
}

std::string LogFileReader::getFilePath() const noexcept {
    return filePath_;
}

} // namespace LogAnalyzer 