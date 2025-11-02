cmake_minimum_required(VERSION 3.10)

project(Construction VERSION 0.1)

set(CURSES_NEED_NCURSES TRUE)
find_package(Curses REQUIRED)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED true)

include(FetchContent)
FetchContent_Declare(
	ftxui
	GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
	GIT_TAG main
)
FetchContent_MakeAvailable(ftxui)

add_executable(construction
	src/main.cpp
)


target_include_directories(construction PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(construction PRIVATE
        ftxui::screen
	ftxui::dom
	ftxui::component
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(construction PRIVATE -Wall -Wextra -Wpedantic)
elseif(MSVC)
    target_compile_options(construction PRIVATE /W4)
endif()
