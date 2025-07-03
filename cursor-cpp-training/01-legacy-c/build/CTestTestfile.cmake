# CMake generated Testfile for 
# Source directory: /Users/ahnkwanki/src-github/cursorVDClaas2025/cursor-cpp-training/01-legacy-c
# Build directory: /Users/ahnkwanki/src-github/cursorVDClaas2025/cursor-cpp-training/01-legacy-c/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[UnitTests]=] "/Users/ahnkwanki/src-github/cursorVDClaas2025/cursor-cpp-training/01-legacy-c/build/log_analyzer_tests")
set_tests_properties([=[UnitTests]=] PROPERTIES  _BACKTRACE_TRIPLES "/Users/ahnkwanki/src-github/cursorVDClaas2025/cursor-cpp-training/01-legacy-c/CMakeLists.txt;120;add_test;/Users/ahnkwanki/src-github/cursorVDClaas2025/cursor-cpp-training/01-legacy-c/CMakeLists.txt;0;")
add_test([=[BasicTest]=] "/Users/ahnkwanki/src-github/cursorVDClaas2025/cursor-cpp-training/01-legacy-c/build/log_analyzer" "../../sample-logs/sample.log")
set_tests_properties([=[BasicTest]=] PROPERTIES  _BACKTRACE_TRIPLES "/Users/ahnkwanki/src-github/cursorVDClaas2025/cursor-cpp-training/01-legacy-c/CMakeLists.txt;121;add_test;/Users/ahnkwanki/src-github/cursorVDClaas2025/cursor-cpp-training/01-legacy-c/CMakeLists.txt;0;")
subdirs("_deps/catch2-build")
