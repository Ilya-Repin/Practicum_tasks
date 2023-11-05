cmake_minimum_required(VERSION 3.11)

project(game_server CXX)
set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)

conan_basic_setup(TARGETS)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)



set(BOOST_LIB CONAN_PKG::boost)
set(ZLIB_LIB Threads::Threads)
set(CATCH2LIB CONAN_PKG::catch2)

# Добавляем библиотеку, указывая, что она статическая.
add_library(Model_lib STATIC
        src/model/model.h
        src/model/model.cpp
        src/model/dog.cpp
        src/model/dog.h
        src/model/loot_generator.h
        src/model/loot_generator.cpp
        src/extra_data.h
        src/extra_data.cpp
        src/model/game_session.cpp
        src/model/game_session.h
        )


# Добавляем сторонние библиотеки. Указываем видимость PUBLIC, т. к.
# они должны быть ввидны и в библиотеке Model_lib и в зависимостях.
target_include_directories(Model_lib PUBLIC ${BOOST_LIB})
target_link_libraries(Model_lib PUBLIC ${BOOST_LIB} ${ZLIB_LIB})


add_executable(game_server
        src/main.cpp
        src/http_server/http_server.cpp
        src/http_server/http_server.h
        src/sdk.h
        src/util/tagged.h
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
        src/app/Player.cpp
        src/app/Application.cpp
        src/app/Application.h
        src/util/command_line_parser.h
        src/util/command_line_parser.cpp
        src/util/ticker.h)

target_link_libraries(game_server PRIVATE ${BOOST_LIB} ${ZLIB_LIB} Model_lib)

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    include(CTest)
    include(${CONAN_BUILD_DIRS_CATCH2_DEBUG}/Catch.cmake)

    add_executable(game_server_tests
        tests/loot_generator_tests.cpp
        tests/model_tests.cpp
        )

    target_link_libraries(game_server_tests PRIVATE ${CATCH2LIB} Model_lib)
    catch_discover_tests(game_server_tests)
endif()