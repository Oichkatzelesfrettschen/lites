set(_posix_src posix.c spawn.c ipc.c)
if(EXISTS "${MACH_INCLUDE_DIR}/mach/cthreads.h" OR EXISTS "${CMAKE_SOURCE_DIR}/include/mach/cthreads.h")
    list(APPEND _posix_src pthread.c)
endif()
add_library(posix_helpers STATIC ${_posix_src})

target_include_directories(posix_helpers PUBLIC ${CMAKE_SOURCE_DIR}/include)

