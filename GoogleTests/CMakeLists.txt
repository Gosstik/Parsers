# 'Google_test' is the subproject name
project(GoogleTests)

# 'lib' is the folder with Google Test sources
add_subdirectory(googletest)

include_directories(${gtest_SOURCE_DIR}/include
        ${gtest_SOURCE_DIR})

set(LOCAL_GTEST_DIR "${CMAKE_SOURCE_DIR}/GoogleTests")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${LOCAL_GTEST_DIR}/bin")

set(test_source
        ${LOCAL_GTEST_DIR}/src/main_test.cpp
        ${LOCAL_GTEST_DIR}/src/TestsEarley.cpp
        ${LOCAL_GTEST_DIR}/src/TestsLR.cpp)

# 'Google_Tests_run' is the target name
# 'test1.cpp tests2.cpp' are source files with tests
add_executable(Google_Tests_run ${source} ${test_source})

target_link_libraries(Google_Tests_run gtest gtest_main)