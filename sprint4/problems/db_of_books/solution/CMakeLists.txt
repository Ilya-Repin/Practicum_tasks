cmake_minimum_required(VERSION 3.11)

project(postgres CXX)
set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
conan_basic_setup(TARGETS)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(book_manager src/main.cpp src/books_db.h src/books_db.cpp src/json_parser.cpp src/json_parser.h)

target_include_directories(book_manager PUBLIC CONAN_PKG::boost)
target_link_libraries(book_manager  PUBLIC CONAN_PKG::libpqxx CONAN_PKG::boost)
