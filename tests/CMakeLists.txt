add_subdirectory(unit)

set(_src_dir "${LITES_SRC_DIR}")
if(NOT EXISTS "${_src_dir}/libs/libos/vm.c")
    set(_src_dir "${CMAKE_CURRENT_SOURCE_DIR}/..")
endif()

add_executable(test_spawn_wait
    spawn_wait/test_spawn_wait.c)
target_link_libraries(test_spawn_wait PRIVATE posix_helpers)
target_compile_options(test_spawn_wait PRIVATE -std=gnu2x -Wall -Wextra -Werror)

# vm_fault depends on libos which requires full Mach headers.
# Skip building it when those headers are unavailable.
