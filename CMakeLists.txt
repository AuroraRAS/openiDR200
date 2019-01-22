cmake_minimum_required(VERSION 2.8)
set (CMAKE_CXX_STANDARD 11)

project(openiDR200)

add_executable(${PROJECT_NAME} "main.cpp" "idrusb.cpp")
target_link_libraries(${PROJECT_NAME} usb-1.0)
