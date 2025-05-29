add_library(lites_os STATIC
    vm.c
)

target_include_directories(lites_os PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)

