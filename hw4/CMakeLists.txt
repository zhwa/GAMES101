project(BezierCurve)

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${eigen_SOURCE_DIR}
    ${OPENCV_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC
    ${OPENCV_LIB_DIR}/opencv_world460.lib)

# Copy OpenCV dll
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OPENCV_BIN_DIR}/opencv_world460.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>)