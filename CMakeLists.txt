# cmake version
cmake_minimum_required(VERSION 3.5)

# project name
project(scatk)

set(LANGUAGE C)

# version
set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)

# detect python3
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
find_package(Python3 COMPONENTS NumPy)

# add python include path
include_directories(${Python3_INCLUDE_DIRS})
include_directories(${Python3_NumPy_INCLUDE_DIRS})

# set include path
include_directories(${PROJECT_SOURCE_DIR}/include)

# set source file search path
aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC_LIST)

# search all header files
file(GLOB_RECURSE HEADER_LIST ${PROJECT_SOURCE_DIR}/include/*.h)

# append header files to source file list
list(APPEND SRC_LIST ${HEADER_LIST})

# set output path
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# set target
add_executable(${PROJECT_NAME} ${SRC_LIST})
# link library libargparse.a
target_link_libraries(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/lib/libargparse.a)


