add_executable(test_cap
    cap/test_cap.c
    ../src-lites-1.1-2025/server/kern/cap.c
    ../kern/auth.c
    ../kern/audit.c)
target_include_directories(test_cap PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_compile_options(test_cap PRIVATE -std=c23 -Wall -Wextra -Werror)

add_executable(test_audit
    audit/test_audit.c
    ../kern/auth.c
    ../kern/audit.c)
target_include_directories(test_audit PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_compile_options(test_audit PRIVATE -std=c23 -Wall -Wextra -Werror)

add_executable(test_iommu
    iommu/test_iommu.c
    ../src-lites-1.1-2025/iommu/iommu.c)
target_compile_options(test_iommu PRIVATE -std=c23 -Wall -Wextra -Werror)

add_executable(test_vm_fault
    vm_fault/test_vm_fault.c
    ../src-lites-1.1-2025/server/vm/vm_handlers.c
    ../posix.c)
target_compile_options(test_vm_fault PRIVATE -std=c23 -Wall -Wextra -Werror)
target_include_directories(test_vm_fault PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)


add_executable(test_pipe
    pipe/test_pipe.c
    ../posix.c)
target_include_directories(test_pipe PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_compile_options(test_pipe PRIVATE -std=c23 -Wall -Wextra -Werror)

add_executable(test_spawn_wait
    spawn_wait/test_spawn_wait.c
    ../posix.c)
target_include_directories(test_spawn_wait PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_compile_options(test_spawn_wait PRIVATE -std=c23 -Wall -Wextra -Werror)


add_executable(test_posix
    posix/test_posix.c
    ../src-lites-1.1-2025/liblites/posix_wrap.c)
target_include_directories(test_posix PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src-lites-1.1-2025/include)
target_compile_options(test_posix PRIVATE -std=c23 -Wall -Wextra -Werror)

add_executable(test_fifo
    fifo_test/test_fifo.c
    ../src-lites-1.1-2025/libposix/posix.c)
target_include_directories(test_fifo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src-lites-1.1-2025/include)
target_compile_options(test_fifo PRIVATE -std=c23 -Wall -Wextra -Werror)
