// Created by Göksu Güvendiren on 2019-05-14.
#include "Scene.hpp"

void Scene::buildBVH()
{
    printf(" - Generating BVH...\n\n");
    bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::intersect(const Ray &ray) const
{
    return bvh->Intersect(ray);
}

void Scene::sampleLight(Intersection &pos, float &pdf) const
{
    float emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k)
    {
        if (objects[k]->hasEmit())
        {
            emit_area_sum += objects[k]->getArea();
        }
    }
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k)
    {
        if (objects[k]->hasEmit())
        {
            emit_area_sum += objects[k]->getArea();
            if (p <= emit_area_sum)
            {
                objects[k]->Sample(pos, pdf);
                break;
            }
        }
    }
}

bool Scene::trace(const Ray &ray, const std::vector<Object*> &objects, float &tNear, uint32_t& index, Object** hitObject)
{
    *hitObject = nullptr;
    for (uint32_t k = 0; k < objects.size(); ++k)
    {
        float tNearK = kInfinity;
        uint32_t indexK;
        Vector2f uvK;
        if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear)
        {
            *hitObject = objects[k];
            tNear = tNearK;
            index = indexK;
        }
    }
    return (*hitObject != nullptr);
}

// Implementation of Path Tracing
Vector3f Scene::castRay(const Ray &ray, int depth) const
{
    // TO DO Implement Path Tracing Algorithm here
    // Based on sudo code in Assignment7.pdf, https://github.com/Quanwei1992/GAMES101/blob/master/07/Scene.cpp
    // and https://github.com/kingiluob/Games101/blob/master/Assignment7/Scene.cpp
    Intersection intersection = intersect(ray);
    if (!intersection.happened)
    {
        return { 0 };
    }
    if (intersection.m->hasEmission())
    {
        return intersection.m->getEmission();
    }

    auto wo = normalize(-ray.direction);
    auto p = intersection.coords;
    auto N = normalize(intersection.normal);
    constexpr float epsilon = 0.0005F;

    float pdf_light{ 0 };
    Intersection inter;
    sampleLight(inter, pdf_light);
    auto x = inter.coords;
    auto r = x - p;
    auto ws = normalize(r);
    auto NN = normalize(inter.normal);

    // Direct light
    Vector3f L_dir{ 0 };
    if ((intersect(Ray(p, ws)).coords - x).norm() < epsilon)
    {
        auto f_r = intersection.m->eval(wo, ws, N);
        auto r2 = dotProduct(r, r);
        auto cos_a = std::max(0.F, dotProduct(ws, N));
        auto cos_b = std::max(0.F, dotProduct(-ws, NN));
        L_dir = inter.emit * f_r * cos_a * cos_b  / (r2 * pdf_light);
    }

    // Indirect light
    Vector3f L_indir{ 0 };
    if (get_random_float() < Scene::RussianRoulette)
    {
        auto wi = intersection.m->sample(wo, N);
        auto f_r = intersection.m->eval(wi, wo, N);
        auto cos = std::max(0.F, dotProduct(wi, N));
        L_indir = castRay(Ray(p, wi), depth) * f_r * cos / (intersection.m->pdf(wi, wo, N) * Scene::RussianRoulette);
    }
    
    return L_indir + L_dir;
}