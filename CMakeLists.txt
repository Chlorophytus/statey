cmake_minimum_required(VERSION 3.19)

project(statey VERSION 0.2.0.0)
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

# set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -lmvec -lm -msse4 -ffast-math")
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lmvec -lm -msse4 -ffast-math")

add_library(${PROJECT_NAME}
    src/${PROJECT_NAME}.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 0
    PUBLIC_HEADER include/statey.hpp)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED TRUE)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)


target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
