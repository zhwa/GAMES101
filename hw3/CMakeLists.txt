project(Rasterizer3)

add_executable(${PROJECT_NAME}
    main.cpp
    rasterizer.hpp
    rasterizer.cpp
    Triangle.hpp
    Triangle.cpp
    Texture.hpp
    Shader.hpp
    OBJ_Loader.h)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${eigen_SOURCE_DIR}
    ${OPENCV_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC
    ${OPENCV_LIB_DIR}/opencv_world460.lib)

# Copy OpenCV dll and model files
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OPENCV_BIN_DIR}/opencv_world460.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/models $<TARGET_FILE_DIR:${PROJECT_NAME}>/models)