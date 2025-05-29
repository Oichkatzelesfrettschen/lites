set(LIBLITES_SRC
    exec_file.c
    enclave.c
    posix_wrap.c
)

if(ARCH STREQUAL "i686")
    list(APPEND LIBLITES_SRC
        i386/in_cksum.c
        i386/ntoh.S
        memcmp.c
    )
elseif(ARCH STREQUAL "x86_64")
    list(APPEND LIBLITES_SRC
        x86_64/in_cksum.c
        x86_64/ntoh.S
        memcmp.c
    )
elseif(ARCH STREQUAL "ns532")
    list(APPEND LIBLITES_SRC
        ns532/in_cksum.c
        ns532/misc_asm.S
        memcmp.c
        bcmp.c
    )
elseif(ARCH STREQUAL "mips")
    list(APPEND LIBLITES_SRC
        mips/ntoh.c
        memcmp.c
        bcmp.c
    )
elseif(ARCH STREQUAL "alpha")
    list(APPEND LIBLITES_SRC
        alpha/ntoh.c
        bcmp.c
        memcmp.c
    )
else()
    list(APPEND LIBLITES_SRC memcmp.c)
endif()

add_library(liblites STATIC ${LIBLITES_SRC})

target_include_directories(liblites PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)

