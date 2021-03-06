# NOTE: The whole project is meant for linux only
# NOTE: Install both ZED SDK and DarkHelp
cmake_minimum_required (VERSION 3.5)

project(vision)
enable_testing()

# Variables scopes follow standard rules
# Variables defined here will carry over to its children, ergo subdirectories

# Setup ZED libs
find_package(ZED 3 REQUIRED)
include_directories(${ZED_INCLUDE_DIRS})
link_directories(${ZED_LIBRARY_DIR})

# Setup CUDA libs for zed and ai modules
find_package(CUDA ${ZED_CUDA_VERSION} REQUIRED)
include_directories(${CUDA_INCLUDE_DIRS})
link_directories(${CUDA_LIBRARY_DIRS})

# Setup OpenCV libs
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INLCUDE_DIRS})

# Setup Darknet libs
find_library(DARKNET_LIBRARY NAMES dark libdark.so libdarknet.so)
#find_package(dark REQUIRED)

# Setup HTTP libs
find_package(httplib REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)

# System libs
SET(SPECIAL_OS_LIBS "pthread")
link_libraries(stdc++fs)

# Optional definitions
add_definitions(-std=c++17 -g -O3)

# Add sub directories
add_subdirectory(zed_module)
add_subdirectory(ai_module)
add_subdirectory(http_api_module)
add_subdirectory(executable_module)

option(RUN_TESTS "Build the tests" off)

if (RUN_TESTS OR CMAKE_BUILD_TYPE MATCHES Debug)
    add_subdirectory(test)
endif()