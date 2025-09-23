
aux_source_directory(. SRC_FILES)

add_executable(001_QOpenGLWidget ${SRC_FILES})

target_link_libraries(001_QOpenGLWidget PRIVATE Qt6::OpenGLWidgets)
