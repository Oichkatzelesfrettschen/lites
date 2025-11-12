cmake_minimum_required(VERSION 3.5)
project(lites LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENABLE_CLANG_TIDY "Run clang-tidy during the build" OFF)
if(ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE NAMES clang-tidy)
    if(CLANG_TIDY_EXE)
        set(CMAKE_C_CLANG_TIDY ${CLANG_TIDY_EXE})
        set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_EXE})
        add_custom_target(clang-tidy
            COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/run-clang-tidy.sh
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Run clang-tidy analysis")
    endif()
endif()
find_package(BISON)

# Allow selecting the build architecture with -DARCH=i686 or ARCH=x86_64
set(ARCH "$ENV{ARCH}" CACHE STRING "Target architecture (x86_64 or i686)")
if(NOT ARCH)
    set(ARCH "x86_64")
endif()
set(CFLAGS "$ENV{CFLAGS}" CACHE STRING "Additional compiler flags")
# Harden binaries by disallowing executable stacks
# Append the noexecstack flag so that resulting binaries are not
# executable from the stack.
set(
    LDFLAGS
    "$ENV{LDFLAGS} -Wl,-z,noexecstack"
    CACHE STRING "Additional linker flags"
)
set(SANITIZE "$ENV{SANITIZE}" CACHE STRING "Enable address sanitizer (1/0)")
set(MULTICORE_SCHED "$ENV{MULTICORE_SCHED}" CACHE STRING "Enable multicore scheduler (1/0)")
set(LITES_MACH_LIB_DIR "$ENV{LITES_MACH_LIB_DIR}" CACHE PATH "Directory containing Mach static libraries")
set(LITES_MACH_DIR "$ENV{LITES_MACH_DIR}" CACHE PATH "Mach kernel source directory")
if(NOT LITES_MACH_DIR AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/openmach")
    set(LITES_MACH_DIR "${CMAKE_CURRENT_SOURCE_DIR}/openmach")
elseif(NOT LITES_MACH_DIR AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/core/mach_kernel")
    set(LITES_MACH_DIR "${CMAKE_CURRENT_SOURCE_DIR}/core/mach_kernel")
endif()
set(MACH_DIR "${LITES_MACH_DIR}")
if(NOT LITES_MACH_LIB_DIR AND MACH_DIR AND EXISTS "${MACH_DIR}/lib")
    set(LITES_MACH_LIB_DIR "${MACH_DIR}/lib")
endif()
set(MACH_LIB_DIR "${LITES_MACH_LIB_DIR}")

set(MACH_INCLUDE_DIR "")
if(MACH_DIR AND EXISTS "${MACH_DIR}/include")
    set(MACH_INCLUDE_DIR "${MACH_DIR}/include")
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/localmach/include")
    set(MACH_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/localmach/include")
elseif(EXISTS "/opt/mach/osfmk/kernel/src")
    set(MACH_INCLUDE_DIR "/opt/mach/osfmk/kernel/src")
else()
    message(WARNING "Mach headers not found. Set LITES_MACH_DIR, add openmach submodule, or run scripts/setup-build-environment.sh. Build may not complete without proper headers.")
    # Create minimal stub to allow configuration
    set(MACH_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/localmach/include")
endif()

if(MACH_LIB_DIR AND EXISTS "${MACH_LIB_DIR}")
    file(GLOB MACH_LIBS "${MACH_LIB_DIR}/*.a")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CFLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LDFLAGS}")
if(SANITIZE STREQUAL "1")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
endif()

if(MULTICORE_SCHED STREQUAL "1")
    add_compile_definitions(CONFIG_SCHED_MULTICORE=1)
else()
    add_compile_definitions(CONFIG_SCHED_MULTICORE=0)
endif()

if(ARCH STREQUAL "i686")
    add_compile_options(-m32)
    add_link_options(-m32)
elseif(ARCH STREQUAL "x86_64")
    add_compile_options(-m64)
    add_link_options(-m64)
elseif(ARCH STREQUAL "arm")
    add_compile_options(-marm)
elseif(ARCH STREQUAL "riscv64")
    add_compile_options(-march=rv64imac -mabi=lp64)
elseif(ARCH STREQUAL "powerpc")
    add_compile_options(-m32)
    add_link_options(-m32)
elseif(ARCH STREQUAL "ia16")
    add_compile_options(-m16)
    add_link_options(-m16)
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
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/build/lites-1.1.u3")
    set(_default_src "${CMAKE_CURRENT_SOURCE_DIR}/build/lites-1.1.u3")
else()
    set(_default_src "${CMAKE_CURRENT_SOURCE_DIR}")
endif()
set(LITES_SRC_DIR "${_default_src}" CACHE PATH "Lites source directory")
get_filename_component(LITES_SRC_DIR "${LITES_SRC_DIR}" REALPATH)

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
        file(GLOB_RECURSE _dir_src "${dir}/*.c" "${dir}/*.S")
        list(APPEND SERVER_SRC ${_dir_src})
    endforeach()
    file(GLOB _kern_src "${CMAKE_CURRENT_SOURCE_DIR}/kern/*.c")
    file(GLOB _posix_src "${CMAKE_CURRENT_SOURCE_DIR}/posix/*.c")
    file(GLOB _libos_src "${CMAKE_CURRENT_SOURCE_DIR}/libos/*.c")
    list(APPEND SERVER_SRC ${_kern_src} ${_posix_src})
    # Build the primary IOMMU implementation only. The alternative
    # version provides identical symbols and would lead to duplicate
    # definitions when linked together.
    set(_iommu_src "${CMAKE_CURRENT_SOURCE_DIR}/drivers/iommu/iommu.c")
    list(APPEND SERVER_SRC ${_iommu_src} ${_libos_src})

    if(BISON_FOUND)
        file(GLOB_RECURSE _y_src
            "${CMAKE_CURRENT_SOURCE_DIR}/*.y"
            "${LITES_SRC_DIR}/server/*.y")
        foreach(yfile ${_y_src})
            get_filename_component(_y_name "${yfile}" NAME_WE)
            set(_y_out "${CMAKE_CURRENT_BINARY_DIR}/${_y_name}.c")
            BISON_TARGET(${_y_name} ${yfile} ${_y_out} COMPILE_FLAGS -d)
            list(APPEND SERVER_SRC ${BISON_${_y_name}_OUTPUTS})
        endforeach()
    endif()

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
            file(GLOB_RECURSE _dir_src "${dir}/*.c" "${dir}/*.S")
            list(APPEND EMULATOR_SRC ${_dir_src})
        endforeach()
        list(APPEND EMULATOR_SRC ${_kern_src} ${_iommu_src} ${_posix_src}
                                ${_libos_src})
        if(BISON_FOUND)
            foreach(yfile ${_y_src})
                get_filename_component(_y_name "${yfile}" NAME_WE)
                list(APPEND EMULATOR_SRC ${BISON_${_y_name}_OUTPUTS})
            endforeach()
        endif()
    endif()

    # The historical server sources are not included in this snapshot, so the
    # server binary is disabled for now. Only the libraries and tests are built.

    if(EMULATOR_SRC)
        add_executable(lites_emulator ${EMULATOR_SRC})
        target_include_directories(lites_emulator PRIVATE
            "${LITES_SRC_DIR}/include"
            "${LITES_SRC_DIR}/emulator"
            "${LITES_SRC_DIR}/libos"
            "${CMAKE_CURRENT_SOURCE_DIR}/libs/ddekit"
            "${MACH_INCLUDE_DIR}"
            "${CMAKE_CURRENT_SOURCE_DIR}/kern"
            "${CMAKE_CURRENT_SOURCE_DIR}/posix"
            "${CMAKE_CURRENT_SOURCE_DIR}/drivers/iommu"
            "${CMAKE_CURRENT_SOURCE_DIR}/include")

        if(ARCH STREQUAL "x86_64")
            target_include_directories(lites_emulator PRIVATE
                "${LITES_SRC_DIR}/include/x86_64")
        elseif(ARCH STREQUAL "i686")
            target_include_directories(lites_emulator PRIVATE
                "${LITES_SRC_DIR}/include/i386")
        elseif(ARCH STREQUAL "arm")
            target_include_directories(lites_emulator PRIVATE
                "${LITES_SRC_DIR}/include/arm")
        elseif(ARCH STREQUAL "riscv64")
            target_include_directories(lites_emulator PRIVATE
                "${LITES_SRC_DIR}/include/riscv64")
        elseif(ARCH STREQUAL "powerpc")
            target_include_directories(lites_emulator PRIVATE
                "${LITES_SRC_DIR}/include/powerpc")
        elseif(ARCH STREQUAL "ia16")
            target_include_directories(lites_emulator PRIVATE
                "${LITES_SRC_DIR}/include/ia16")
        endif()
        target_compile_options(lites_emulator PRIVATE ${C23_FLAG} ${CFLAGS})
        target_link_options(lites_emulator PRIVATE ${LDFLAGS})
        if(MACH_LIBS)
            target_link_libraries(lites_emulator PRIVATE ${MACH_LIBS})
        endif()
    endif()
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/libs/libposix")
    add_subdirectory(libs/libposix)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/libs/ddekit")
    add_subdirectory(libs/ddekit)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/libs/libcrypto")
    add_subdirectory(libs/libcrypto)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/libs/liblites" AND
   MACH_INCLUDE_DIR AND EXISTS "${MACH_INCLUDE_DIR}/mach.h")
    add_subdirectory(libs/liblites)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/arch")
    add_subdirectory(arch)
endif()

# libos depends on full Mach headers which are
# incomplete in this repository. Skip building it
# unless explicitly enabled in the future.

# Unit tests require missing Mach headers. They are disabled
# in this minimal configuration.

option(LITES_BUILD_TESTS "Build the Lites unit tests" OFF)
if(LITES_BUILD_TESTS)
    add_subdirectory(tests)
endif()
