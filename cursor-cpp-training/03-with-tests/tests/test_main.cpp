// 이 파일은 Catch2::Catch2WithMain을 사용하므로 비어있어도 됩니다.
// 하지만 커스텀 설정이 필요한 경우를 위해 파일을 생성합니다.

#include <catch2/catch_test_macros.hpp>

// 전역 테스트 설정이 필요한 경우 여기에 추가할 수 있습니다.
TEST_CASE("테스트 환경 확인", "[setup]") {
    REQUIRE(true);
} 