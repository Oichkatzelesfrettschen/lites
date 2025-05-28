add_library(posix_helpers STATIC
    posix.c
    pthread.c
    spawn.c
    ipc.c
)

target_include_directories(posix_helpers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

