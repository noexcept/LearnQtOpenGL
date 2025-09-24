cmake_minimum_required(VERSION 3.16)

project(LearnQtOpenGL)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets OpenGLWidgets REQUIRED)

add_subdirectory(001_QOpenGLWidget)
add_subdirectory(002_Triangle)
add_subdirectory(003_ThreeTriangleEBO)
