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

# detect eigen3
find_package(Eigen3 REQUIRED)

# add include path of packages
include_directories(${Python3_INCLUDE_DIRS})
include_directories(${Python3_NumPy_INCLUDE_DIRS})

# set include path
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/argparse/include)

# set source file search path
aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC_LIST)
# add argparse source files
aux_source_directory(${PROJECT_SOURCE_DIR}/argparse/src/argparse ARGPARSE_SRC_LIST)


# search all header files
file(GLOB_RECURSE HEADER_LIST ${PROJECT_SOURCE_DIR}/include/*.h)
# add argparse header files
file(GLOB_RECURSE ARGPARSE_HEADER_LIST ${PROJECT_SOURCE_DIR}/include/argparse/*.h)


# append header files to source file list
list(APPEND SRC_LIST ${HEADER_LIST})
list(APPEND SRC_LIST ${ARGPARSE_HEADER_LIST})
list(APPEND SRC_LIST ${ARGPARSE_SRC_LIST})

# set output path
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# set target
add_executable(${PROJECT_NAME} ${SRC_LIST})
# link packages
target_link_libraries(${PROJECT_NAME} ${Python3_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${Python3_NumPy_LIBRARIES})


