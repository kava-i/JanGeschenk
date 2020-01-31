cmake_minimum_required(VERSION 3.15)
project(JanGeschenk)

if(NOT EXISTS $ENV{HOME}/ciavelli_packages/ciavelli.cmake)
    execute_process(COMMAND pip --user install ciavelli)
endif()
include($ENV{HOME}/ciavelli_packages/ciavelli.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
SET(CMAKE_CXX_FLAGS "-Wall -Werror -O3")
set(EXECUTABLE_OUTPUT_PATH ../bin)



add_executable(basic_compile main.cpp)
target_link_libraries(basic_compile PUBLIC pthread)
target_link_libraries(basic_compile PUBLIC boost_system)
add_git_cmake_project(NAME websocketpp USEDBY basic_compile
    URL "https://github.com/zaphoyd/websocketpp"
    BRANCH "develop")

install(FILES Webconsole.hpp Webgame.hpp DESTINATION ${CMAKE_INSTALL_PREFIX}/include/JanGeschenk)
