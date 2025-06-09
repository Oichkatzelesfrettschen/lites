add_library(arch_x86_64_v1 STATIC startup.c)
target_include_directories(arch_x86_64_v1 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
