add_library(lites_crypto STATIC
    aes_fallback.c
    keystore.c
)

target_include_directories(lites_crypto PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)

