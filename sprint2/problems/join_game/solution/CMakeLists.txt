cmake_minimum_required(VERSION 3.9)

project(game_server CXX)
set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

find_package(Boost 1.78.0 REQUIRED)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif ()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(game_server

	src/main.cpp
		src/http_server/http_server.cpp
		src/http_server/http_server.h
	src/sdk.h
        src/model/model.h
        src/model/model.cpp
	src/tagged.h
	src/boost_json.cpp
		src/json_loader/json_loader.h
		src/json_loader/json_loader.cpp
		src/http_handler/request_handler.cpp
		src/http_handler/request_handler.h
		src/logger/logger.cpp
		src/logger/logging_request_handler.h
		src/http_handler/api_handler.h
		src/http_handler/api_handler.cpp
		src/app/Player.h
		src/app/Player.cpp src/app/Application.cpp src/app/Application.h src/model/game_session.cpp src/model/game_session.h src/model/game_session.h src/model/dog.cpp src/model/dog.h)
  


target_include_directories(game_server PRIVATE CONAN_PKG::boost)
target_link_libraries(game_server PRIVATE CONAN_PKG::boost PRIVATE Threads::Threads)
