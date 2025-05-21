cmake_minimum_required(VERSION 3.5)
project(lites LANGUAGES C)

# Optionally allow building out-of-tree archives.
set(LITES_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build/lites-1.1.u3" CACHE PATH "Extracted lites source")

# Collect sources for server and emulator if the directory exists
if(EXISTS "${LITES_SRC_DIR}")
    file(GLOB_RECURSE SERVER_SRC
        "${LITES_SRC_DIR}/server/*.c")
    file(GLOB_RECURSE EMULATOR_SRC
        "${LITES_SRC_DIR}/emulator/*.c")

    add_executable(lites_server ${SERVER_SRC})
    target_include_directories(lites_server PRIVATE
        "${LITES_SRC_DIR}/include"
        "${LITES_SRC_DIR}/server")
    target_compile_options(lites_server PRIVATE ${CFLAGS})
    target_link_options(lites_server PRIVATE ${LDFLAGS})

    add_executable(lites_emulator ${EMULATOR_SRC})
    target_include_directories(lites_emulator PRIVATE
        "${LITES_SRC_DIR}/include"
        "${LITES_SRC_DIR}/emulator")
    target_compile_options(lites_emulator PRIVATE ${CFLAGS})
    target_link_options(lites_emulator PRIVATE ${LDFLAGS})
endif()

