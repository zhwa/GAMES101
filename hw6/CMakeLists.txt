project(RayTracing2)

add_executable(${PROJECT_NAME} main.cpp Object.hpp Vector.cpp Vector.hpp Sphere.hpp Triangle.hpp Scene.cpp
        Scene.hpp Light.hpp AreaLight.hpp BVH.cpp BVH.hpp Bounds3.hpp Ray.hpp Material.hpp Intersection.hpp
        Renderer.cpp Renderer.hpp)

# Copy model file
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/models/bunny $<TARGET_FILE_DIR:${PROJECT_NAME}>/models/bunny)