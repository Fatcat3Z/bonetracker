cmake_minimum_required(VERSION 3.12)
project(Multithreadingtest)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenCV REQUIRED)
find_package(Threads)

add_executable(Multithreadingtest main.cpp bonetaker.cpp bonetaker.h Threadinfo.cpp Threadinfo.h BoneInfo.cpp BoneInfo.h Serial.cpp Serial.hpp info.cpp info.h)

target_link_libraries(Multithreadingtest ${OpenCV_LIBS})
target_link_libraries(Multithreadingtest ${CMAKE_THREAD_LIBS_INIT})