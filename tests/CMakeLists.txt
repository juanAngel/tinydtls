cmake_minimum_required(VERSION 2.8)

project(tests C)


add_executable(test_dtls_client dtls-client.c)
add_executable(test_dtls_server dtls-server.c)

target_link_libraries(test_dtls_client tinydtls)
target_link_libraries(test_dtls_server tinydtls)

