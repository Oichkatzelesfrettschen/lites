add_subdirectory(unit)

set(_src_dir "${LITES_SRC_DIR}")
if(NOT EXISTS "${_src_dir}/libs/libos/vm.c")
    set(_src_dir "${CMAKE_CURRENT_SOURCE_DIR}/..")
endif()

add_executable(test_cap
    cap/test_cap.c
    ${_src_dir}/server/kern/cap.c
    ../kern/auth.c
    ../kern/audit.c)
target_include_directories(test_cap PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_compile_options(test_cap PRIVATE -std=gnu2x -Wall -Wextra -Werror)

add_executable(test_audit
    audit/test_audit.c
    ../kern/auth.c
    ../kern/audit.c)
target_include_directories(test_audit PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_compile_options(test_audit PRIVATE -std=gnu2x -Wall -Wextra -Werror)

add_executable(test_iommu
    iommu/test_iommu.c
    ${_src_dir}/iommu/iommu.c)
target_compile_options(test_iommu PRIVATE -std=gnu2x -Wall -Wextra -Werror)

add_executable(test_vm_fault
    vm_fault/test_vm_fault.c
    ${_src_dir}/libos/vm.c
    ../posix.c)
target_compile_options(test_vm_fault PRIVATE -std=gnu2x -Wall -Wextra -Werror)
target_include_directories(test_vm_fault PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)


add_executable(test_pipe
    pipe/test_pipe.c
    ../posix.c)
target_include_directories(test_pipe PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_compile_options(test_pipe PRIVATE -std=gnu2x -Wall -Wextra -Werror)

add_executable(test_spawn_wait
    spawn_wait/test_spawn_wait.c)
target_link_libraries(test_spawn_wait PRIVATE posix_helpers)
target_compile_options(test_spawn_wait PRIVATE -std=gnu2x -Wall -Wextra -Werror)

# vm_fault depends on libos which requires full Mach headers.
# Skip building it when those headers are unavailable.
