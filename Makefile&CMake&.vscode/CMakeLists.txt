cmake_minimum_required(VERSION 3.10)

project(course VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 11)  # 将 C++ 标准设置为 C++ 11
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # C++ 11 是强制要求，不会衰退至低版本
set(CMAKE_CXX_EXTENSIONS OFF)  # 禁止使用编译器特有扩展

if(NOT CMAKE_BUILD_TYPE)
	message(WARNING "NOT SET CMAKE_BUILD_TYPE")
    set(CMAKE_BUILD_TYPE "Release")
endif()

message(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message(STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "CMAKE_CURRENT_LIST_FILE: ${CMAKE_CURRENT_LIST_FILE}")
message(STATUS "CMAKE_CURRENT_LIST_DIR: ${CMAKE_CURRENT_LIST_DIR}")
message(STATUS "CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")
message(STATUS "CMAKE_INCLUDE_PATH: ${CMAKE_INCLUDE_PATH}")
message(STATUS "CMAKE_LIBRARY_PATH: ${CMAKE_LIBRARY_PATH}")
message(STATUS "CMAKE_SYSTEM: ${CMAKE_SYSTEM}")
message(STATUS "CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMAKE_SYSTEM_VERSION: ${CMAKE_SYSTEM_VERSION}")
message(STATUS "CMAKE_C_COMPILER: ${CMAKE_C_COMPILER}")
message(STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")

# 通过aux_source_directory命令获取文件夹中的所有源文件名称
aux_source_directory(src SRC)
#message(STATUS ${SRC})
set(INCLUDE "include")

# 生成可执行文件目标run
#set(SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
add_executable(run ${SRC})

add_executable(run main.cpp)   #生成可执行文件目标run
add_library(math SHARED add.cpp sub.cpp)   #生成共享库目标math
add_library(math STATIC add.cpp sub.cpp)   #生成静态库目标math

# 使用install安装文件
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/install/)
install(TARGETS 
            run my_math
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION lib)

 install(FILES 
                include/my_math.hpp DESTINATION my_math/include)
# 可生成如下文件
├── install
│   ├── bin
│   │   └── run
│   ├── lib
│   │   └── libmy_math.a
│   └── my_math
│       └── include
│           └── my_math.hpp



# 引用第三方库
# 通常三方库会给我们提供一些变量
<LibaryName>_FOUND # 告诉我们是否找到
<LibaryName>_INCLUDE_DIR 
<LibaryName>_INCLUDES` `
<LibaryName>_LIBRARY
<LibaryName>_LIBRARIES

例如
find_package(Protobuf)
if(Protobuf_FOUND)
	message(STATUS "Protobuf found")
    message(STATUS ${Protobuf_FOUND})
    message(STATUS ${Protobuf_INCLUDE_DIR})
    message(STATUS ${Protobuf_INCLUDES})
    message(STATUS ${Protobuf_LIBRARY})
    message(STATUS ${Protobuf_LIBRARIES})
else()
	message(WARNING "Protobuf not  found")
endif(）