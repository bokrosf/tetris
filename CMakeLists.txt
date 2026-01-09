cmake_minimum_required(VERSION 3.29.0)
project(tetris)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)

FetchContent_Declare(
  sdl3
  URL https://github.com/libsdl-org/SDL/archive/refs/tags/release-3.2.28.tar.gz
)

FetchContent_Declare(
  json
  URL https://github.com/nlohmann/json/archive/refs/tags/v3.12.0.tar.gz
)

set(SDL_LIBURING OFF)
set(SDL_TEST_LIBRARY OFF)
set(SDL_TESTS OFF)
FetchContent_MakeAvailable(sdl3 json)

add_executable("${PROJECT_NAME}")
add_subdirectory(src)
target_compile_options("${PROJECT_NAME}" PRIVATE -Wall)

target_link_libraries("${PROJECT_NAME}"
  PRIVATE SDL3::SDL3
  PRIVATE nlohmann_json::nlohmann_json
)

target_include_directories("${PROJECT_NAME}"
  PRIVATE "src"
  PRIVATE "${sdl3_BINARY_DIR}/include"
  PRIVATE "${json_BINARY_DIR}/include"
)

file(CREATE_LINK "${PROJECT_SOURCE_DIR}/asset" "${PROJECT_BINARY_DIR}/asset" SYMBOLIC)
