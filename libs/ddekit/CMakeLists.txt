add_library(ddekit INTERFACE)

target_include_directories(ddekit INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/badgerferret
    ${CMAKE_SOURCE_DIR}/include
)

