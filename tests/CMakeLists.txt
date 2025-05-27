set(_src_dir "${LITES_SRC_DIR}")
if(NOT EXISTS "${_src_dir}/server/kern/cap.c")
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

add_library(libos STATIC
    ${_src_dir}/libos/vm.c)
target_include_directories(libos PUBLIC ${_src_dir}/include)

add_executable(test_vm_fault
    vm_fault/test_vm_fault.c
    ../posix.c)
target_link_libraries(test_vm_fault PRIVATE libos)
target_compile_options(test_vm_fault PRIVATE -std=gnu2x -Wall -Wextra -Werror)
target_include_directories(test_vm_fault PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)


add_executable(test_pipe
    pipe/test_pipe.c
    ../posix.c)
target_include_directories(test_pipe PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_compile_options(test_pipe PRIVATE -std=gnu2x -Wall -Wextra -Werror)

add_executable(test_spawn_wait
    spawn_wait/test_spawn_wait.c
    ../posix.c)
target_include_directories(test_spawn_wait PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_compile_options(test_spawn_wait PRIVATE -std=gnu2x -Wall -Wextra -Werror)



# The posix_wrap tests depend on full Mach headers which are
# not available in this environment.  Disable for now.
#add_executable(test_posix
#    posix/test_posix.c
#    ${_src_dir}/liblites/posix_wrap.c)
#target_include_directories(test_posix PRIVATE
#    ${_src_dir}/include)
#target_compile_options(test_posix PRIVATE -std=gnu23 -Wall -Wextra -Werror)

#add_executable(test_fifo
#    fifo_test/test_fifo.c
#    ${_src_dir}/libposix/posix.c)
#target_include_directories(test_fifo PRIVATE ${_src_dir}/include)
#target_compile_options(test_fifo PRIVATE -std=gnu23 -Wall -Wextra -Werror)

add_executable(test_xec596_init
    xec596/test_xec596_init.c)
target_compile_options(test_xec596_init PRIVATE -std=gnu23 -Wall -Wextra -Werror)

