
aux_source_directory(. SRC_FILES)

add_executable(003_ThreeTriangleEBO ${SRC_FILES})

target_link_libraries(003_ThreeTriangleEBO PRIVATE Qt6::OpenGLWidgets)
