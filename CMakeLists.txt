cmake_minimum_required(VERSION 3.5)
project(lites LANGUAGES C)
set(CMAKE_C_STANDARD 11)

# Allow selecting the build architecture with -DARCH=i686, ARCH=ppc64 or ARCH=ppc
set(ARCH "$ENV{ARCH}" CACHE STRING "Target architecture (x86_64, i686, ppc64 or ppc)")
set(CFLAGS "$ENV{CFLAGS}" CACHE STRING "Additional compiler flags")
set(LDFLAGS "$ENV{LDFLAGS}" CACHE STRING "Additional linker flags")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CFLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LDFLAGS}")

if(ARCH STREQUAL "i686")
    add_compile_options(-m32)
    add_link_options(-m32)
elseif(ARCH STREQUAL "x86_64")
    add_compile_options(-m64)
    add_link_options(-m64)
elseif(ARCH STREQUAL "ppc")
    set(CMAKE_SYSTEM_NAME Linux)
    set(CMAKE_C_COMPILER powerpc-linux-gnu-gcc)
    set(CMAKE_CXX_COMPILER powerpc-linux-gnu-g++)
elseif(ARCH STREQUAL "ppc64")
    set(CMAKE_SYSTEM_NAME Linux)
    set(CMAKE_C_COMPILER powerpc64-linux-gnu-gcc)
    set(CMAKE_CXX_COMPILER powerpc64-linux-gnu-g++)
endif()

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
    target_compile_options(lites_server PRIVATE -std=c2x ${CFLAGS})
    target_link_options(lites_server PRIVATE ${LDFLAGS})

    add_executable(lites_emulator ${EMULATOR_SRC})
    target_include_directories(lites_emulator PRIVATE
        "${LITES_SRC_DIR}/include"
        "${LITES_SRC_DIR}/emulator")
    target_compile_options(lites_emulator PRIVATE -std=c2x ${CFLAGS})
    target_link_options(lites_emulator PRIVATE ${LDFLAGS})
endif()

