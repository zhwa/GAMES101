project(RayTracing1)

add_executable(${PROJECT_NAME}
    main.cpp
    Object.hpp
    Vector.hpp
    Sphere.hpp
    Triangle.hpp
    Scene.cpp
    Scene.hpp
    Light.hpp
    Renderer.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC WindowsApp.lib)