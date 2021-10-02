cmake_minimum_required(VERSION 3.0.0)
project(balloon)

include_directories(./include)
aux_source_directory(./src DIR_ROOT_SRC)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")         # 开启警告
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lm")           # 链接数学库
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")    # 使用c++11

set(CMAKE_CXX_FLAGS "-O4 -DNDEBUG " )                   # release优化
set(CMAKE_BUILD_TYPE "release")
# set(CMAKE_CXX_FLAGS  "-O0" )                          # 调试不优化
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")          # 开启调试信息
# set(CMAKE_BUILD_TYPE "debug")

add_executable(${PROJECT_NAME} ${DIR_ROOT_SRC})
