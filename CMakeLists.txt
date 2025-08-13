cmake_minimum_required(VERSION 3.10)
project(seal_project)

set(CMAKE_CXX_STANDARD 17)
set(SEAL_DIR ../SEAL)

# Find packages
find_package(Eigen3 REQUIRED)

# Add SEAL subdirectory
add_subdirectory(${SEAL_DIR} ${CMAKE_BINARY_DIR}/seal_build)
include_directories(${SEAL_DIR}/native/src)

# Get all cpp files
file(GLOB SRC_FILES *.cpp)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Create executables for each cpp file and link both SEAL and Eigen
foreach(src ${SRC_FILES})
    get_filename_component(exec_name ${src} NAME_WE)
    add_executable(${exec_name} ${src})
    target_link_libraries(${exec_name} SEAL::seal Eigen3::Eigen)
endforeach()
