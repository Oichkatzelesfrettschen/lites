add_library(ddekit STATIC
    memory.c
)

target_include_directories(ddekit PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/.."
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/badgerferret
    ${CMAKE_SOURCE_DIR}/include
)

