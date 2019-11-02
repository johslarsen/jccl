cmake_minimum_required (VERSION 3.2.0)

project(jccl VERSION 0.1.0 LANGUAGES CXX C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)

add_definitions(-Wall -fno-omit-frame-pointer
  -Wextra -Wshadow
  -Wnull-dereference -Wformat=2 -Wwrite-strings
  -Wswitch-default -Wfloat-equal -Winline -fstack-protector
  -UNDEBUG
)

enable_testing()
add_subdirectory(test)
