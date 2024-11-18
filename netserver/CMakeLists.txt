cmake_minimum_required(VERSION 3.0)
project(netserver)

# 配置编译选项
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

# 配置最终的可执行文件输出的路径    bin存的是最后的可执行文件
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#见过源码文件保存在SRC_LIST变量里
aux_source_directory(src SRC_LIST)

# 配置头文件的搜索路径
include_directories(${PROJECT_SOURCE_DIR}/include)


# add_executable(tcp tcp)
# # 加载子目录
add_subdirectory(src)

add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)
