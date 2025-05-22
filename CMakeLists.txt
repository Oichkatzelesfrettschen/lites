cmake_minimum_required(VERSION 3.5)
project(lites LANGUAGES C)
set(CMAKE_C_STANDARD 23)

# Allow selecting the build architecture with -DARCH=i686 or ARCH=x86_64
set(ARCH "$ENV{ARCH}" CACHE STRING "Target architecture (x86_64 or i686)")
set(CFLAGS "$ENV{CFLAGS}" CACHE STRING "Additional compiler flags")
set(LDFLAGS "$ENV{LDFLAGS}" CACHE STRING "Additional linker flags")
set(LITES_MACH_DIR "$ENV{LITES_MACH_DIR}" CACHE PATH "Mach kernel source directory")
if(NOT LITES_MACH_DIR AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/openmach")
    set(LITES_MACH_DIR "${CMAKE_CURRENT_SOURCE_DIR}/openmach")
endif()
set(MACH_DIR "${LITES_MACH_DIR}")

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
    # Collect architecture specific and common server sources
    file(GLOB SERVER_SUBDIRS RELATIVE "${LITES_SRC_DIR}/server" "${LITES_SRC_DIR}/server/*")
    set(SERVER_DIRS "${LITES_SRC_DIR}/server/${ARCH}")
    foreach(dir ${SERVER_SUBDIRS})
        if(IS_DIRECTORY "${LITES_SRC_DIR}/server/${dir}")
            if(NOT dir STREQUAL "i386" AND NOT dir STREQUAL "x86_64" AND
               NOT dir STREQUAL "mips" AND NOT dir STREQUAL "ns532" AND
               NOT dir STREQUAL "parisc")
                list(APPEND SERVER_DIRS "${LITES_SRC_DIR}/server/${dir}")
            endif()
        endif()
    endforeach()
    set(SERVER_SRC)
    foreach(dir ${SERVER_DIRS})
        file(GLOB_RECURSE _dir_src "${dir}/*.c" "${dir}/*.s")
        list(APPEND SERVER_SRC ${_dir_src})
    endforeach()

    if(EXISTS "${LITES_SRC_DIR}/emulator")
        file(GLOB EMULATOR_SUBDIRS RELATIVE "${LITES_SRC_DIR}/emulator" "${LITES_SRC_DIR}/emulator/*")
        set(EMULATOR_DIRS "${LITES_SRC_DIR}/emulator/${ARCH}")
        foreach(dir ${EMULATOR_SUBDIRS})
            if(IS_DIRECTORY "${LITES_SRC_DIR}/emulator/${dir}")
                if(NOT dir STREQUAL "i386" AND NOT dir STREQUAL "x86_64" AND
                   NOT dir STREQUAL "mips" AND NOT dir STREQUAL "ns532" AND
                   NOT dir STREQUAL "parisc")
                    list(APPEND EMULATOR_DIRS "${LITES_SRC_DIR}/emulator/${dir}")
                endif()
            endif()
        endforeach()
        set(EMULATOR_SRC)
        foreach(dir ${EMULATOR_DIRS})
            file(GLOB_RECURSE _dir_src "${dir}/*.c" "${dir}/*.s")
            list(APPEND EMULATOR_SRC ${_dir_src})
        endforeach()
    endif()

    add_executable(lites_server ${SERVER_SRC})
    target_include_directories(lites_server PRIVATE
        "${LITES_SRC_DIR}/include"
        "${LITES_SRC_DIR}/server"
        $<$<BOOL:${MACH_DIR}>:${MACH_DIR}/include>)
    if(ARCH STREQUAL "x86_64")
        target_include_directories(lites_server PRIVATE
            "${LITES_SRC_DIR}/include/x86_64")
    elseif(ARCH STREQUAL "i686")
        target_include_directories(lites_server PRIVATE
            "${LITES_SRC_DIR}/include/i386")
    endif()
    target_compile_options(lites_server PRIVATE ${C23_FLAG} ${CFLAGS})
    target_link_options(lites_server PRIVATE ${LDFLAGS})

    if(EMULATOR_SRC)
        add_executable(lites_emulator ${EMULATOR_SRC})
        target_include_directories(lites_emulator PRIVATE
            "${LITES_SRC_DIR}/include"
            "${LITES_SRC_DIR}/emulator"
            $<$<BOOL:${MACH_DIR}>:${MACH_DIR}/include>)
        if(ARCH STREQUAL "x86_64")
            target_include_directories(lites_emulator PRIVATE
                "${LITES_SRC_DIR}/include/x86_64")
        elseif(ARCH STREQUAL "i686")
            target_include_directories(lites_emulator PRIVATE
                "${LITES_SRC_DIR}/include/i386")
        endif()
        target_compile_options(lites_emulator PRIVATE ${C23_FLAG} ${CFLAGS})
        target_link_options(lites_emulator PRIVATE ${LDFLAGS})
    endif()
endif()

