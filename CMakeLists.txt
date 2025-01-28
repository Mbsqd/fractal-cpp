cmake_minimum_required(VERSION 3.10...3.31)
project(FractalApp LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(main src/main.cpp)

set(OUTPUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/output")
add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_DIR}
    COMMENT "Created directory: ${OUTPUT_DIR}"
)

target_include_directories(main PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_features(main PRIVATE cxx_std_20)
target_link_libraries(main PRIVATE SFML::Graphics)
