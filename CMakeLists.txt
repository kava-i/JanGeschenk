cmake_minimum_required(VERSION 3.16)
project(JanGeschenk)
if(EXISTS $ENV{HOME}/ciavelli_packages/ciavelli.cmake)
    include($ENV{HOME}/ciavelli_packages/ciavelli.cmake)
else()
    file(MAKE_DIRECTORY .ciavelli_tmp)
    file(WRITE .ciavelli_tmp/CMakeLists.txt "cmake_minimum_required(VERSION 3.10)\nproject(Ciavelli)\ninclude(${CMAKE_ROOT}/Modules/ExternalProject.cmake)\nset(ENV{CLICOLOR_FORCE} 1)\nExternalProject_Add(Ciavelli GIT_REPOSITORY \"https://github.com/ShadowItaly/Ciavelli\")")
    execute_process(COMMAND ${CMAKE_COMMAND} . WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/.ciavelli_tmp)
    execute_process(COMMAND ${CMAKE_COMMAND} --build . WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/.ciavelli_tmp)
    file(REMOVE_RECURSE .ciavelli_tmp)
    file(REMOVE_RECURSE Ciavelli)
    include($ENV{HOME}/ciavelli_packages/ciavelli.cmake)
    string(ASCII 27 Esc)
    message("${Esc}[92mInstalled Ciavelli Package manager successfully${Esc}[0m")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
SET(CMAKE_CXX_FLAGS "-Wall -Werror -O3")
set(EXECUTABLE_OUTPUT_PATH ../bin)



add_executable(basic_compile main.cpp)
target_link_libraries(basic_compile PUBLIC pthread)
add_git_cmake_project(NAME websocketpp USEDBY basic_compile
    URL "https://github.com/zaphoyd/websocketpp"
    BRANCH "develop")

install(FILES Webconsole.hpp Webgame.hpp DESTINATION ${CMAKE_INSTALL_PREFIX}/include/JanGeschenk)
