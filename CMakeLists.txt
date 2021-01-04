cmake_minimum_required(VERSION 3.19)

project(statey VERSION 0.1.0.0)
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)
pkg_check_modules(rtaudio REQUIRED rtaudio)
pkg_check_modules(rtmidi REQUIRED rtmidi)

add_executable(${PROJECT_NAME}
    src/${PROJECT_NAME}_audio.cpp
    src/${PROJECT_NAME}_midi.cpp
    src/${PROJECT_NAME}_object.cpp
    src/${PROJECT_NAME}.cpp)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED TRUE)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME} PRIVATE
    Threads::Threads
    ${rtaudio_LIBRARIES}
    ${rtmidi_LIBRARIES})
