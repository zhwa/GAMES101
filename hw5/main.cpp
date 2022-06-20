#include "Scene.hpp"
#include "Sphere.hpp"
#include "Triangle.hpp"
#include "Light.hpp"
#include "Renderer.hpp"

// In the main function of the program, we create the scene (create objects and lights)
// as well as set the options for the render (image width and height, maximum recursion
// depth, field-of-view, etc.). We then call the render function().
int main()
{
    Scene scene(1280, 960);

    auto sph1 = std::make_unique<Sphere>(Vector3f(-1.F, 0, -12.F), 2.F);
    sph1->materialType = MaterialType::DIFFUSE_AND_GLOSSY;
    sph1->diffuseColor = Vector3f(0.6, 0.7, 0.8);

    auto sph2 = std::make_unique<Sphere>(Vector3f(0.5F, -0.5F, -8.F), 1.5F);
    sph2->ior = 1.5;
    sph2->materialType = MaterialType::REFLECTION_AND_REFRACTION;

    scene.Add(std::move(sph1));
    scene.Add(std::move(sph2));

    Vector3f verts[4] = {{-5.F,-3.F,-6.F}, {5.F,-3.F,-6.F}, {5.F,-3.F,-16.F}, {-5.F,-3.F,-16.F}};
    uint32_t vertIndex[6] = {0, 1, 3, 1, 2, 3};
    Vector2f st[4] = {{0.F, 0.F}, {1.F, 0.F}, {1.F, 1.F}, {0.F, 1.F}};
    auto mesh = std::make_unique<MeshTriangle>(verts, vertIndex, 2, st);
    mesh->materialType = MaterialType::DIFFUSE_AND_GLOSSY;

    scene.Add(std::move(mesh));
    scene.Add(std::make_unique<Light>(Vector3f(-20.F, 70.F, 20.F), 0.5F));
    scene.Add(std::make_unique<Light>(Vector3f(30.F, 50.F, -12.F), 0.5F));

    Renderer r;
    r.Render(scene);

    return 0;
}