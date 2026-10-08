cmake_minimum_required(VERSION 3.30)
project(untitled9)

set(CMAKE_CXX_STANDARD 20)

# Вказуємо шлях до OpenSSL
set(OPENSSL_ROOT_DIR "/opt/homebrew/opt/openssl@3")
set(OPENSSL_INCLUDE_DIR "/opt/homebrew/opt/openssl@3/include")
set(OPENSSL_LIBRARIES "/opt/homebrew/opt/openssl@3/lib")

find_package(OpenSSL REQUIRED)

include_directories(${OPENSSL_INCLUDE_DIR})
link_directories(${OPENSSL_LIBRARIES})

add_executable(untitled9 main.cpp)
target_link_libraries(untitled9 OpenSSL::SSL OpenSSL::Crypto)

