
aux_source_directory(. SRC_FILES)

add_executable(004_MVPMatrix ${SRC_FILES})

target_link_libraries(004_MVPMatrix PRIVATE Qt6::OpenGLWidgets glm::glm)
