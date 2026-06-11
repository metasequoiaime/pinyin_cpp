cmake_minimum_required(VERSION 3.25.0)
project(pinyin_cpp VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_definitions(
    /D_UNICODE=1
    /DUNICODE=1
)

# fmt
find_package(fmt REQUIRED)
find_package(SQLite3 REQUIRED)

set(MY_EXECUTABLE_NAME "pinyin_cpp")
set(
  SOURCE_FILES
  "./src/main.cpp"
  "./src/pinyin_query.cpp"
)

add_executable(${MY_EXECUTABLE_NAME} ${SOURCE_FILES})

target_include_directories(${MY_EXECUTABLE_NAME} PRIVATE "./src")

target_link_libraries(${MY_EXECUTABLE_NAME} PRIVATE fmt::fmt SQLite3::SQLite3)
