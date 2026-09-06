cmake_minimum_required(VERSION 3.25.0)
project(quanpin_cpp VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_compile_definitions(_UNICODE=1 UNICODE=1)

# fmt
find_package(fmt REQUIRED)
find_package(SQLite3 REQUIRED)

set(MY_EXECUTABLE_NAME "quanpin_cpp")
set(
  SOURCE_FILES
  "./src/main.cpp"
  "./src/quanpin_query.cpp"
  "./src/shuangpin_query.cpp"
)

add_executable(${MY_EXECUTABLE_NAME} ${SOURCE_FILES})

target_include_directories(${MY_EXECUTABLE_NAME} PRIVATE "./src")

target_link_libraries(${MY_EXECUTABLE_NAME} PRIVATE fmt::fmt SQLite::SQLite3)

include(CTest)
if(BUILD_TESTING)
    add_executable(query_tests tests/query_tests.cpp src/quanpin_query.cpp src/shuangpin_query.cpp)
    target_include_directories(query_tests PRIVATE src)
    target_link_libraries(query_tests PRIVATE fmt::fmt SQLite::SQLite3)
    add_test(NAME query_regressions COMMAND query_tests)
endif()
