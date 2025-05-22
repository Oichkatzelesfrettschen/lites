cmake_minimum_required(VERSION 3.5)
project(lites LANGUAGES C)
set(CMAKE_C_STANDARD 23)

# Allow selecting the build architecture with -DARCH=i686 or ARCH=x86_64
set(ARCH "$ENV{ARCH}" CACHE STRING "Target architecture (x86_64 or i686)")
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
endif()

include(CheckCCompilerFlag)
check_c_compiler_flag(-std=c23 HAS_STD_C23)
if(HAS_STD_C23)
    set(C23_FLAG "-std=c23")
else()
    set(C23_FLAG "-std=c2x")
endif()

# Optionally allow building out-of-tree archives.
# Prefer the modernised tree if present.  Otherwise fall back to an
# extracted historical archive under build/.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src-lites-1.1-2025")
    set(_default_src "${CMAKE_CURRENT_SOURCE_DIR}/src-lites-1.1-2025")
else()
    set(_default_src "${CMAKE_CURRENT_SOURCE_DIR}/build/lites-1.1.u3")
endif()
set(LITES_SRC_DIR "${_default_src}" CACHE PATH "Lites source directory")

# Ensure an include/machine symlink exists before collecting sources.
if(EXISTS "${LITES_SRC_DIR}/include")
    if(NOT EXISTS "${LITES_SRC_DIR}/include/machine")
        set(_machine_arch "${ARCH}")
        if(_machine_arch STREQUAL "i686")
            set(_machine_arch "i386")
        endif()
        if(EXISTS "${LITES_SRC_DIR}/include/${_machine_arch}")
            execute_process(
                COMMAND "${CMAKE_COMMAND}" -E create_symlink "${_machine_arch}" "machine"
                WORKING_DIRECTORY "${LITES_SRC_DIR}/include")
        endif()
    endif()
endif()

# Collect sources for server and emulator if the directory exists
if(EXISTS "${LITES_SRC_DIR}")
    file(GLOB_RECURSE SERVER_SRC
        "${LITES_SRC_DIR}/server/*.c" "${LITES_SRC_DIR}/server/*.s")
    if(EXISTS "${LITES_SRC_DIR}/emulator")
        file(GLOB_RECURSE EMULATOR_SRC
            "${LITES_SRC_DIR}/emulator/*.c" "${LITES_SRC_DIR}/emulator/*.s")
    endif()

    add_executable(lites_server ${SERVER_SRC})
    target_include_directories(lites_server PRIVATE
        "${LITES_SRC_DIR}/include"
        "${LITES_SRC_DIR}/server")
    if(ARCH STREQUAL "x86_64")
        target_include_directories(lites_server PRIVATE
            "${LITES_SRC_DIR}/include/x86_64")
    endif()
    target_compile_options(lites_server PRIVATE ${C23_FLAG} ${CFLAGS})
    target_link_options(lites_server PRIVATE ${LDFLAGS})

    if(EMULATOR_SRC)
        add_executable(lites_emulator ${EMULATOR_SRC})
        target_include_directories(lites_emulator PRIVATE
            "${LITES_SRC_DIR}/include"
            "${LITES_SRC_DIR}/emulator")
        if(ARCH STREQUAL "x86_64")
            target_include_directories(lites_emulator PRIVATE
                "${LITES_SRC_DIR}/include/x86_64")
        endif()
        target_compile_options(lites_emulator PRIVATE ${C23_FLAG} ${CFLAGS})
        target_link_options(lites_emulator PRIVATE ${LDFLAGS})
    endif()
endif()

