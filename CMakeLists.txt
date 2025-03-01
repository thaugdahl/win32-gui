cmake_minimum_required(VERSION 3.13)
project(main LANGUAGES C CXX)

add_executable(main WIN32
    src/main.cpp
    src/WindowIDHandler.cpp
    src/MainWindow.cpp
    src/WindowClassManager.cpp
    src/GLViewport.cpp
 "src/GLViewport.cpp")

set_property(TARGET main PROPERTY CXX_STANDARD 17)
find_package(OpenGL REQUIRED)

target_link_libraries(main User32)
target_link_libraries(main OpenGL::GL)
