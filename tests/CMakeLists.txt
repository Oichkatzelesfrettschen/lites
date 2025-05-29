add_subdirectory(unit)

set(_src_dir "${LITES_SRC_DIR}")
if(NOT EXISTS "${_src_dir}/server/kern/cap.c")
    set(_src_dir "${CMAKE_CURRENT_SOURCE_DIR}/..")
endif()

add_executable(test_spawn_wait
    spawn_wait/test_spawn_wait.c)
target_link_libraries(test_spawn_wait PRIVATE posix_helpers)
target_compile_options(test_spawn_wait PRIVATE -std=gnu2x -Wall -Wextra -Werror)

add_executable(test_vm_fault
    vm_fault/test_vm_fault.c
    ${_src_dir}/libos/vm.c
)
target_compile_options(test_vm_fault PRIVATE -std=gnu2x -Wall -Wextra -Werror)
target_link_libraries(test_vm_fault PRIVATE posix_helpers)
