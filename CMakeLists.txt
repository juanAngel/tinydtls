cmake_minimum_required(VERSION 2.8)

project(tinydtls C)

set(PLATFORM "linux" CACHE STRING "plataforma para la que se compilara")
set_property(CACHE PLATFORM PROPERTY STRINGS linux rexlab rexlab-odin ipex12 ipex16)

option(DTLS_PSK "Activa el soporte de cifrado PSK" OFF)
option(DTLS_ECC "Activa el soporte de cifrado ECC" OFF)
option(DTLS_MA "Activa el soporte de cifrado MA" OFF)

option(WITH_SHA256 "Activa el soporte de sha256" ON)
option(WITH_SHA384 "Activa el soporte de sha284" OFF)
option(WITH_SHA512 "Activa el soporte de sha512" OFF)

file(GLOB src
    "*.h"
    "*.c"
)
file(GLOB aes_src
    "aes/*.h"
    "aes/*.c"
)
file(GLOB ecc_src
    "ecc/ecc.h"
    "ecc/ecc.c"
)
file(GLOB sha2_src
    "sha2/sha2.h"
    "sha2/sha2.c"
)
file(GLOB ma_src
    "ma/ma.h"
    "ma/ma.c"
    "ma/mini-gmp.c"
    "ma/mini-gmp.h"
)

add_library(${PROJECT_NAME} ${src} ${aes_src} ${ecc_src} ${ma_src} ${sha2_src})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(${PROJECT_NAME} PRIVATE PACKAGE_NAME="${PROJECT_NAME}")
target_compile_definitions(${PROJECT_NAME} PRIVATE PACKAGE_VERSION="${PROJECT_VERSION}")

if(PLATFORM STREQUAL "linux")
    target_compile_definitions(${PROJECT_NAME} PUBLIC HAVE_TIME_H)
    target_compile_definitions(${PROJECT_NAME} PUBLIC HAVE_VPRINTF)
    target_compile_definitions(${PROJECT_NAME} PUBLIC HAVE_ASSERT_H)
    target_compile_definitions(${PROJECT_NAME} PUBLIC HAVE_SYS_TIME_H)
    target_compile_definitions(${PROJECT_NAME} PUBLIC HAVE_ARPA_INET_H)
    target_compile_definitions(${PROJECT_NAME} PUBLIC SHA2_USE_INTTYPES_H)
endif()

if(DTLS_PSK)
    target_compile_definitions(${PROJECT_NAME} PUBLIC DTLS_PSK)
endif()
if(DTLS_ECC)
    target_compile_definitions(${PROJECT_NAME} PUBLIC DTLS_ECC)
endif()
if(DTLS_MA)
    target_compile_definitions(${PROJECT_NAME} PUBLIC DTLS_MA)
endif()
if(WITH_SHA256)
    target_compile_definitions(${PROJECT_NAME} PUBLIC WITH_SHA256=1)
endif()
if(WITH_SHA384)
    target_compile_definitions(${PROJECT_NAME} PUBLIC WITH_SHA384=1)
endif()
if(WITH_SHA512)
    target_compile_definitions(${PROJECT_NAME} PUBLIC WITH_SHA512=1)
endif()

add_subdirectory(tests)
