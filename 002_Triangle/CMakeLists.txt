
aux_source_directory(. SRC_FILES)

add_executable(002_Triangle ${SRC_FILES})

target_link_libraries(002_Triangle PRIVATE Qt6::OpenGLWidgets)
