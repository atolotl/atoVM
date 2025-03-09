cmake_minimum_required(VERSION 3.20)

file(GLOB SOURCE_FILES
    *.cc
    **/*.cc
    **/**/*.cc
)

add_executable(atoVM ${SOURCE_FILES})
target_compile_definitions(atoVM PUBLIC NOMINMAX)

target_include_directories(atoVM PUBLIC .)
target_link_libraries(atoVM fmt::fmt xbyak magic_enum::magic_enum)