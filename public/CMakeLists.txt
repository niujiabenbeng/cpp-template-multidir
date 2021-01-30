################################################################################
### * cmake命令不分大小写, 变量区分大小写.
### * 这里所有命令用小写, 所有变量用大写
### * cmake自动处理头文件依赖问题, 不需要额外的工作.
################################################################################

cmake_minimum_required(VERSION 3.0.2)

# 这里修改project的名字
project(hello)

# 生成compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 这里选择用clang还是gcc
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_FLAGS "-g -Wall -fpic -O3 -std=c++14")

set(INCDIR   ${PROJECT_SOURCE_DIR}/include)
set(SRCDIR   ${PROJECT_SOURCE_DIR}/src)
set(TOOLSDIR ${PROJECT_SOURCE_DIR}/tools)

# 这里添加第三方库
set(INCLUDE ./3rdparty/boost/include)
set(LIBRARY ./3rdparty/boost/lib)
set(LIBS dl m z rt glog gtest gflags jsoncpp pthread
  boost_system boost_filesystem)
include_directories(SYSTEM ${INCLUDE})
include_directories(${INCDIR})
link_directories(${LIBRARY})
link_libraries(${LIBS})

# 这里添加不想被编译的文件, 文件路径为绝对路径.
# 这里两个cpp文件并不存在, 只是用来做例子.
set(EXCLUDE ${SRCDIR}/exclude.cpp ${TOOLSDIR}/exclude.cpp)

# cmake递归查找命令(GLOB_RECURSE)有bug, 这里采用非递归方式.
# 这里后面的两个subdir并不存在, 只是用来做例子.
file(GLOB SRC_SRC
  ${SRCDIR}/*.cpp
  ${SRCDIR}/subdir/*.cpp)
file(GLOB SRC_TOOLS
  ${TOOLSDIR}/*.cpp
  ${TOOLSDIR}/subdir/*.cpp)
list(REMOVE_ITEM SRC_SRC   ${EXCLUDE})
list(REMOVE_ITEM SRC_TOOLS ${EXCLUDE})

# 生成动态库, 位于: lib/lib${PROJECT_NAME}.so
add_library(${PROJECT_NAME} SHARED ${SRC_SRC})
set_target_properties(${PROJECT_NAME}
  PROPERTIES LIBRARY_OUTPUT_DIRECTORY "lib")

# 生成可执行文件
foreach(TOOL ${SRC_TOOLS})
  file(RELATIVE_PATH RELPATH ${PROJECT_SOURCE_DIR} ${TOOL})
  get_filename_component(BASENAME ${RELPATH} NAME)
  get_filename_component(DIRNAME  ${RELPATH} DIRECTORY)
  string(REGEX REPLACE "cpp$" "bin" BINNAME ${BASENAME})
  add_executable(${BINNAME} ${SRC_SRC} ${TOOL})
  set_target_properties(${BINNAME}
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${DIRNAME})
endforeach(TOOL)
