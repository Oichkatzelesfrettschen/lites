add_executable(test_cap
    cap/test_cap.c
    ../src-lites-1.1-2025/server/kern/cap.c
    ../kern/auth.c
    ../kern/audit.c)
target_include_directories(test_cap PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_compile_options(test_cap PRIVATE -std=c23 -Wall -Werror)

add_executable(test_audit
    audit/test_audit.c
    ../kern/auth.c
    ../kern/audit.c)
target_include_directories(test_audit PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_compile_options(test_audit PRIVATE -std=c23 -Wall -Werror)

add_executable(test_iommu
    iommu/test_iommu.c
    ../src-lites-1.1-2025/iommu/iommu.c)
target_compile_options(test_iommu PRIVATE -std=c23 -Wall -Werror)

add_executable(test_vm_fault
    vm_fault/test_vm_fault.c
    ../src-lites-1.1-2025/server/vm/vm_handlers.c)
target_compile_options(test_vm_fault PRIVATE -std=c23 -Wall -Werror)

