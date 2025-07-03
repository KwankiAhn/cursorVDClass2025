- **목표**: `01-legacy-c` 프로젝트에 로그 분석 결과를 JSON 파일로 저장하는 기능을 추가한다.
- **요구사항**:
  1. `main.cpp`에 `--output-json <파일경로>` 커맨드라인 인자를 추가한다.
  2. `nlohmann/json` 라이브러리를 사용하여 `LogStats`의 통계 정보를 JSON으로 변환하고 파일에 저장한다.
  3. `CMakeLists.txt`를 수정하여 `nlohmann/json` 라이브러리를 `FetchContent`로 연동한다.

  