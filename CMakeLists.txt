cmake_minimum_required(VERSION 3.23)
project(broadcast C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

add_executable(broadcast main.c main.h client.c client.h server.c server.h)
target_link_libraries(broadcast asound)
