cmake_minimum_required(VERSION 3.10)
project(ProtocolOrganizationEDII C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -O2)
endif()

include_directories(include)

# Versión Modular
add_executable(protocol_organizer
    src/main.c
    src/graph.c
    src/protocol_service.c
    src/file_parser.c
    src/ui_view.c
)

# Versión Standalone Monolítica
add_executable(protocol_organizer_standalone
    protocol_organizer.c
)
