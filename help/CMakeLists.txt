project(utils C)


file(GLOB src
    "*.h"
    "*.c"
)

add_library(${PROJECT_NAME} ${src})


target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
